const uint8_t n = 6;
uint8_t buff[n][14], nF = 0, nF25 = 0;
MCP2515 mcp(4);

typedef union
{
 //data formats
 long int INT;
 unsigned long int uINT;
 float FLT;
 
 int16_t INT16;
 uint16_t uINT16;

 int32_t INT32;
 uint32_t uINT32;

 int64_t  INT64;
 uint64_t  uINT64;
 
 uint8_t bytes[8];
 
} UNION_t;

void sendCAN(){
  static uint8_t i = 0;//, INT = 0;
  uint8_t ctr[1] = {0}, tx = 0, TX[8];
  IDunion_t sID, eID;

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

    CANframe FTS(buff[i]);
    mcp.writeFrame(FTS, tx);

    nF = nF - 1;
    i = i + 1;
    if(i == n){
      i = 0;            
    }
  }
}

void putinBuff(uint16_t IDstd, uint32_t IDext, uint16_t nB, uint8_t *data){
  static uint8_t i = 0;
  uint8_t nb = 0;
  IDunion_t sID, eID;

  if(nB > 8*n){
    //nB muito grande,
    return;
  }
  
  if(IDstd > 0x7FF or IDext > 0x3FFFF){
    //erro IDstd ou IDext invalido;
    return;
  }

  sID.u16 = IDstd;
  eID.u32 = IDext;
  
  uint8_t k = 0;
  while(nB >= k){
  
    buff[i][6+nb] = data[k];
    k = k + 1;
    nb = nb +1;
    if(nb == 8 | k == nB){
      buff[i][0] = sID.bytes[1];
      buff[i][1] = sID.bytes[0];
      buff[i][2] = eID.bytes[2];
      buff[i][3] = eID.bytes[1];
      buff[i][4] = eID.bytes[0];
      buff[i][5] = nb;

      nb = 0;
      i = i + 1;
      if(i == n){
        i = 0;
      }

      nF = nF + 1;
      if(nF > n){
        nF = n;
      }
    }
  }
  
  if(nF25 < 2){
    sendCAN();
  }
  return;
}


void putinBuff(uint16_t IDstd, uint16_t nB, uint8_t *data){
  static uint8_t i = 0;
  uint8_t nb = 0;
  IDunion_t sID;

  if(nB > 8*n){
    //nB muito grande,
    return;
  }
  
  if(IDstd > 0x7FF){
    //erro IDstd invalido;
    return;
  }

  sID.u16 = IDstd;
  
  uint8_t k = 0;
  while(nB >= k){
  
    buff[i][6+nb] = data[k];
    k = k + 1;
    nb = nb +1;
    if(nb == 8 | k == nB){
      buff[i][0] = sID.bytes[1];
      buff[i][1] = sID.bytes[0];
      buff[i][2] = 0;
      buff[i][3] = 0;
      buff[i][4] = 0;
      buff[i][5] = nb;

      nb = 0;
      i = i + 1;
      if(i == n){
        i = 0;
      }

      nF = nF + 1;
      if(nF > n){
        nF = n;
      }
    }   
  }
  
  if(nF25 < 2){
    sendCAN();
  }
  return;  
}
