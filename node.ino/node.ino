#include <RFM69.h>
#include <SPI.h>

#define NODEID        1
#define BASEID        99
#define NETWORKID     100

#define FREQUENCY     RF69_433MHZ
#define ENCRYPTKEY    "sampleEncryptKey"
#define SERIAL_BAUD   115200
#define ACK_TIME      30

RFM69 radio;
byte readSerialLine(char* input, char endOfLineChar=10, byte maxLength=64, uint16_t timeout=50);

int currentVal = 1;

void setup(){
  Serial.begin(SERIAL_BAUD);
  radio.initialize(FREQUENCY,NODEID,NETWORKID);
  radio.encrypt(ENCRYPTKEY);
  Serial.println("Ready");
  
  pinMode(2, INPUT_PULLUP);
}

void loop(){
  if (radio.receiveDone()){
    if (radio.DATALEN > 0){
      char *p;
      char *cmd;
      char *data;
      
      p = strtok((char *)radio.DATA, ":");
      if (p){
        cmd = p;
      }
      p = strtok(NULL, ":");
      if(p){
        data  = p;
      }
      Serial.println(cmd);
      Serial.println(data);
    }
  }
  int sensorVal = digitalRead(2);
  if (sensorVal != currentVal){
    currentVal = sensorVal;
    Serial.println("Changed");
    Serial.println(currentVal);
    if (radio.sendWithRetry(BASEID, "TEMP1:76.3", 10, 0)){
      Serial.println("Sent to base");
    }
  }
}
