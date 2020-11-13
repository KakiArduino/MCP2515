#include <SPI.h>
#include "SdFat.h"
#if ENABLE_SOFTWARE_SPI_CLASS  // Must be set in SdFat/SdFatConfig.h
//
// Pin numbers in templates must be constants.
const uint8_t SOFT_SCK_PIN  = 7;
const uint8_t SOFT_MOSI_PIN = 6;
const uint8_t SOFT_MISO_PIN = 5;

//
// Chip select may be constant or RAM variable.
const uint8_t SD_CHIP_SELECT_PIN = 8;

// SdFat software SPI template
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;


// Test file.
SdFile file;

////////////////////////////////////// MCP2515
#include <MCP2515_1.h>
MCP2515 mcp(0x00, 8, 4);
const uint8_t n = 32; // n =numero maximo de frames armazenados
uint8_t buff[n][13];
char fileName[21] = "MiniLOGG/logg_001.csv";
const unsigned long int maxl = 100; // 60;
uint8_t fileNumber[3] = {0x30, 0x30, 0x31};
#include "CANSave.h"

void setup() {
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  pinMode(A2, INPUT);
  
  Serial.begin(9600);
  // Wait for USB Serial 
  //while (!Serial) {
  //  SysCall::yield();
  //}
  
  
  if(!sd.begin(SD_CHIP_SELECT_PIN, SD_SCK_MHZ(50))){
    while(1){
      digitalWrite(3, HIGH);
      delay(80);
      digitalWrite(3, LOW);
      delay(80);
    }
  }
  delay(100);
  digitalWrite(3, LOW);
  if(!sd.exists("MiniLOGG")){
    sd.mkdir("MiniLOGG");
  }
  filesC();

  delay(100);
  
  for (uint8_t i = 0; i < n; i++) {
      buff[i][4] = 0;
  }
  
  attachInterrupt(digitalPinToInterrupt(2), readCAN, FALLING);
}
//------------------------------------------------------------------------------
void loop() {
  interrupts();
  static uint8_t ii = 0, buts = 0;;
  static unsigned long int joy = 0, jj;
  
  ///////////////////////////////////////////////////////
  if (buff[ii][4] > 0 and buts == 0){    
    long unsigned int micr = micros();

    for(uint8_t kk = 0; kk < buff[ii][4] + 5; kk++){
      if(kk < buff[ii][4] + 5 - 1){
        file.printField(buff[ii][kk], ',');
      }
      else{
        file.printField(buff[ii][kk], '\n');
      }
    }

    micr = micros() - micr;
/*
    for(uint8_t kkjk = 0; kkjk < buff[ii][4] + 5; kkjk++){
        Serial.print(buff[ii][kkjk]);
        Serial.print(F(" "));
    } */


    //coment
    Serial.print(F(" "));
    Serial.print(joy);
    Serial.print(F("° "));
    Serial.print(micr);
    Serial.print(F(" us, "));
    Serial.print(file.fileSize());
    Serial.println(F(" bytes."));
    //coment
    
    buff[ii][4] = 0;     
    joy = joy+1;
  }
  
  ///////////////////////////////////////////////////////
  if(joy >= maxl){
    file.close();
    filesC();
    joy = 0;
  }
  
  ///////////////////////////////////////////////////////
  
  if(digitalRead(A2) == 0){
    buts = putPush(buts);
  }
  
 ///////////////////////////////////////////////////////
  ii = ii + 1;
  if (ii == n) {
    ii = 0;
  }
  
}
#else  // ENABLE_SOFTWARE_SPI_CLASS
#error ENABLE_SOFTWARE_SPI_CLASS must be set non-zero in SdFat/SdFatConfig.h
#endif  //ENABLE_SOFTWARE_SPI_CLASS
