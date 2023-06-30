
#include <Servo.h>  // We want to use the servo library

Servo myservo;      // Call the servo object

const int X_PIN = A0;
const int Y_PIN = A1;
const int PRESS_PIN = 8;
const int SERVO_PIN = 9;

int xValue = 0;
int yValue = 0;

// Define functions so the code know it exists
void updateJoystick();
void updateServo();

void setup() {
  Serial.begin(9600);
  pinMode(PRESS_PIN, INPUT_PULLUP);

  myservo.attach(SERVO_PIN); // Make servo used defined pin

}

void loop() {
  updateJoystick();
  updateServo();
}

void updateJoystick(){
  xValue = analogRead(X_PIN); // Read potensiometer value of joystick x axis
  yValue = analogRead(Y_PIN); // Read potensiometer value of joystick y axis
  //Serial.print("X: "); Serial.print(xValue);
  //Serial.print(" Y: "); Serial.println(yValue);
  
  yValue = map(yValue, 0, 1023, 0, 180); // Map the y values so they fix within 180°
  //Serial.println(yValue);
}

void updateServo(){
  if(xValue >= 500){ // If the joystick is pointed upwards
    myservo.write(yValue);
  }
}