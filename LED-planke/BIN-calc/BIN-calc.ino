
#include <Adafruit_NeoPixel.h>

const int LED_PIN = 6;    // Set pin for connected to first Digital IN
const int LED_COUNT = 96; // Set amount of LEDS connected, for us it's 96 (3 strips)

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // LED strip object

String message;
enum Calculation { ERROR, ADDITION, SUBTRACTION }; // Enum to define calculation type
Calculation calc = ERROR;

// Declare all functions
void updateInput();
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
  updateInput(); // Update Inputs, needed when we got more than one input type
}

void updateInput(){
  if (Serial.available() > 0) { // If we got an input from serial
    // read the incoming message:
    message = Serial.readString();  // Read message
    message.replace(" ", "");       // Remove spaces from message
    //Serial.println(message); 

    String temp;        // String to temporary store numbers
    uint32_t number1;   // Number 1, forced to accept 32 bit integers
    uint32_t number2;   // Number 2, forced to accept 32 bit integers

    bool first = true;  // Flag to seperate numbers

    for(int x = 0; x < message.length(); x++){ // Analyze message
      if(isDigit(message[x])){  // If message is a digit
        temp += message[x];     // Add digit to total number
        //Serial.println(temp);
      }
      if(!isDigit(message[x])){   // If message is not a digit
        if(first){                // If we are still on the first number
          number1 = temp.toInt(); // Save first number
          temp = "";              // Clear placeholder
          first = false;          // Flag that we got the first number
        } else {                  // If we're not on the first number
          number2 = temp.toInt(); // Save second number
        }
      }
    }
    
    //Serial.println(number1);
    //Serial.println(number2);

    if(message.indexOf('+') != -1){         // Check if requested operation is addition
      calc = ADDITION;
    } else if(message.indexOf('-') != -1){  // If it is not, check if it is subtraction
      calc = SUBTRACTION;
    } else {                                // If it is not, then it is illegal
      calc = ERROR;
    }
    //Serial.println(calc);
    
    if(calc == ADDITION){                  // Initiate addition if requested
      addition(number1, number2);
    } else if(calc == SUBTRACTION){        // Initiate subtraction if requested
      subtraction(number1, number2);
    } else {
      Serial.println("ERROR: Input not accepted"); // Otherwise cancel operation
    }
  }
}

void addition(uint32_t x, uint32_t y){
  strip.clear();

  uint32_t z = x + y; // Perform the calculation
  printToLED(x, 1); // Print to first plank
  printToLED(y, 2); // Print to second plank
  printToLED(z, 3); // Print to third plank
  strip.show(); 

  //Serial.print(x); Serial.print(" + "); Serial.print(y); Serial.print(" = "); Serial.println(z);
}

void subtraction(uint32_t x, uint32_t y){ 
  strip.clear();

  uint32_t z = x - y; // Perform the calculation
  printToLED(x, 1);   // Print to first plank
  printToLED(y, 2);   // Print to second plank
  printToLED(z, 3);   // Print to third plank
  strip.show(); 

  //Serial.print(x); Serial.print(" - "); Serial.print(y); Serial.print(" = "); Serial.println(z);
}

void printToLED(uint32_t number, int plank){ // Print given number to set plank
  //Serial.print("Number: "); Serial.println(number);
  String n = String(number, BIN); // Convert number to binary form
  n = reverseString(n);           // Reverse numbers binary form (This is because LSB must be set as index 0)
  int start = 32 * (plank - 1);   // Designate starting position
  
  for(int x = 0; x <= n.length(); x++){ // Check every digit in the number
    //Serial.print(n[x]);
    if(n[x] == '1'){ // If digit is 1 make it white
      strip.setPixelColor((x + start), strip.Color(127, 127, 127));
    } else if(n[x] == '0'){ // If digit is 0 make it red
      strip.setPixelColor((x + start), strip.Color(127, 0, 0));
    }
  }
  //Serial.println("");
} 

String reverseString(String text){ // Reverse string
  //Serial.print("Before: ");Serial.println(text);
  String textRev; // Placeholder string
  for(int x = 0; x < text.length(); x++){ // For entire message
    textRev += text[text.length()-1-x];   // Set first char in placeholder to be last char in original
  };
  //Serial.print("After:  "); Serial.println(textRev);
  return textRev; // Return the reversed string
}
