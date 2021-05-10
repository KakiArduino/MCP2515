#include <SPI.h>
#include "SdFat.h"
#if ENABLE_SOFTWARE_SPI_CLASS  // Must be set in SdFat/SdFatConfig.h
//
// Pin numbers in templates must be constants.
const uint8_t SOFT_SCK_PIN  = 7;
const uint8_t SOFT_MOSI_PIN = 6;
const uint8_t SOFT_MISO_PIN = 5;
// Chip select may be constant or RAM variable.
const uint8_t SD_CHIP_SELECT_PIN = 8;
// SdFat software SPI template
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;
// Test file.
SdFile file;

////////////////////////////////////// MCP2515
#include <MCP2515_1.h>
MCP2515 mcp(4);
const uint8_t n = 20; // n =numero maximo de frames armazenados
uint8_t buff[n][14];
char fileName[21] = "LOGG/logg_001.csv";
uint8_t fileNumber[3] = {0x30, 0x30, 0x31};
#include "CANSave.h"

void setup() {
 
  if(!sd.begin(SD_CHIP_SELECT_PIN, SD_SCK_MHZ(50))){
    //Serial.println("CardErr");
    while(1){
      delay(1000);
    }
  }
   
  delay(100);
  if(!sd.exists("LOGG")){
    sd.mkdir("LOGG");
  }
  filesC();
  
  delay(100);

  mcp.begin();  
  if(mcp.errLog != F("no error")){
    //Serial.println(mcp.errLog);
    while(1){
      delay(1000);
    }
  }

  for (uint8_t i = 0; i < n; i++) {
      buff[i][5] = 0;
  }

  attachInterrupt(digitalPinToInterrupt(2), readCAN, FALLING);
}

void loop() {
  interrupts();
  static uint8_t ii = 0, buts = 0;
  static unsigned long int nlines = 0;

  //SDprint function
  if (buff[ii][5] > 0 and buts == 0){
      
    rec.reload(buff[ii]);
    
    file.printField(rec.id_std, ',');
    file.printField(rec.id_ext, ',');
    file.printField(rec.dlc, ',');
    
    for(uint8_t kk = 6; kk < rec.dlc + 6; kk++){
      if(kk < rec.dlc + 6 - 1){
        file.printField(rec.bts[kk], ',');
      }
      else{
        file.printField(rec.bts[kk], '\n');
      }
    }
 
    
    buff[ii][5] = 0;     
    nlines = nlines+1;
  }
  
  if(nlines >= 3600){//maxl){
    file.close();
    filesC();
    nlines = 0;
  }
  
  ii = ii + 1;
  if (ii == n) {
    ii = 0;
  }  
}
#else  // ENABLE_SOFTWARE_SPI_CLASS
#error ENABLE_SOFTWARE_SPI_CLASS must be set non-zero in SdFat/SdFatConfig.h
#endif  //ENABLE_SOFTWARE_SPI_CLASS
