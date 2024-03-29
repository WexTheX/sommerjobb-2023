#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int TEMP_PIN = A1;
const int MOTOR_CCW_PIN = 4;
const int MOTOR_CW_PIN = 5;
const int MOTOR_ENABLE_PIN = 6;

#define TIME_INTERVAL 500 // 500 ms interval between updates
#define DELAY_INTERVAL 5000 // 5 seconds interval for delay, change to increase time before motor turn off

unsigned long prevMil = 0;

int celsius = 0;
int desiredTemp = 21; // Desired temperature, edit to change desired temp
unsigned long stateDelay = 0; // Delay to avoid rapid turning on and off of motor when temp is close to desired
int motorState = false;

// Function declarations needed for the rest of the code to see them before they are described
void printText(String txt);
void writeMotorSpeed(int speed);
void updateMotorSpeed();

void setup() {
  Serial.begin(9600);

  pinMode(MOTOR_CCW_PIN, OUTPUT);
  pinMode(MOTOR_CW_PIN, OUTPUT);
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display(); // Adafruit Logo
  delay(2000); // Pause for 2 seconds

  display.clearDisplay();
}

void loop() {
  celsius = map(((analogRead(TEMP_PIN) - 20) * 3.04), 0, 1023, -40, 125);
    // Change sensor value to degrees celcius

  if (millis() - prevMil > TIME_INTERVAL){ // Print celcius value every TIME_INTERVAL
    Serial.println(celsius);
    printText(String(celsius)); // Change celsius value into a string so it's compatible with print function
  }

  if(celsius > desiredTemp){ // If temperature of higher than desired, tell program to turn on motor
    motorState = true;
    stateDelay = millis();
  } else {
    if (millis() - stateDelay > DELAY_INTERVAL) { // If temperature has been lower than desired for longer than 5 seconds.
      motorState = false;
    }
  }

  updateMotorSpeed(); // Update the motorspeed (turn it on or off in this case)
}

void printText(String txt){ // Display a given string of text
  display.clearDisplay();

  display.setTextSize(4); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(txt);
  display.display();      // Show initial text
}

void writeMotorSpeed(int speed){
  if (speed < 0)
    {
        // Negative speeds set motor clockwise
        digitalWrite(MOTOR_CW_PIN, HIGH);
        digitalWrite(MOTOR_CCW_PIN, LOW);
    }

    if (speed > 0)
    {
        // Positive speeds set motor counterclockwise
        digitalWrite(MOTOR_CW_PIN, LOW);
        digitalWrite(MOTOR_CCW_PIN, HIGH);
    }

    if (speed == 0)
    {
        // Motor stop
        digitalWrite(MOTOR_CW_PIN, LOW);
        digitalWrite(MOTOR_CCW_PIN, LOW);
    }

    // Set the speed
    analogWrite(MOTOR_ENABLE_PIN, speed);
}

void updateMotorSpeed()
{
    switch (motorState)
    {
    // Off
    case false:
        writeMotorSpeed(0);
        break;
    // On  
    case true:
        writeMotorSpeed(255);
        break;
    }
}
