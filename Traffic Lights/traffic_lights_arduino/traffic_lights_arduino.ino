#define IR_SEND_PIN 3 // Arduino pin for sending IR, must be defined before including IRremote.h

#include <IRremote.hpp>

// Pins for the traffic light
#define RED_LED_PIN 12 
#define YELLOW_LED_PIN 11
#define GREEN_LED_PIN 10

#define DEVICE_ADDRESS 0x00 // Address of this device

#define RED_LIGHT_COMMAND 0x34      // Command for red light
#define YELLOW_LIGHT_COMMAND 0x35   // Command for yellow light
#define GREEN_LIGHT_COMMAND 0x36    // Command for green light

#define SEND_INTERVAL 100 // Update interval in ms
#define LIGHT_CHANGE_INTERVAL 10000 // State change interval in ms

enum lightState {RED, YELLOW, GREEN};
lightState state = RED;     // Current state of the traffic light
bool towardsGreen = true;   // Going towards green or red light

// Timing variables
unsigned long lastUpdate = 0;
unsigned long lastStateChange = 0;

void setup() {
    // Setup pins
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(YELLOW_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);

    // Setup serial communication for debug messages
    Serial.begin(9600);

    // Print debug info
    DEBUG_SERIAL(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));
    DEBUG_SERIAL_S(F("Send IR signals at pin "));
    DEBUG_SERIAL(IR_SEND_PIN);

    // Start with IR_SEND_PIN as send pin and disable feedback LED
    IrSender.begin(DISABLE_LED_FEEDBACK); 
}

void loop() {
    unsigned long timeNow = millis();

    if (timeNow - lastUpdate > SEND_INTERVAL) { // Send IR signal every SEND_INTERVAL
        lastUpdate = timeNow;
        updateState();
    }

    if (timeNow - lastStateChange > LIGHT_CHANGE_INTERVAL) { // Change state every LIGHT_CHANGE_INTERVAL
        lastStateChange = timeNow;
        switch (state) {
            case RED:
                state = YELLOW;
                towardsGreen = true;
                break;
            case YELLOW:
                if (towardsGreen) {
                    state = GREEN;
                } else {
                    state = RED;
                }
                break;
            case GREEN:
                state = YELLOW;
                towardsGreen = false;
                break;
        }
    }
    
}

// Update the state of the traffic light and send IR signal
void updateState(){
    switch (state) {
        case RED:
            digitalWrite(RED_LED_PIN, HIGH);
            digitalWrite(YELLOW_LED_PIN, LOW);
            digitalWrite(GREEN_LED_PIN, LOW);
            IrSender.sendNEC(DEVICE_ADDRESS, RED_LIGHT_COMMAND, 0);
            break;
        case YELLOW:
            digitalWrite(RED_LED_PIN, LOW);
            digitalWrite(YELLOW_LED_PIN, HIGH);
            digitalWrite(GREEN_LED_PIN, LOW);
            IrSender.sendNEC(DEVICE_ADDRESS, YELLOW_LIGHT_COMMAND, 0);
            break;
        case GREEN:
            digitalWrite(RED_LED_PIN, LOW);
            digitalWrite(YELLOW_LED_PIN, LOW);
            digitalWrite(GREEN_LED_PIN, HIGH);
            IrSender.sendNEC(DEVICE_ADDRESS, GREEN_LIGHT_COMMAND, 0);
            break;
    }
}