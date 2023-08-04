#include <Adafruit_NeoPixel.h>

const int LED_PIN = 6;    // Set pin for connected to first Digital IN
const int LED_COUNT = 32; // Set amount of LEDS connected, for us it's 96 (3 strips)

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800); // LED strip object

const int BUTTON_PIN = 4;         // Button pin
unsigned long button_delay = 0;   // Button millis flag
int number = 0;                   // Store number
uint32_t bit_limit = pow(2,8)-1;  // Highest BIN number you can show ((2^x)-1), in code "pow(2,x)-1";, where x is a number between 0 and 32
                                  // Replace this as you see fit, but beware that 2^32-1 is the upper limit of the variable
// Declare functions
void updateInput();
void updateNumber();
int BinaryToGray(int number);
void printToLED(int number);
String reverseString(String text);

void setup() {
  Serial.begin(9600);

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  pinMode(BUTTON_PIN, INPUT);
}

void loop() {
  updateInput(); // Update button input
}

void updateInput(){
  if(millis() - button_delay > 250){  // If button has not been pressed within the last 1/4 of a second
    if(digitalRead(BUTTON_PIN)){      // If button is pressed
      button_delay = millis();

      updateNumber();                 // Go to next number
    }
  }
}

void updateNumber(){
  if(number < bit_limit){ // Increase number if within limit
    number++;
  } else if (number == bit_limit){  
    number = 0;
  }
  //Serial.println(number);

  strip.clear();
  printToLED(number);
  strip.show();
}

int BinaryToGray(int number) // Convert number to GRAY code equivalent
{
    return number ^ (number >> 1); // The operator >> is shift right. The operator ^ is exclusive or.
}

void printToLED(int number){ // Print given number to set plank
  Serial.print("Number: "); Serial.println(number);
  
  number = BinaryToGray(number);  // Comment out this if you want to see binary numbers not gray-code numbers

  String n = String(number, BIN); // Convert number to binary form
  n = reverseString(n);           // Reverse numbers binary form (This is because LSB must be set as index 0)
  
  for(int x = 0; x <= n.length()-1; x++){ // Check every digit in the number
    //Serial.print(n[x]);
    if(n[x] == '1'){ // If digit is 1 make it white
      strip.setPixelColor(x, strip.Color(127, 127, 127));
    } else if(n[x] == '0'){ // If digit is 0 make it red
      strip.setPixelColor(x, strip.Color(127, 0, 0));
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
