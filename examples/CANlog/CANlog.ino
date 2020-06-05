#include <MCP2515_1.h>
#include <SD.h>


MCP2515 mon(0x00, 8, 4);


const uint8_t n = 35; //n = numero maximos de frames armazenados
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

//SD
  if(!SD.begin(10)){
    //Serial.println(F("SD card Erro"));
    while(1);
  }
  
}



void loop() {
  interrupts();
  static uint8_t ii = 0;

  if ((buff[ii][4]) > 0) {
      SDprint(buff[ii],ii);
      buff[ii][4] = 0;
  }

  ii = ii + 1;
  if (ii == n) {
    ii = 0;
  }

}

void SDprint(uint8_t *BUFF, uint8_t ii){
      uint32_t ID = 0;
      ID = buff[ii][0];
      ID = (ID << 8) + buff[ii][1];
      ID = (ID << 8) + buff[ii][2];
      ID = (ID << 8) + buff[ii][3];

      uint32_t DATA0 = 0, DATA1 = 0;
      if(buff[ii][4] > 4){
      for(uint8_t i = 0; i < 4; i++){
        DATA0 = DATA0 + (buff[ii][5 + i] << (8*(buff[ii][4] - i -1)));        
      }
      for(uint8_t i = 4; i < buff[ii][4]; i++){
        DATA1 = DATA1 + (buff[ii][5 + i] << (8*(buff[ii][4] - i -1)));
      }
      }else{
      for(uint8_t i = 0; i < buff[ii][4]; i++){
        DATA0 = DATA0 + (buff[ii][5 + i] << (8*(buff[ii][4] - i -1)));        
      }
      }
      String str = String(String(ID)+(" (")+String(buff[ii][4])+("): ")+ String(DATA0, HEX )+String(DATA1, HEX));
      Serial.println(str);
      
      long fileSize = 0;
      do{
        static uint8_t arqn = 0;
        String arqN = String(("CANlog")+String(arqn)+(".txt"));
        
        File arq = SD.open(arqN, FILE_WRITE);

        fileSize = arq.size();
        if(fileSize < 1000 & arq){
          arq.println(str);
          arq.close();
          break;
        }
        arqn = arqn + 1;
      }while(fileSize >= 1000);
}
