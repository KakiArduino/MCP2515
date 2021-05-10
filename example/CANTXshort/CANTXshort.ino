#include <MCP2515_1.h>

MCP2515 mcp(4);

void setup() {
  mcp.begin();
}

void loop() {
  
  UNION_t Data;
  
  Data.INT = analogRead(A0);
  
  CANframe frm(25, sizeof(Data.INT), Data.bytes);
  
  mcp.writeFrame(frm);
  
  delay(1000);
}
