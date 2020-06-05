#include <MCP2515_1.h>


MCP2515 mon(0x00, 8, 4);


const uint8_t n = 50; //n = numero maximo de frames armazenados
uint8_t buff[n][13];

void readCAN () {
  noInterrupts();
  uint8_t bx0[13], bx1[13];
  static uint8_t bxi = 0;

  if(bxi == n){
    bxi = 0;
  }
  
  mon.readDATA(bx0, bx1);

  if (bx0[4] > 0) {

    while (bxi < n) {
      if (buff[bxi][4] == 0) {
        for (uint8_t j = 0; j < 4; j++) {
          buff[bxi][j] = bx0[j];
        }
        for (uint8_t j = 0; j < bx0[4] + 5; j++) {
          buff[bxi][j + 5] = bx0[j + 5];
        }
        buff[bxi][4] = bx0[4];
        bxi = bxi + 1;
        break;
      }
    }

  }

  if (bx1[4] > 0) {
    while (bxi < n) {
      if (buff[bxi][4] == 0) {
        for (uint8_t j = 0; j < 4; j++) {
          buff[bxi][j] = bx1[j];
        }
        for (uint8_t j = 0; j < bx1[4] + 5; j++) {
          buff[bxi][j + 5] = bx1[j + 5];
        }
        buff[bxi][4] = bx1[4];
        bxi = bxi + 1;
        break;
      }
    }
  }

  return;
}


void setup() {
  Serial.begin(9600);

  for (uint8_t i = 0; i < 2; i++) {
    for (uint8_t j = 0; j < 13; j++) {
      buff[i][j] = 0;
    }
  }

  attachInterrupt(digitalPinToInterrupt(2), readCAN, FALLING);
}



void loop() {
  interrupts();
  static uint8_t ii = 0;

  if ((buff[ii][4]) > 0 && Serial.availableForWrite() > (buff[ii][4] + 5)) {
    Serial.write(buff[ii], (buff[ii][4] + 5));
    
    buff[ii][4] = 0;
  }

  ii = ii + 1;
  if (ii == n) {
    ii = 0;
  }


}
