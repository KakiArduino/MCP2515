
#include <MCP2515_1.h>


MCP2515 mcp(0xBB8, 8, 4);

void setup() {
   Serial.begin(9600);

}

void loop() {


uint8_t cont[4];

mcp.errorCont(cont);

Serial.print("TEC cont: ");
Serial.print(cont[0]);
Serial.print('\t');
Serial.print("REC cont: ");
Serial.print(cont[1]);
Serial.print('\t');

Serial.print(cont[2]);
Serial.print('\t');

Serial.print("TEC overflow ");
Serial.print((cont[2]&0xF));
Serial.print(" times");
Serial.print('\t');
   
Serial.print("REC overflow ");
Serial.print((cont[2] >> 4) & 0xF);
Serial.print(" times");
Serial.print('\t');

Serial.println();


Serial.println(byte(cont[3]));

delay(1000);
}
