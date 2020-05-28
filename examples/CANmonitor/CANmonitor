#include <MCP2515_1.h>


MCP2515 mon(0x00, 8, 4);

// 4 bytes de ID + 4 byte controle + 32 bytes de dado
uint8_t buff[2][40]; //n = numero de dispositivos
static uint8_t i = 0;

void readCAN (){ //n = numero de dispositivos, 
  uint8_t n=2, N = 100;
                      
                                           //N = numero de leitura maxima
  uint8_t bx0[13], bx1[13];                // ate que ler um dado valido

  uint8_t t = 0;
    
    mon.readDATA(bx0, bx1);

    if(bx0[4] > 0){

      for(uint8_t i = 0; i < n; i++){
        if ((buff[i][0] == bx0[0] && buff[i][1] == bx0[1] && 
             buff[i][2] == bx0[2] && buff[i][3] == bx0[3] )|
             (buff[i][0] == 0 && buff[i][1] == 0 && 
             buff[i][2] == 0 && buff[i][3] == 0) ){
            
            if((buff[i][4] + bx0[4]) > 32){
              return;
            }
            Serial.println("OI");
            for(uint8_t j =0; j < 4; j++){
              buff[i][j] = bx0[j];
            }
            buff[i][4] = buff[i][4] + bx0[4];
            for(uint8_t j =0; j < bx0[4]; j++){
              buff[i][j+5+buff[i][4]] = bx0[j];
            } 
        }
      }
    }

    if(bx1[4] > 0){
      for(uint8_t i = 0; i < n; i++){
        if (buff[i][0] == bx1[0] && buff[i][1] == bx1[1] && 
            buff[i][2] == bx1[2] && buff[i][3] == bx1[3] |
            buff[i][0] == 0 && buff[i][1] == 0 && 
            buff[i][2] == 0 && buff[i][3] == 0 ){
            if((buff[i][4] + bx1[4]) > 32){
              return;
            }
            for(uint8_t j =0; j < 4; j++){
              buff[i][j] = bx1[j];
            }
            buff[i][4] = buff[i][4] + bx1[4];
            for(uint8_t j =0; j < bx1[4]; j++){
              buff[i][j+5+buff[i][4]] = bx1[j];
            } 
        }
      }
    }

  return;
  
}


void setup(){
  Serial.begin(9600);

  for(uint8_t i = 0; i<2; i++){
    for(uint8_t j = 0; j<40; j++){
      buff[i][j] = 0;  
    }
  }
    
  
  attachInterrupt(digitalPinToInterrupt(2), readCAN, CHANGE);  
}



void loop(){
  
  if (i == 2){
    i = 0;
  }
  
  if(buff[i][4] > 0){
    for(uint8_t j = 0; j < buff[i][4] + 5; j++){
      Serial.print(buff[i][j], HEX);
      Serial.print(' ');  
    }
    Serial.println();
    for(uint8_t j = 0; j < 40; j++){
      buff[i][j] = 0;  
    }
  }

  i = i + 1;

  
}
