
#include <RFM69.h>
#include <SPI.h>

#define BASEID        99
#define NETWORKID     100

#define FREQUENCY     RF69_433MHZ
#define ENCRYPTKEY    "sampleEncryptKey"
#define SERIAL_BAUD   115200
#define ACK_TIME      30

RFM69 radio;
byte readSerialLine(char* input, char endOfLineChar=10, byte maxLength=64, uint16_t timeout=50);

byte inputLen=0;
char input[64];
char *payload;
int nodeId = 128;
  
void setup(){
  Serial.begin(115200);
  radio.initialize(FREQUENCY, BASEID, NETWORKID);
  radio.encrypt(ENCRYPTKEY);
  Serial.println("Ready");
}

void loop(){
  if (Serial.available() > 0){
    inputLen = readSerialLine(input);
  
    char *p;
    p = strtok(input, " ");
    if (p){
      nodeId = atoi(p);
    }
    p = strtok(NULL, " ");
    if(p){
      payload = p;
    }
    if (radio.sendWithRetry(nodeId, payload, strlen(payload), 0)){
    }
  }
  
  if (radio.receiveDone()){
    for (byte i = 0; i < radio.DATALEN; i++){
      Serial.print((char)radio.DATA[i]);
    }
    if (radio.ACKRequested()){
      radio.sendACK();
    }
    Serial.println();
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
