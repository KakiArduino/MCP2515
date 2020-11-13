
void sendCAN(){
  static uint8_t i = 0;//, INT = 0;
  uint8_t ctr[1] = {0}, tx = 0, TX[8];
  uint32_t ID;

  if(nF25 == 2){
    mcp.read(0x30, ctr); 
    if(bitRead(ctr[0], 3) == 0){
      noInterrupts();
      nF25 = 0;      
      mcp.bitModify(0x2c, 0x04, 0x00);
    }
  }
  
  while(1){
    
    if(nF == 0){
      return;
    }

    mcp.read(0x30, ctr); 
    if(bitRead(ctr[0], 3) == 1){
      nF25 = 2;
      interrupts();    
      return;
    }
    mcp.bitModify(0x2c, 0x04, 0x00);
      
    mcp.read(0x40, ctr);
    if(bitRead(ctr[0], 3) == 1){
      tx = 0;      
    }else{
      tx = 1;
      mcp.bitModify(0x2c, 0x08, 0x00);
    }

    ID = (buff[i][0] << 8 | buff[i][1]) << 18 | ((buff[i][2] << 8 | buff[i][3]) << 8 | buff[i][4]);
    for(uint8_t k=0; k<buff[i][5]; k++){
      TX[k] = buff[i][6+k];
    }
    
    static uint32_t id = 0x7FC;
    
    mcp.writeID(id, tx);
    mcp.writeDATA(buff[i][5], TX, tx);
    mcp.send(tx+1);
    
    id = id + 1;
    if(id > 4095){
      id = 0x7FC;
    }

    nF = nF - 1;
    i = i + 1;
    if(i == n){
      i = 0;            
    }
  }
}
  
//####################3333333#######################################
void putinBuff(uint16_t IDstd, uint32_t IDext, uint8_t nB, uint8_t *data){
  static uint8_t i = 0;

  if(IDstd > 2047 or IDext > 262143){
    //erro IDstd ou IDext invalido;
    return;
  }

  buff[i][0] = (IDstd >> 8) | 7;
  buff[i][1] = IDstd;
  
  buff[i][2] = (IDext >> 16) | 3;
  buff[i][3] = IDext >> 8;
  buff[i][4] = IDext;
  
  buff[i][5] = nB;

  for(uint8_t k = 0; k < nB; k++){
    buff[i][6+k] = data[k];
  }
  
  i = i + 1;
  if(i == n){
    i = 0;
  }

 
  nF = nF + 1;
  if(nF > n){
    nF = n;
  }
  
  if(nF25 < 2){
    sendCAN();
  }
  return;
  
}
