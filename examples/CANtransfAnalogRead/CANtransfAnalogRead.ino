

#include <MCP2515_1.h>

//criando um obejto MCP2515, (ID, 2515CLK, CS)
MCP2515 mcp(3000, 8, 4);


void setup() {
  Serial.begin(9600);
  Serial.println(mcp.erroLog);
  Serial.println('\n');  
  //analogReference(INTERNAL);
}

void loop() {
 
  //Leitura de um sinal analogico
  int n = 0;
  long A = 0, t = millis();
  while(millis()- t < 10){
    A = A + analogRead(A0);  
    n = n + 1;
  }
  A = round(A/n); 
  sendData(A, sizeof(A));
  
  delay(1000);
}


void sendData(uint64_t data, uint8_t dataSize){
  //Vetor com os bytes a serem enviados, max 8
  uint8_t TX[8] ={0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00};
                  
  uint8_t i=0, n=0, crt = 0;
  while( i < dataSize){
    if(((data >>(8*(dataSize-1-i))) & 0xFF)>0 | crt ==1){
      TX[n] = (data >>(8*(dataSize-1-i))) & 0xFF;
      n = n+1;
      crt = 1;
    } 
    i = i+1;
  }
  mcp.writeDATA(n, TX);
  uint8_t n = 0;
  do{
    if(n > 10){
      Serial.println("Erro de checagem de envio");
      break;
    }
    n = n+1
  }while(mcp.regCheck(0x30, 0x04, 0x04)!=0);
  
  Serial.print(n);
  Serial.print('\t');
  for(uint8_t i = 0; i< n; i++){
    Serial.print(TX[i]);  
    Serial.print(' ');  
  }
  Serial.print('\n');

}
