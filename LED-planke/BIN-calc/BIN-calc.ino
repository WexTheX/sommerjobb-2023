
#include <Adafruit_NeoPixel.h>

const int LED_PIN = 6;
const int LED_COUNT = 96;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

String message;
enum Calculation { ERROR, ADDITION, SUBTRACTION }; 
Calculation calc = ERROR;

void updateInput();
void updateSerialInput();
void addition();
void subtraction();
void printToLED(int number, int plank);
String reverseString(String);

void setup() {
  Serial.begin(9600);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
}

void loop() {
  updateInput();
}

void updateInput(){
  updateSerialInput();
}

void updateSerialInput(){
  if (Serial.available() > 0) {
    // read the incoming message:
    message = Serial.readString();
    message.replace(" ", "");
    Serial.println(message);

    String temp;
    uint32_t number1;
    uint32_t number2;

    bool first = true;

    for(int x = 0; x < message.length(); x++){
      if(isDigit(message[x])){
        temp += message[x];
        //Serial.println(temp);
      }
      if(!isDigit(message[x])){
        if(first){
          number1 = temp.toInt();
          temp = "";
          first = false;
        } else {
          number2 = temp.toInt();
        }
      }
    }
    
    //Serial.println(number1);
    //Serial.println(number2);

    if(message.indexOf('+') != -1){
      calc = ADDITION;
    } else if(message.indexOf('-') != -1){
      calc = SUBTRACTION;
    } else {
      calc = ERROR;
    }
    //Serial.println(calc);
    
    if(calc == ADDITION){
      addition(number1, number2);
    } else if(calc == SUBTRACTION){
      subtraction(number1, number2);
    } else {
      Serial.println("ERROR: Input not accepted");
    }
  }
}

void addition(uint32_t x, uint32_t y){
  strip.clear();

  uint32_t z = x + y;
  printToLED(x, 1);
  printToLED(y, 2);
  printToLED(z, 3);
  strip.show(); 

  Serial.print(x); Serial.print(" + "); Serial.print(y); Serial.print(" = "); Serial.println(z);
}

void subtraction(uint32_t x, uint32_t y){
  strip.clear();

  uint32_t z = x - y;
  printToLED(x, 1);
  printToLED(y, 2);
  printToLED(z, 3);
  strip.show(); 

  Serial.print(x); Serial.print(" - "); Serial.print(y); Serial.print(" = "); Serial.println(z);
}

void printToLED(uint32_t number, int plank){
  Serial.print("Number: "); Serial.println(number);
  String n = String(number, BIN);
  n = reverseString(n);
  int start = 32 * (plank - 1);
  
  for(int x = 0; x <= n.length(); x++){
    Serial.print(n[x]);
    if(n[x] == '1'){
      strip.setPixelColor((x + start), strip.Color(127, 127, 127));
    } else if(n[x] == '0'){
      strip.setPixelColor((x + start), strip.Color(127, 0, 0));
    }
  }
  Serial.println("");
}

String reverseString(String text){
  Serial.print("Before: ");Serial.println(text);
  String textRev;
  for(int x = 0; x < text.length(); x++){
    textRev += text[text.length()-1-x];
  };
  Serial.print("After:  "); Serial.println(textRev);
  return textRev;
}
