
#include <Stepper.h>

#define STEPS_PER_REVOLUTION 200

#define L293D_IN1_PIN 8
#define L293D_IN2_PIN 9
#define L293D_IN3_PIN 10
#define L293D_IN4_PIN 11

Stepper myStepper(STEPS_PER_REVOLUTION, L293D_IN1_PIN, L293D_IN2_PIN, L293D_IN3_PIN, L293D_IN4_PIN);

void setup() {
  myStepper.setSpeed(100);
}

void loop() {
  // Rotate one revolution
   myStepper.step(STEPS_PER_REVOLUTION);
   delay(1000);
   // Rotate back
   myStepper.step(-STEPS_PER_REVOLUTION);
   delay(1000);
}
