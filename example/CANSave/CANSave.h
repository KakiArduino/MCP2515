CANframe rec;

void readCAN () {
 
  noInterrupts();
  static uint8_t bxi = 0;

  if(bxi == n){
    bxi = 0;
  }
  
  mcp.readFrame();

  if (mcp.frameRXB0.type != F("No frame")) {
        for (uint8_t j = 0; j < mcp.frameRXB1.dlc + 6; j++) {
          buff[bxi][j] = mcp.frameRXB0.bts[j];              
        }              
        bxi = bxi + 1;
    }

  if (mcp.frameRXB1.type != F("No frame")) {
        for (uint8_t j = 0; j < mcp.frameRXB1.dlc + 6; j++) {
          buff[bxi][j] = mcp.frameRXB1.bts[j];  
        }        
        bxi = bxi + 1;
  }

  interrupts();
  return;
}


void filesC(){ 
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

  file.open(fileName, O_APPEND | O_RDWR | O_CREAT);
    file.print(F("Arquivo log Mini CAN\nIDstd, IDext, dlc, d1, d2, d3, d4, d5, d6, d7, d8\n"));
    return;
}
  
