#define DECODE_NEC // We want to decode NEC protocol

#define IR_RECEIVE_PIN 2
#define IR_SEND_PIN 3

#include <IRremote.hpp>

#define DEVICE_ID 202 // ID of charger device 

#define COMMAND_CHARGE_STATION 0xF2 // Command telling the robot it is at a charger station

#define COMMAND_CHARGE 0xC1 // Command telling charge station to charge the robot
#define COMMAND_CHARGE_COMPETE 0xC2 // Command for telling the charge station the charge is completed

#define SEND_INTERVAL 100

bool awaitingAnswer = false;
unsigned long lastTransmission = 0;

int amountCharged = 0;
int IDList[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 53}; // List of ID of all robots in the system used as reference for accounts
unsigned long chargeList[10] = { 0 }; // List of total charge for each robot (aka the account of each robot)
int lastCarID;

uint8_t getIDIndex(uint8_t ID) { // Get index of in array of IDs of correct robot
  for (int i = 0; i < 10; i++) {
    if (ID == IDList[i]) {
      //Serial.println("ID found");
      return i;
    }
  }
  //Serial.println("ID not found");
  return 255;  // ID not found return 255
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(9600);
  delay(3000);

  IrReceiver.begin(IR_RECEIVE_PIN);
  IrSender.begin(DISABLE_LED_FEEDBACK);
}

void loop() {
  unsigned long timeNow = millis();

  if(timeNow - millis() > SEND_INTERVAL){ // Sends IR-signal for robot to know it's at the charge station every SEND_INTERVAL ms
    IrSender.sendNEC(DEVICE_ID, COMMAND_CHARGE_STATION, 0);
    lastTransmission = timeNow;
    awaitingAnswer = true;
  }
 
  if(IrReceiver.decode()){ // If signal is received

    if(IrReceiver.decodedIRData.protocol == UNKNOWN){
      Serial.println(F("Received noise or and unknown (or not yet enabled protocol)"));
      IrReceiver.printIRResultRawFormatted(&Serial, true);
    }
    Serial.println();
    IrReceiver.resume(); // Open IR-reciever for new commands

    if (awaitingAnswer && IrReceiver.decodedIRData.address != DEVICE_ID) { // If we are waiting for an answer and the answer is not from ourself
      Serial.println("Got answer");
      Serial.print("The car ID is:");
      Serial.println(IrReceiver.decodedIRData.address);

      Serial.print("Recived command is:");
      Serial.println(IrReceiver.decodedIRData.command);

      if (IrReceiver.decodedIRData.command == COMMAND_CHARGE) { // If the command is a request to charge
        lastCarID = IrReceiver.decodedIRData.address;
        // Serial.print(".");
        amountCharged += 1;
      }

      if (IrReceiver.decodedIRData.command == COMMAND_CHARGE_COMPETE){ // If the command say that the charge is completed
        Serial.println(" Charge Complete");
        
        chargeList[getIDIndex(lastCarID)] += amountCharged; // Save charged amount to account array
        Serial.print("This charge: "); Serial.print(amountCharged); 
        Serial.print(". Total charge: "); Serial.println(chargeList[getIDIndex(lastCarID)]);
        
        amountCharged = 0; // Clear variables for next charge
      }

      awaitingAnswer = false;
    }
  }
}
