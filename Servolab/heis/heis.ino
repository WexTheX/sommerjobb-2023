
#include <Stepper.h>

#define STEPS_PER_REVOLUTION 200

#define L293D_IN1_PIN 8
#define L293D_IN2_PIN 9
#define L293D_IN3_PIN 10
#define L293D_IN4_PIN 11

Stepper myStepper(STEPS_PER_REVOLUTION, L293D_IN1_PIN, L293D_IN2_PIN, L293D_IN3_PIN, L293D_IN4_PIN);

const int BUTTON_PIN_1 = 6;
const int BUTTON_PIN_2 = 7;

enum elevatorMovement { STAND, UP, DOWN };
elevatorMovement elevatorState = STAND;

// Function declarations
void updateButton();
void updateMotor();

void setup() {
  Serial.begin(9600);
  myStepper.setSpeed(100);
  pinMode(BUTTON_PIN_1, INPUT);
  pinMode(BUTTON_PIN_2, INPUT);
}

void loop() {
  updateButton();
  updateMotor();
}

void updateButton(){
  if(digitalRead(BUTTON_PIN_1)){
    elevatorState = UP;
  } else if (digitalRead(BUTTON_PIN_2)){
    elevatorState = DOWN;
  } else {
    elevatorState = STAND;
  }
}

void updateMotor(){
  switch(elevatorState){
    case STAND:
      break;
    case UP:
      myStepper.step(STEPS_PER_REVOLUTION);
      break;
    case DOWN:
      myStepper.step(-STEPS_PER_REVOLUTION);
      break;
  }
}