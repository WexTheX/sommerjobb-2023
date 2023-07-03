
#include <Servo.h> // We want to use the servo library

Servo topservo; // Declare top servo object
Servo botservo; // Declare bottom servo object

// Pin definitions
const int NPR_PIN = A0;
const int SPR_PIN = A2;
const int EPR_PIN = A3;
const int WPR_PIN = A1;

const int SERVO_PIN_TOP = 9;
const int SERVO_PIN_BOT = 10;

unsigned long prevMil = 0;

int NPR = 0; // Values for photoresistors (North, South, East, West)
int SPR = 0; // Note these are names, if they acutally align with magnetic poles depends on how the physical structure is set up.
int EPR = 0;
int WPR = 0;

int servoPosTop = 90; // Set starting position of servos
int servoPosBot = 90;

enum servoState { MIDDLE, LEFT, RIGHT }; // TODO change ifelse to enum
servoState servoTop = MIDDLE;
servoState servoBot = MIDDLE;

// Declare functions
void updateSunPosition();
void updateServoPosition();
void updateServoPositionTop();
void updateServoPositionBot();

void setup() {
  Serial.begin(9600);
  topservo.attach(SERVO_PIN_TOP); // Attach pins to servo objects
  botservo.attach(SERVO_PIN_BOT);

}

void loop() {
  updateSunPosition();
  if(millis() - prevMil > 100){ // Only move servo within a given interval

    updateServoPosition();
    prevMil = millis();
  }
}

void updateSunPosition(){ // Get light intensity values
  NPR = analogRead(NPR_PIN);
  SPR = analogRead(SPR_PIN);
  EPR = analogRead(EPR_PIN);
  WPR = analogRead(WPR_PIN);

  /* Serial.print("N: "); Serial.print(NPR);
  Serial.print(" S: "); Serial.print(SPR);
  Serial.print(" E: "); Serial.print(EPR);
  Serial.print(" W: "); Serial.println(WPR); */
}

void updateServoPosition(){ // Combine servo update functions
  updateServoPositionTop();
  updateServoPositionBot();
}

void updateServoPositionTop(){ // Update position of top servo
  if((NPR+50 > EPR) && (WPR+50 > SPR)){ // If the light is stronger on the North/West side (with buffer)
    servoPosTop -= 1;
  } else if ((NPR < EPR+50) && (WPR < SPR+50)){ // If the light is stronger on the South/East side (with buffer)
    servoPosTop += 1;
  }

  servoPosTop = constrain(servoPosTop, 0, 180); // Makes sure position is within servos range
  //Serial.print("Top"); Serial.println(servoPosTop);
  
  topservo.write(servoPosTop);
}

void updateServoPositionBot(){
  if((NPR+50 > SPR) && (EPR+50 > WPR)){ // If the light is stronger on the North/East side (with buffer)
    servoPosBot += 1;
  } else if ((NPR < SPR+50) && (EPR < WPR+50)){ // If the light is stronger on the South/West side (with buffer)
    servoPosBot -= 1;
  }

  servoPosBot = constrain(servoPosBot, 0, 180); // Make sure position is within servos range
  //Serial.print("Bot: "); Serial.println(servoPosBot);

  botservo.write(servoPosBot);
}