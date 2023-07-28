#include <Servo.h>

Servo top_right;
Servo top_left;

const int TR_PIN = 3;
const int BR_PIN = 5;

int pos1 = 50;
int pos2 = 130;
bool turn = false;

unsigned long prevMil = 0;

void setup() {
  Serial.begin(9600);
  top_right.attach(TR_PIN);
  top_left.attach(BR_PIN);
}

void loop() {
  if(millis() - prevMil >= 10){
    prevMil = millis();
    if(pos1 >= 130){
      turn = true;
    } else if(pos1 <= 50) {
      turn = false;
    }

    if(turn){
      pos1 -= 1;
      pos2 += 1;
    } else {
      pos1 += 1;
      pos2 -= 1;
    }

    Serial.println(pos1);
    Serial.println(pos2);

    top_right.write(pos1);
    top_left.write(pos2);
  } 
}
