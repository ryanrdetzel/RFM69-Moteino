#include <RFM69.h>
#include <SPI.h>

#define BASEID        128
#define NETWORKID     100

#define FREQUENCY     RF69_433MHZ
#define ENCRYPTKEY    "EncryptKey"
#define SERIAL_BAUD   115200
#define ACK_TIME      30

byte readSerialLine(char* input, char endOfLineChar=10, byte maxLength=64, uint16_t timeout=50);

RFM69 radio;
byte inputLen;
byte nodeId;
char input[64];
char *payload;
char *p;

void setup(){
  radio.initialize(FREQUENCY, BASEID, NETWORKID);
  radio.encrypt(ENCRYPTKEY);
  
  Serial.begin(115200);
}

void loop(){
  if (Serial.available() > 0){
    /* If data came in from the PI parse out the target node and then send the 
       rest down the wire */
       
    inputLen = readSerialLine(input);
    p = strtok(input, " ");
    nodeId = atoi(p); // Conver it to an int

    payload = strtok(NULL, " ");
    
    if (nodeId != NULL && payload != NULL){  
      if (nodeId >= 0 && nodeId < 128){
        if (radio.sendWithRetry(nodeId, payload, strlen(payload), 0)){
          /* Message sent to the wire */
        }
      }
    }
  }

  if (radio.receiveDone()){
    for (byte i = 0; i < radio.DATALEN; i++){
      Serial.print((char)radio.DATA[i]);
    }
    if (radio.ACKRequested()){
      radio.sendACK();
    }
    Serial.println(); // Do we need this?
  }
}

byte readSerialLine(char* input, char endOfLineChar, byte maxLength, uint16_t timeout){
  byte inputLen = 0;
  Serial.setTimeout(timeout);
  inputLen = Serial.readBytesUntil(endOfLineChar, input, maxLength);
  input[inputLen] = 0; //null-terminate it
  Serial.setTimeout(0);
  return inputLen;
}
