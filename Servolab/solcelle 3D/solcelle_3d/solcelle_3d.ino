
#include <Servo.h> // We want to use the servo library

Servo myservo; 

// Pin definitions
const int NPR_PIN = A0;
const int SPR_PIN = A2;
const int EPR_PIN = A1;
const int WPR_PIN = A3;

const int SERVO_PIN_1 = 9;
const int SERVO_PIN_2 = 10;

int NPR = 0; // Values for photoresistors (North, South, East, West)
int SPR = 0; // Note these are names, if they acutally align with magnetic poles depends on how the physical structure is set up.
int EPR = 0;
int WPR = 0;

int servoPosTop = 90; // Set starting position of servos
int servoPosBot = 90;

enum servoState { MIDDLE, LEFT, RIGHT };
servoState servoTop = MIDDLE;
servoState servoBot = MIDDLE;

void updateSunPosition();

void setup() {
  Serial.print(9600);
  pinMode(SERVO_PIN_1);
  pinMode(SERVO_PIN_2);

}

void loop() {
  // put your main code here, to run repeatedly:

}

void updateSunPosition(){
  NPR = analogRead(NPR_PIN);
  SPR = analogRead(SPR_PIN);
  EPR = analogRead(EPR_PIN);
  WPR = analogRead(WPR_PIN);

  
}
