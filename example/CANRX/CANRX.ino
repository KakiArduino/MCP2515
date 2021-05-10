#include <MCP2515_1.h>

MCP2515 mcp(4);
/*  objectName(SPI_CS, SPI_MAX_SPEED, SPI_MODE)
 *  Default values:
 *  objectName.SPI_MAX_SPEED = 10000000
 *  objectName.SPI_MODE = 0
*/

const char separator = ' ';

void setup() {
  Serial.begin(9600);

// MCP2515 initialization and configure.
  mcp.begin();


}

void loop() {
  mcp.readFrame();

  if (mcp.frameRXB0.type != F("No frame")) {
    Serial.print(mcp.frameRXB0.id_std);
    Serial.print(separator);
    Serial.print(mcp.frameRXB0.id_ext);
    Serial.print(separator);
    Serial.print(mcp.frameRXB0.dlc);
    Serial.print(separator);

    for(uint8_t i = 0; i < mcp.frameRXB0.dlc; i++){
      Serial.print(mcp.frameRXB0.data[i]);
      Serial.print(separator);
    }
    Serial.print('\n');
  }

    if (mcp.frameRXB1.type != F("No frame")) {
      Serial.print(mcp.frameRXB1.id_std);
      Serial.print(separator);
      Serial.print(mcp.frameRXB1.id_ext);
      Serial.print(separator);
      Serial.print(mcp.frameRXB1.dlc);
      Serial.print(separator);

      for(uint8_t i = 0; i < mcp.frameRXB1.dlc; i++){
        Serial.print(mcp.frameRXB1.data[i]);
        Serial.print(separator);
      }
      Serial.print('\n');
    }

}
