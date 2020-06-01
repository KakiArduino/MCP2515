#include <MCP2515_1.h>


MCP2515 mon(0x00, 8, 4);


// 4 bytes de ID + 4 byte controle + 32 bytes de dado
static uint8_t i = 0;

const uint8_t n = 10; //n = numero de dispositivos
uint8_t buff[n][61];

void readCAN (){ 
  noInterrupts();           
  uint8_t bx0[13], bx1[13];
  
  mon.readDATA(bx0, bx1);

  if(bx0[4] > 0){
    for(uint8_t i = 0; i < n; i++){
      
      if (bx0[0] == buff[i][0] && bx0[1] == buff[i][1] && 
          bx0[2] == buff[i][2] && bx0[3] == buff[i][3]){
        if((buff[i][4] + bx0[4]) > 58){
          break;
        }
        for(uint8_t j=0; j < bx0[4] + 5; j++){
          buff[i][j+5+buff[i][4]] = bx0[j+5];
        }
        buff[i][4] = buff[i][4] + bx0[4]; 
        break;
      
      }else{
        if (buff[i][4] == 0){
          for(uint8_t j = 0; j < 4; j++){
            buff[i][j] = bx0[j];
          }   
          for(uint8_t j =0; j < bx0[4] + 5; j++){
            buff[i][j+5] = bx0[j+5];
          }   
          buff[i][4] = bx0[4];
          break;
        }      
     }
     
    }
  }


  if(bx1[4] > 0){
    for(uint8_t i = 0; i < n; i++){
      if (bx1[0] == buff[i][0] && bx1[1] == buff[i][1] && bx1[2] == buff[i][2] && bx1[3] == buff[i][3]){
        if((buff[i][4] + bx1[4]) > 58){
          break;
        }
        for(uint8_t j =0; j < bx0[4] + 5; j++){
          buff[i][j+5+buff[i][4]] = bx1[j+5];
        }     
        buff[i][4] = buff[i][4] + bx1[4]; 
        break;

      }else{
        if (buff[i][4] == 0){
          for(uint8_t j =0; j < 4; j++){
            buff[i][j] = bx1[j];
          }   
          for(uint8_t j =0; j < bx1[4] + 5; j++){
            buff[i][j+5] = bx1[j+5];
          }   
          buff[i][4] = bx1[4];
          break;
    
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
  
  attachInterrupt(digitalPinToInterrupt(2), readCAN, FALLING);  
}



void loop(){
  interrupts();

  if((buff[i][4]) > 0 && Serial.availableForWrite() > (buff[i][4]+5)){
    Serial.write(buff[i], (buff[i][4]+5));
    buff[i][4] = 0;  
  }
  
  i = i+1;
  if(i == n){
    i = 0;
  }
}
