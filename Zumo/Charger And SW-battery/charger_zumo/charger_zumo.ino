/*
* Modified version of the Zumo32U4 example "LineFollower" to work with the IR receiver
* and react to commands from the IR remote.
*/

#include <Wire.h>
#include <Zumo32U4.h>
#define DECODE_NEC
#include <IRremote.hpp>
#include <Zumo32U4IRPulses.h>

#define SUBCARIER_PERIOD 420  // The default frequency is 16000000 / (420 + 1) = 38.005 kHz
#define IR_BRIGHNESS 100      // 0-400, 0 is off, 400 is max brightness
Zumo32U4IRPulses::Direction IR_DIRECTION = Zumo32U4IRPulses::Left; // Direction of the IR LED

#define IR_RECEIVE_PIN 22   // pin for IR receiver, 22 is the front proximity sensor

#define DEVICE_ID 2  // ID of the device, used to identify the robot

// IR-Sensor commands the robot either sends or recieves.
#define RED_LIGHT_COMMAND 52          // Commands for traffic light states
#define YELLOW_LIGHT_COMMAND 53
#define GREEN_LIGHT_COMMAND 54

#define COMMAND_IDENTIFY 0xF0         // Command for telling the robot to identify itself
#define COMMAND_TOL_STATION 0xF1      // Command for telling the robot it's at a tol station
#define COMMAND_CHARGE_STATION 0xF2   // Command telling the robot it is at a charger station

#define COMMAND_OPEN 0x01             // Command for telling the tol station to open the gate
#define COMMAND_IDENTIFYING 0x02      // Command for identifying itself

#define COMMAND_CHARGE 0xC1           // Command for telling the charge station to charge the robot
#define COMMAND_CHARGE_COMPETE 0xC2   // Command for telling the charge station the charge is completed
#define COMMAND_MANUAL_CHARGE 0xC3    // Command for manually telling the robot to charge at the next pass

unsigned long recivedCommandTime = 0;
int CommandToAnswer;
int senderID;
bool newCommand = false;

#define ANSWER_DELAY 30 // Delay before answering a command, needed for the IR receiver to be ready

const int authorisedDevices[] = { 200, 201, 202 }; // List of authorised devices

#define YELLOW_LED 13 // On-robot LED light

/*  This is the maximum speed the motors will be allowed to turn.
    A maxSpeed of 400 lets the motors go at top speed.  
    Decrease this value to impose a speed limit. */
uint16_t maxSpeed = 400;

Zumo32U4Buzzer buzzer;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4OLED display;
Zumo32U4Encoders encoders;

// Traffic light variables
enum trafficLightState { RED, YELLOW, GREEN }; // Traffic light states
trafficLightState lightState = GREEN; 

#define YELLOW_SLOW_TIME 2000  // Time to drive slowly after yellow light
unsigned long yellowStartTime = 0;

int16_t lastError = 0;

#define NUM_SENSORS 5   
unsigned int lineSensorValues[NUM_SENSORS];

// SW-Battery Variables
enum batteryState { OK, BAD, CRITICAL }; // Battery states
batteryState batState = OK;

unsigned long lastDrain = 0;
bool charging = false;

int batteryLevel = 255; // Values for battery, edit to change startvalue, maxvalue and thresholds.
int batteryMax = 255;
int chargeLimit = 150;
int criticalLimit = 50;

// Function declarations
void updateBattery();
void drainBattery();

// Sets up special characters for the display so that we can show
// bar graphs.
void loadCustomCharacters() {
  static const char levels[] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 63, 63, 63, 63, 63, 63, 63
  };
  display.loadCustomCharacter(levels + 0, 0);  // 1 bar
  display.loadCustomCharacter(levels + 1, 1);  // 2 bars
  display.loadCustomCharacter(levels + 2, 2);  // 3 bars
  display.loadCustomCharacter(levels + 3, 3);  // 4 bars
  display.loadCustomCharacter(levels + 4, 4);  // 5 bars
  display.loadCustomCharacter(levels + 5, 5);  // 6 bars
  display.loadCustomCharacter(levels + 6, 6);  // 7 bars
}

void printBar(uint8_t height) {
  if (height > 8) { height = 8; }
  const char barChars[] = { ' ', 0, 1, 2, 3, 4, 5, 6, (char)255 };
  display.print(barChars[height]);
}

void calibrateSensors() {
  display.clear();

  // Wait 1 second and then begin automatic sensor calibration
  // by rotating in place to sweep the sensors over the line
  delay(1000);

  for (int i = 0; i < 200; i++) {  // ramp up
    motors.setSpeeds(i, -i);
    delay(1);
  }
  for (uint16_t i = 0; i < 120; i++) {

    if (i == 30) {  // Speed ramp
      for (int x = -200; x < 200; x++) {
        motors.setSpeeds(-x, x);
        delay(1);
      }
    } else if (i == 90) {  // Speed ramp
      for (int x = -200; x < 200; x++) {
        motors.setSpeeds(-x, x);
        delay(1);
      }
    } else if (i > 30 && i < 90) {
      motors.setSpeeds(-200, 200);
    } else {
      motors.setSpeeds(200, -200);
    }

    lineSensors.calibrate();
  }
  motors.setSpeeds(0, 0);
}

// Shows a bar graph of sensor readings on the display.
// Returns after the user presses A.
void showReadings() {
  display.clear();

  while (!buttonA.getSingleDebouncedPress()) {
    lineSensors.readCalibrated(lineSensorValues);

    display.gotoXY(0, 0);
    for (uint8_t i = 0; i < NUM_SENSORS; i++) {
      uint8_t barHeight = map(lineSensorValues[i], 0, 1000, 0, 8);
      printBar(barHeight);
    }
  }
}

// function to read the IR receiver
void readIR() {
  if (!IrReceiver.decode()) { 
    return;     // Nothing new to read, so we exit the function
  }
  IrReceiver.resume(); // Enable receiving of the next value
  if (IrReceiver.decodedIRData.command == 0) {
    return; // We have a bad reading, so we exit the function
  } else if (IrReceiver.decodedIRData.command == RED_LIGHT_COMMAND) {
    lightState = RED;
  } else if (IrReceiver.decodedIRData.command == YELLOW_LIGHT_COMMAND) {
    lightState = YELLOW;
    yellowStartTime = millis();
  } else if (IrReceiver.decodedIRData.command == GREEN_LIGHT_COMMAND) {
    lightState = GREEN;
  } else if (IrReceiver.decodedIRData.command == COMMAND_IDENTIFY) {
    // If we recive a command to identify, we set the parameters for answering
    recivedCommandTime = millis();
    CommandToAnswer = COMMAND_IDENTIFY;
    digitalWrite(YELLOW_LED, HIGH);
    senderID = IrReceiver.decodedIRData.address;
    newCommand = true;
  } else if (IrReceiver.decodedIRData.command == COMMAND_TOL_STATION){
    // If we recive a command for a tolstation, we set the parameters for answering
    recivedCommandTime = millis();
    CommandToAnswer = COMMAND_TOL_STATION;
    senderID = IrReceiver.decodedIRData.address;  
    newCommand = true;
  } else if (IrReceiver.decodedIRData.command == COMMAND_CHARGE_STATION){
    // If we recive a command for a charge station, we set the parameters for answering
    recivedCommandTime = millis();
    CommandToAnswer = COMMAND_CHARGE_STATION;
    digitalWrite(YELLOW_LED, HIGH);
    senderID = IrReceiver.decodedIRData.address;
    newCommand = true;
  } else if (IrReceiver.decodedIRData.command == COMMAND_MANUAL_CHARGE){
    // If we recive a command for a manual charge, we set the parameters for answering
    recivedCommandTime = millis();
    CommandToAnswer = COMMAND_MANUAL_CHARGE;
    digitalWrite(YELLOW_LED, HIGH);
    senderID = IrReceiver.decodedIRData.address;
    newCommand = true;
  }
}

// Function to update the speed of the robot based on the traffic light state
void updateSpeed() {
  switch (lightState) {
    case RED:
      maxSpeed = 0;
      break;
    case YELLOW:
      maxSpeed = 100;
      if (millis() - yellowStartTime >= YELLOW_SLOW_TIME) {
        lightState = GREEN;
      }
      break;
    case GREEN:
      maxSpeed = 400;
      break;
  }
}

// SW-Battery
void updateBattery(){ // Core function for battery behaviour
  switch(charging){
    case false: // When battery is draining
      drainBattery(); 
      break;
    case true: // When battery is charging
      if(batteryLevel < batteryMax){ // As long as battery is lower than max value
        sendCommand(DEVICE_ID, COMMAND_CHARGE, IR_DIRECTION); // Send order command to charge station
        delay(100);
        batteryLevel++;
        int batteryLevelPercent = map(batteryLevel, 0, 255, 0, 100);
        display.clear();
        display.print(String(batteryLevelPercent)); display.print("% ");
        display.print(String(speedTotal)); display.print("m/s");
      } else {
        batState = OK;
        sendCommand(DEVICE_ID, COMMAND_CHARGE_COMPETE, IR_DIRECTION); // Tells charge station that the robot is fully charged
        lightState = GREEN;
        charging = false;
        IrReceiver.start(); // Enable receiving of the next value
      }
      break;
  }
}

void drainBattery(){
  // Basic battery drain function based on made up values and robot speed
  if((millis() - lastDrain > 1000) && batteryLevel > 0){
    // Basic battery drain based only on maxSpeed set by lightState.
    //batteryLevel -= (maxSpeed/40); 

    // Battery drain based robots encoders measuring counts
    int16_t countLeft = encoders.getCountsAndResetLeft();
    int16_t countRight = encoders.getCountsAndResetRight();
        
    // Serial.println(rotLeft);
    // Serial.println(rotRight);

    /*  According to the documentation does the encoders provide 12 counts per revolution of the motor shaft
        and the motors have a 75:1 ratio (more precisely 75.81:1)
        in other words the encoders have a 909.7 counts per revolution for the motors (75.81 * 12) */
    
    double revLeft = (countLeft/909.7); // As this function runs every second
    double revRight = (countRight/909.7); // This value should be RPS (rotations per second)
    // Serial.println(revLeft);
    // Serial.println(revRight);
    
    double revTotal = revLeft + revRight; // Calculate rotations of both motors
    Serial.println(revTotal); 
    batteryLevel -= revTotal; // Calculates drain based on total rotations

    // V (velocity) = (pi/2)*(D*RPS)
    // D (diameter) = 0.039m
    double speedLeft = (3.14/2)*0.039*revLeft; // Calculate speed left wheels
    double speedRight = (3.14/2)*0.039*revRight; // Calculate speed of right wheels
    // Serial.println(speedLeft);
    // Serial.println(speedRight);
    double speedTotal = (speedLeft+speedRight)/2; // Calculate total speed of robot
    Serial.println(speedTotal);

    lastDrain = millis();

    if(batteryLevel < chargeLimit){ // Only ask for charge automatically when charge is lower than chargeLimit
      Serial.println("Needs charge");
      batState = BAD;
    }

    if(batteryLevel < criticalLimit){ // Make robot drive slower when battery is critically low
      Serial.println("Battery critical");
      batState = CRITICAL;
      lightState = YELLOW;
      yellowStartTime = millis();
    }
    
    int batteryLevelPercent = map(batteryLevel, 0, 255, 0, 100); // Convert batteryvalue to percent
    display.clear();
    display.print(String(batteryLevelPercent)); display.print("% ");
    display.print(String(speedTotal)); display.print("m/s");
        
    Serial.println(batteryLevel);
    // Serial.println(batState);
  }
}

// Function for handling received commands
void handleCommand(uint16_t senderID, uint16_t command) {
  if (isAuthorised(senderID)) {
    if (command == COMMAND_IDENTIFY) {
      Serial.println("Identify");
      sendCommand(DEVICE_ID, COMMAND_IDENTIFYING, IR_DIRECTION);   // Send device ID and identifying command
    } else if (command == COMMAND_TOL_STATION) {
      Serial.println("Tol station");
      sendCommand(DEVICE_ID, COMMAND_OPEN, IR_DIRECTION); //Send open command
    } else if (command == COMMAND_CHARGE_STATION){
      Serial.println("Charge station");
      if(batState != OK){ // If battery is low enough
        lightState = RED; // Stop robot
        charging = true; // Initiate battery charging
      }
    } else if (command == COMMAND_MANUAL_CHARGE){ // Initiate charge regardless of battery level
      Serial.println("Manual Charge");
      lightState = RED;
      charging = true;
    }
  }
}

// Function for checking if the sender is an authorised device
bool isAuthorised(uint16_t senderID) {
  for (int i = 0; i < (sizeof(authorisedDevices) / sizeof(authorisedDevices[0])); i++) {
    if (senderID == authorisedDevices[i]) {
      Serial.println("Is authorised");
      return true;
      ;
    }
  }
  Serial.println("Not authorised");
  return false;
}

// Function for sending 8 bit command in acordance with the NEC protocol
void sendCommand(uint8_t deviceID, uint8_t command, Zumo32U4IRPulses::Direction direction) {
  Serial.print("Sending : ");
  Serial.print(command);
  Serial.print(" From : ");
  Serial.println(deviceID);
  // Send a start pulse. high for 9ms, low for 4.5ms
  //Serial.println("StartPulse");
  startPulse(direction);
  delayMicroseconds(9000);
  Zumo32U4IRPulses::stop();
  delayMicroseconds(4500);

  // Send device ID.
  sendByte(deviceID, direction);

  // Send inverted device ID.
  sendByte(~deviceID, direction);

  // Send command.
  sendByte(command, direction);

  // Send inverted command.
  sendByte(~command, direction);

  // Send stop pulse. high for 562.5us, low for 562.5us
  //Serial.println("StopPulse");
  startPulse(direction);
  delayMicroseconds(560);
  Zumo32U4IRPulses::stop();
  delayMicroseconds(560);
}

// Send 8 bit byte in acordance with the NEC protocol
void sendByte(uint8_t byte, Zumo32U4IRPulses::Direction direction) {
  for (uint8_t i = 0; i < 8; i++) {   // iterate through the bits in the byte, LSB first
    uint8_t bit = (byte >> (i)) & 1;  // get the current bit
    if (bit == 1) {                   // if the bit is 1, send 1
      //Serial.print("1");
      startPulse(direction);
      delayMicroseconds(560);
      Zumo32U4IRPulses::stop();
      delayMicroseconds(1690);
    } else {  // if the bit is 0, send 0
      //Serial.print("0");
      startPulse(direction);
      delayMicroseconds(560);
      Zumo32U4IRPulses::stop();
      delayMicroseconds(560);
    }
  }
  //Serial.println("");
}

// Start the IR pulse with settings for subcarrier
void startPulse(Zumo32U4IRPulses::Direction direction) {
  Zumo32U4IRPulses::start(direction, IR_BRIGHNESS, SUBCARIER_PERIOD);  // Turn on IR LED int the specified direction with subcarrier
}

void setup() {
  // Uncomment if necessary to correct motor directions:
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);
  Serial.begin(9600);

  IrReceiver.begin(IR_RECEIVE_PIN);
  pinMode(YELLOW_LED, OUTPUT);

  lineSensors.initFiveSensors();
  //lineSensors.initThreeSensors();

  loadCustomCharacters();

  // Play a little welcome song
  //buzzer.play(">g32>>c32");

  // Wait for button A to be pressed and released.
  display.clear();
  display.print(F("Press A"));
  display.gotoXY(0, 1);
  display.print(F("to calib"));
  buttonA.waitForButton();

  //calibrateSensors();

  showReadings();

  // Play music and wait for it to finish before we start driving.
  display.clear();
  display.print(F("Go!"));
  //buzzer.play("L16 cdegreg4");
  //while (buzzer.isPlaying());
}

void loop() {
  readIR();
  updateSpeed();
  updateBattery();

  if (millis() - recivedCommandTime > ANSWER_DELAY && newCommand == true) {
    handleCommand(senderID, CommandToAnswer);
    newCommand = false;
    IrReceiver.start();  // Enable receiving of the next value
  }
  
  // Get the position of the line.  Note that we *must* provide
  // the "lineSensorValues" argument to readLine() here, even
  // though we are not interested in the individual sensor
  // readings.
  int16_t position = lineSensors.readLine(lineSensorValues);
  //Serial.print("Pos:");
  //Serial.print(position);

  // Our "error" is how far we are away from the center of the
  // line, which corresponds to position 2000.
  int16_t error = position - 2000;
  //Serial.print("\t err:");
  //Serial.print(error);

  // Get motor speed difference using proportional and derivative
  // PID terms (the integral term is generally not very useful
  // for line following).  Here we are using a proportional
  // constant of 1/4 and a derivative constant of 6, which should
  // work decently for many Zumo motor choices.  You probably
  // want to use trial and error to tune these constants for your
  // particular Zumo and line course.
  int16_t speedDifference = error / 4 + 6 * (error - lastError);
  //Serial.print("\t dif:");
  //Serial.println(speedDifference);

  lastError = error;

  // Get individual motor speeds.  The sign of speedDifference
  // determines if the robot turns left or right.
  int16_t leftSpeed = (int16_t)maxSpeed + speedDifference;
  int16_t rightSpeed = (int16_t)maxSpeed - speedDifference;

  // Constrain our motor speeds to be between 0 and maxSpeed.
  // One motor will always be turning at maxSpeed, and the other
  // will be at maxSpeed-|speedDifference| if that is positive,
  // else it will be stationary.  For some applications, you
  // might want to allow the motor speed to go negative so that
  // it can spin in reverse.
  leftSpeed = constrain(leftSpeed, 0, (int16_t)maxSpeed);
  rightSpeed = constrain(rightSpeed, 0, (int16_t)maxSpeed);

  //motors.setSpeeds(leftSpeed, rightSpeed);
}