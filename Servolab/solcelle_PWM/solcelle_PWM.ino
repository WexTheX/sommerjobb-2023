
const int LPR_PIN     = A0; // Define three analog pins for photoresistors
const int MPR_PIN     = A1;
const int RPR_PIN     = A2;
const int SERVO_PIN   = 9;  // Define PWM-compatible digital pin for servo (On Arduino UNO: 3, 5, 6, 9, 10, 11)

// Variables
unsigned long prevMil = 0;

int LPR = 0; // Variables for photoresistiors (left, middle, right)
int MPR = 0;
int RPR = 0;

enum position { MIDDLE, LEFT, RIGHT }; // To define position the strongest light is coming from
position lightState = MIDDLE;

int servoPos = 150; // Servo has a range of 50 - 250, so 150 should be middle

// Function declarations so the program knows these functions exist
void updateSunPosition();
void updateServoPosition();

void setup() {
  Serial.begin(9600);
  pinMode(SERVO_PIN, OUTPUT);
}

void loop() {
  if(millis() - prevMil > 10){ // Only run after a set interval (mostly to read debug prints easier)
    updateSunPosition();
    updateServoPosition();

    prevMil = millis();
  }
}

void updateSunPosition(){ // Function for updating the position of the strongest light
  LPR = analogRead(LPR_PIN);
  MPR = analogRead(MPR_PIN);
  RPR = analogRead(RPR_PIN);

    // Print photoresistor values for debugging
  Serial.print("L: "); Serial.print(LPR);
  Serial.print(" M: "); Serial.print(MPR);
  Serial.print(" R: "); Serial.println(RPR);

  if((MPR+100 >= LPR) && (MPR+100 >= RPR)){ // If the light is strongest in the middle (+ a buffer to avoid constant shifting back and fourth)
    lightState = MIDDLE;
  } else {
    if(LPR > RPR){                  // If the light is strongest on the left side
      lightState = LEFT;
    } else if (RPR > LPR){          // If the light is strongest on the right side
      lightState = RIGHT;
    }
  }
}

void updateServoPosition(){  // Function for moving the servo to correct position
  switch(lightState){
    case MIDDLE:
      break;
    case LEFT:
      servoPos -= 1; // Move towards 50, aka to the servos left
      break;
    case RIGHT:
      servoPos += 1; // Move towards 50, aka to the servos left
      break;
  }

  servoPos = constrain(servoPos, 50, 250);  // Constrain servoPos to within the servos range
  if(lightState != MIDDLE){
    analogWrite(SERVO_PIN, servoPos);  // Write the servoPos PWM signal to the servo
  }
  //Serial.print("servoPos: "); Serial.println(servoPos);
}