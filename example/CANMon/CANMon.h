MCP2515 mon(4);

CANframe rec;

const uint8_t n = 70; //n = numero maximo de frames armazenados
uint8_t buff[n][14];

const char separator = ' ';

void readCAN () {
 
  noInterrupts();
  static uint8_t bxi = 0;

  if(bxi == n){
    bxi = 0;
  }
  
  mon.readFrame();

  if (mon.frameRXB0.type != F("No frame")) {
        for (uint8_t j = 0; j < mon.frameRXB1.dlc + 6; j++) {
          buff[bxi][j] = mon.frameRXB0.bts[j];              
        }              
        bxi = bxi + 1;
    }

  if (mon.frameRXB1.type != F("No frame")) {
        for (uint8_t j = 0; j < mon.frameRXB1.dlc + 6; j++) {
          buff[bxi][j] = mon.frameRXB1.bts[j];  
        }        
        bxi = bxi + 1;
  }

  interrupts();
  return;
}


void printFrame(const char separator = ','){
  static uint8_t ii = 0;
  
  if ((buff[ii][5]) < 255 && Serial.availableForWrite() > (buff[ii][5] + 6)) {
    rec.reload(buff[ii]);

    Serial.print(rec.id_std);
    Serial.print(separator);
    Serial.print(rec.id_ext);
    Serial.print(separator);
    Serial.print(rec.dlc);
    Serial.print(separator);
    
    for(uint8_t i = 6; i < rec.dlc+6; i++){
      Serial.print(rec.bts[i]);
      Serial.print(separator);
    }
    
    Serial.println();
    buff[ii][5] = 255;
  }

  ii = ii + 1;
  if (ii == n) {
    ii = 0;
  }
}

void writeFrame_serial(){
  static uint8_t ii = 0;
  
  if ((buff[ii][5]) < 255 && Serial.availableForWrite() > (buff[ii][5] + 6)) {    
    Serial.write(buff[ii], (buff[ii][5] + 6));    
    buff[ii][5] = 255;
  }

  ii = ii + 1;
  if (ii == n) {
    ii = 0;
  }
}
