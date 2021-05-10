#include <MCP2515_1.h>

MCP2515 mcp(4);
/*  objectName(SPI_CS, SPI_MAX_SPEED, SPI_MODE)
 *  Default values:
 *  objectName.SPI_MAX_SPEED = 10000000
 *  objectName.SPI_MODE = 0
*/

void setup() {
  //Serial.begin(1000000);

// MCP2515 initialization and configure.
  mcp.begin();
}

void loop() {
  uint16_t ID_std; // Maximum value = 0x7FF, use uint16
  uint32_t ID_ext; // Maximum value = 0x3FFFF, use uint32
  uint8_t data_len; // From 0x0 to 0x8, use unit8
  uint8_t data_field[8]; // use unit8 - list

  ID_std = random(0x7FF);// Generate a randon Standard Id
  ID_ext = random(0x3FFFF);
  
  // Generate a randon lenght of data (data_field) on the frame.
  data_len = random(0x8);

  for(uint8_t i = 0; i < data_len; i++){
    // Generate a randon data for the data_field
    data_field[i] = random(0xFF);
  }

  // Create a std. data Frame
  CANframe frm(ID_std, data_len, data_field);

  // Send a frame by TXB0
  mcp.writeFrame(frm); // defalt (frm, 0)

  // Reload 'frm' as ext. data Frame
  frm.reload(ID_std, ID_ext, data_len, data_field);

  // Send a frame by TXB1
  mcp.writeFrame(frm, 1);

  // Create a ext. data Frame
  CANframe frm1(ID_std, ID_ext, data_len, data_field);

  // Send a frame by TXB2
  mcp.writeFrame(frm1, 2);

  //Atencion after
  delay(1000);

  data_len = random(0x8);

  for(uint8_t i = 0; i < data_len; i++){
    // Generate a randon data for the data_field
    data_field[i] = random(0xFF);
  }

  // Is also can reload only whith data_len and data_field
  frm1.reload(data_len, data_field); //but it don't change the IDs
  // Send a frame by TXB0
  mcp.writeFrame(frm1);
  delay(1000);

}
