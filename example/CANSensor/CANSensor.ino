/// CANSensor
#include <MCP2515_1.h>
#include "CANSensor.h"

void setup() {
  
  mcp.CANINTE = 0x04;
  mcp.begin();

  attachInterrupt(digitalPinToInterrupt(2), sendCAN, FALLING);
}

void loop() {

  // ID declaration
  uint16_t Sid = 0x7F0; //standart
  uint32_t Eid = 0; // extend
  Sid = random(0x7FF); //Set std. ID, maximun is 0x7FF
  Eid = random(0x3FFFF); //Set ext. ID, maximun is 0x3FFFF
  
  // Example of data acquisition and send -----------------------------
  // Use union type to convert variable formats
  UNION_t Data;

  // Int data example
  Data.INT = analogRead(A0);

  //Send a extended frame
  putinBuff(Sid, Eid, sizeof(Data.INT), Data.bytes);

  // Float data example
  Data.FLT = analogRead(A0)*(5.0/1023.0); // convert in Volts

  // Send a standard frame
  putinBuff(Sid, sizeof(Data.FLT), Data.bytes);
  
  delay(1000);
}
