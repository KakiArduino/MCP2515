#include <MCP2515_1.h>
#include "CANMon.h"

void setup() {
  Serial.begin(9600);
  
  mon.bitF = 125; // k Hertz
  mon.begin();
  
  if(mon.errLog != F("no error")){
    Serial.println(mon.errLog);
    while(1){
      delay(1000);
    }
  }

  // Zerar o buff, e setar 255 na posicao dlc para controle
  for (uint8_t i = 0; i < 2; i++) {
    for (uint8_t j = 0; j < 13; j++) {
      buff[i][j] = 0;
      if(j == 5){
        buff[i][j] = 255;
      }
    }
  }

  attachInterrupt(digitalPinToInterrupt(2), readCAN, FALLING);
}

void loop() {
/* Choose between wireFrame_serial () or print_Frame (separator char), 
 * commenting and uncommenting the lines below. 
 */
 
 //writeFrame_serial(); // More fast, but out puts bytes format, 
 
 printFrame(' '); //For friendly use and little busy bus, chosen it.
 // ' ' is columns separator char. ',' is default.

}
