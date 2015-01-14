#include <RFM69.h>
#include <SPI.h>

#define NODEID        1
#define BASEID        128
#define NETWORKID     100

#define FREQUENCY     RF69_433MHZ
#define ENCRYPTKEY    "EncryptKey"
#define SERIAL_BAUD   115200
#define ACK_TIME      30

RFM69 radio;
int currentVal = 1;
char *p, *cmd, *data;

void setup(){
  radio.initialize(FREQUENCY, NODEID, NETWORKID);
  radio.encrypt(ENCRYPTKEY);

  Serial.begin(SERIAL_BAUD);
  Serial.println("Ready");

  pinMode(2, INPUT_PULLUP);
}

void loop(){
  /* Check for any data coming in over the wire and if there is parse it
     to check for a valid command. */
     
  if (radio.receiveDone() && radio.DATALEN > 0){
    if (parseCommand((char *)radio.DATA)){
      Serial.println(cmd);
      Serial.println(data);
    }else{
      Serial.println("Not a valid command");
    }
  }

  /* Example of how to send data back to the base when an input changes */
  int sensorVal = digitalRead(2);
  if (sensorVal != currentVal){
    currentVal = sensorVal;
    Serial.print("Changed: ");
    Serial.println(currentVal);
    
    char payload[9];
    sprintf(payload, "SENSOR:%d", currentVal);
    
    if (radio.sendWithRetry(BASEID, payload, strlen(payload), 0)){
      Serial.println("Sent to base");
    }
  }
}

int parseCommand(char *radioData){
  /* Parse the data (char array) that comes in off the wire.
     It should be in the format command:data */

  p = strtok(radioData, ":");
  cmd = p;
  p = strtok(NULL, ":");
  data  = p;

  if (cmd != NULL && data != NULL){
     return 1;
  }
  return 0;
}
