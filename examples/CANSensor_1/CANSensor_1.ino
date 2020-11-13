/// CANSensor
#include <MCP2515_1.h>
const uint8_t n = 10;
uint8_t buff[n][14], nF = 0, nF25 = 0;
MCP2515 mcp(0x00, 8, 4);
#include "CANSensor.h"


void setup() {
  Serial.begin(9600);
  mcp.confINT(0x04, 0x0F, 0x00);
  
  attachInterrupt(digitalPinToInterrupt(2), sendCAN, FALLING);
  
}

void loop() {
  FLOATUNION_t leitura;
  
  leitura.number = analogRead(A0);

  uint8_t data[sizeof(leitura.number)];
  
  putinBuff(0x0F, 0x0, sizeof(leitura.number), leitura.bytes);

  
  for(uint8_t jw = 0; jw < sizeof(leitura.number); jw++){
    Serial.print(leitura.bytes[jw], HEX);
    Serial.print(' ');
  }
  Serial.println();
  
  
  delay(1000);
}
