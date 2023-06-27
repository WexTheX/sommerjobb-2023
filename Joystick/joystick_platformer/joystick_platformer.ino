
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// JOYSTICK PINS
#define X_PIN A0
#define Y_PIN A1
#define PRESS_PIN 8

// OLED DEFINITIONS
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
// PINS arduino UNO: A4(SDA), A5(SCL)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Delay flags
unsigned long prevMil = 0;

// Joystick variables
int xValue;
int yValue;
int press;
int direction;

// Joystick enums 
enum stickState { TOP, MID, BOT }; // If joystick potensiometer gives high or low value (HIGH != UP, LOW != DOWN);
stickState xState = MID;
stickState yState = MID;

// Direction of joystick as seen from when text on joystick is the correct way
enum directionState { NONE,
                 UP,
                 R_UP,
                 RIGHT,
                 R_DOWN,
                 DOWN,
                 L_DOWN,
                 LEFT,
                 L_UP };
directionState dir = NONE; 
// Array of direction names to make it easier to write direction when needed (enum only gives index value)
char *directionName[] = { "NONE",
                          "UP",
                          "RIGHT and UP",
                          "RIGHT",
                          "RIGHT and DOWN",
                          "DOWN",
                          "LEFT and DOWN",
                          "LEFT",
                          "LEFT and UP" };

// Game variables
int playerXPos = 35;
int playerYPos = 26;
bool rising = false;
unsigned long airtime = 0;
int enemyPos[] = {10};
unsigned long hitDelay = 0;

enum scrollDirection { STAND, MOVE_RIGHT, MOVE_LEFT };
scrollDirection scrollDir = STAND;
char *scrollDirectionName[] = { "STAND", "MOVE RIGHT", "MOVE LEFT "};

// Function declarations so the code knows these functions exist somewhere in the code
// Joystick declarations
void updateJoystick();
void updateXAxis();
void updateYAxis();
void updatePress();
void updateDirection();
// OLED declarations
void updateOLED();
void drawTerrain();
void drawCharacter();
void drawDefaultEnemy();
// Game Mechanic Functions
void updateGame();
void movePlayer();
void moveDefaultEnemy();
void hitDetection();

void setup() {
  Serial.begin(9600);
  pinMode(PRESS_PIN, INPUT_PULLUP);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  delay(500); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
}

void loop() {
  updateJoystick();
  

  if (millis() - prevMil > 25) {
    prevMil = millis();
    updateOLED();
    updateGame();

    //Serial.print("x direction: "); Serial.print(xState);
    //Serial.print(". y direction: "); Serial.print(yState);
    //Serial.print(". press value: "); Serial.println(press);
    //Serial.println(directionName[dir]); 
    //Serial.println(scrollDirectionName[scrollDir]);
    //Serial.print(playerXPos); Serial.print(". "); Serial.println(playerYPos);
    //Serial.println(enemyPos[0]);
    //Serial.println("");
  }
}

// Joystick-functions
void updateJoystick() { // Function that combines all joystick-updating functions
  updateXAxis();
  updateYAxis();
  updatePress();
  updateDirection();
}

void updateXAxis() { // Updates x-axis of joystick
  xValue = analogRead(X_PIN);
  if (xValue > 800) {
    xState = TOP;
  } else if (xValue < 200) {
    xState = BOT;
  } else {
    xState = MID;
  }
}

void updateYAxis() {// Updates y-axis of joystick
  yValue = analogRead(Y_PIN);
  if (yValue > 800) {
    yState = TOP;
  } else if (yValue < 200) {
    yState = BOT;
  } else {
    yState = MID;
  }
}

void updatePress() { // Updates where or not button is pressed
  press = digitalRead(PRESS_PIN);
}

void updateDirection() { // Uses axis updates to find the actual direction of the joystick
  if (xState == TOP) {
    if (yState == TOP) {
      dir = R_DOWN;
    } else if (yState == BOT) {
      dir = R_UP;
    } else {
      dir = RIGHT;
    }
  } else if (xState == BOT) {
    if (yState == TOP) {
      dir = L_DOWN;
    } else if (yState == BOT) {
      dir = L_UP;
    } else {
      dir = LEFT;
    }
  } else {
    if (yState == TOP) {
      dir = DOWN;
    } else if (yState == BOT) {
      dir = UP;
    } else {
      dir = NONE;
    }
  }
}

// OLED-functions
void updateOLED(){
  display.clearDisplay();
  drawTerrain();
  drawPlayer();
  drawDefaultEnemy();
  display.display();
}

void drawTerrain(){
  display.drawLine(0, 31, display.width()-1, 31, SSD1306_WHITE);

  for(int i = 0; i < 256; i += 16){
    display.drawLine(i - playerXPos, 16, i - playerXPos, 31, SSD1306_WHITE);
  }
}

void drawPlayer(){
  display.drawRect(35, playerYPos, 7, 5, SSD1306_WHITE);
}

void drawDefaultEnemy(){
  for(int i = 0; i < sizeof(enemyPos); i++){
    //display.drawRect(enemyPos[i], 25, 6,6, SSD1306_WHITE);
    display.drawTriangle(enemyPos[i], 25, enemyPos[i]-3, 25+6, enemyPos[i]+3, 25+6, SSD1306_WHITE);
  }
}
// Game Mechanic Fuctions
void updateGame(){
  movePlayer();
  moveDefaultEnemy();
  hitDetection();
};

void movePlayer(){
  if(yState == BOT){
    scrollDir = MOVE_RIGHT;
  } else if (yState == TOP){
    scrollDir = MOVE_LEFT;
  } else {
    scrollDir = STAND;
  }

  switch(scrollDir){
    case STAND:
      break;
    case MOVE_RIGHT:
      playerXPos -= 1;
      if (playerXPos < -1){
        playerXPos = 127;
      }
      break;
    case MOVE_LEFT:
      playerXPos += 1;
      if(playerXPos > 127){
        playerXPos = 0;
      }
      break;
  }

  if ((playerYPos == 26) && (xState == TOP)){
    rising = true;
    airtime = millis();
  }
  if ((rising == true) && (millis()-airtime < 500)){
    playerYPos -= 1;
  } else {
    rising = false;
    if(playerYPos < 26){
      playerYPos += 1;
    }
  } 
}

void moveDefaultEnemy(){
  for(int i = 0; i < sizeof(enemyPos); i++){
    int movement = random(0, 3);
    enemyPos[i] -= movement;

    if(scrollDir == MOVE_RIGHT){
      enemyPos[i] += 1;
    } else if (scrollDir == MOVE_LEFT){
      enemyPos[i] -= 1;
    }

    if (enemyPos[i] < -1){
      enemyPos[i] = 130;
    }
  }
}

void hitDetection(){
  if(millis() - hitDelay > 500){
    for(int i = 0; i < sizeof(enemyPos); i++){
    if((((playerXPos - enemyPos[i]) < 5) && ((playerXPos - enemyPos[i]) > 0)) && (playerYPos+5 > 25)){
      Serial.println("Hit");
      hitDelay = millis();
      }
    }
  }
}