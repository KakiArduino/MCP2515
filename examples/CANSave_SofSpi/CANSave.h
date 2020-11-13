void readCAN () {
  noInterrupts();
  uint8_t bx0[13], bx1[13];
  static uint8_t bxi = 0;

  if(bxi == n){
    bxi = 0;
  }
  
  mcp.readDATA(bx0, bx1);

  if (bx0[4] > 0) {

    while (bxi < n) {
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

  if (bx1[4] > 0) {
    while (bxi < n) {
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

  return;
}

uint8_t putPush(uint8_t buts){
  
  if(buts == 1){
    buts = 0;
    
    Serial.println(F(" Verificando cartão!\n\n"));
    
    delay(250);
    digitalWrite(SD_CHIP_SELECT_PIN, LOW);
    delayMicroseconds(10000);
    while(!sd.begin(SD_CHIP_SELECT_PIN, SD_SCK_MHZ(50))) {
      digitalWrite(3, HIGH);
      delay(80);
      digitalWrite(3, LOW);
      delay(80);
    }
    delay(50);
    
    Serial.println(F(" Foi!\n\n"));
    
    file.open(fileName, O_APPEND | O_RDWR | O_CREAT);
    digitalWrite(3, LOW);
    return buts;
  }else{ 
    buts = 1;
    noInterrupts();
    file.close();
    delayMicroseconds(100000);
    digitalWrite(SD_CHIP_SELECT_PIN, HIGH);
    
    Serial.println(F("\n\n - Já pode tirar o cartão!\n\n Após insirir o cartão aperte o botão!\n"));
    
    digitalWrite(3, HIGH);
    interrupts();
    delay(300);
    return buts;
  }
  
}

void filesC(){ 
  digitalWrite(3, HIGH);
  while(sd.exists(fileName) and file.fileSize() > 10){
    fileName[16] = fileName[16] + 1;
    if(fileName[16] > 0x39){
      fileName[16] = 0x30;
      fileName[15] = fileName[15] + 1;  
      if(fileName[15] > 0x39){
        fileName[15] = 0x30;  
        fileName[14] = fileName[14] + 1;
        if(fileName[14] > 0x39){
          fileName[14] = 0x30;
        }      
      }   
    }
  }
    if(!file.open(fileName, O_APPEND | O_RDWR | O_CREAT)){
      while(1){
      digitalWrite(3, HIGH);
      delay(80);
      digitalWrite(3, LOW);
      delay(80);
      }
    }
    digitalWrite(3, LOW);
    file.print(F("Arquivo log Mini CAN\nIDms, IDls, extms, ext, extls, dlc, d1, d2, d3, d4, d5, d6, d7, d8\n"));
    return;

}
  
