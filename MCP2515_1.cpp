/* Biblioteca para o CI MCP2515, datasheet:
 * http://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-
   CAN-Controller-with-SPI-20001801J.pdf
 * Doc.
*/

#include "MCP2515_1.h"

//Data Frame functions

CANframe::CANframe(uint8_t *frameBytes, uint8_t extFlag = 0){
  IDunion_t std_id;
  IDunion_t ext_id;

  std_id.bytes[0] = frameBytes[1];
  std_id.bytes[1] = frameBytes[0];
  id_std = std_id.u16;

  if(extFlag == 1){
    ext_id.bytes[0] = frameBytes[4];
    ext_id.bytes[1] = frameBytes[3];
    ext_id.bytes[2] = frameBytes[2];
    ext_id.bytes[3] = 0;
    id_ext = ext_id.u32;
    type = F("Ext. Data");
  }else{
    ext_id.bytes[0] = 0;
    ext_id.bytes[1] = 0;
    ext_id.bytes[2] = 0;
    ext_id.bytes[3] = 0;
    id_ext = ext_id.u32;
    type = F("Std. Data");
  }

  dlc = frameBytes[5];

  for (uint8_t i = 0; i < dlc + 6; i++){
    bts[i] = frameBytes[i];
    if(i > 5){
      data[i - 6] = frameBytes[i];
    }
  }

  return;
}
void CANframe::reload(uint8_t *frameBytes, uint8_t extFlag = 0){

  dlc = frameBytes[5];

  for (uint8_t i = 0; i < dlc + 6; i++){

    bts[i] = frameBytes[i];
    if(i > 5){
      data[i - 6] = frameBytes[i];
    }
  }

  IDunion_t std_id;
  IDunion_t ext_id;

  std_id.bytes[0] = frameBytes[1];
  std_id.bytes[1] = frameBytes[0];
  id_std = std_id.u16;

  if(extFlag == 1){
    ext_id.bytes[0] = frameBytes[4];
    ext_id.bytes[1] = frameBytes[3];
    ext_id.bytes[2] = frameBytes[2];
    ext_id.bytes[3] = 0;
    id_ext = ext_id.u32;
    type = F("Ext. Data");
  }else{
    ext_id.bytes[0] = 0;
    ext_id.bytes[1] = 0;
    ext_id.bytes[2] = 0;
    ext_id.bytes[3] = 0;
    id_ext = ext_id.u32;
    type = F("Std. Data");
  }

  return;
}

CANframe::CANframe(uint16_t idstd, uint32_t idext, uint8_t dlc_, uint8_t *data_){

  if(idstd > 0x7FF){
    Serial.println("Erro IDsdt > 0x7FF");
    return;
  }
  if(idext > 0x3FFFF){
    Serial.println("Erro IDsdt > 0x7FF");
    return;
  }
  if(dlc_ > 8){
    Serial.println("Erro DLC > 8");
    return;
  }
  type = F("Ext. Data");
  IDunion_t idstd_;

  id_std = idstd;
  idstd_.u16 = idstd;
  bts[1] = idstd_.bytes[0];
  bts[0] = idstd_.bytes[1];

  IDunion_t idext_;

  id_ext = idext;
  idext_.u32 = idext;
  bts[4] = idext_.bytes[1];
  bts[3] = idext_.bytes[2];
  bts[2] = idext_.bytes[3];

  dlc = dlc_;
  bts[5] = dlc_;

  for (uint8_t i = 0; i < dlc_; i++){
    bts[i + 6] = data_[i];
    data[i] = data_[i];
  }
}
void CANframe::reload(uint16_t idstd, uint32_t idext, uint8_t dlc_, uint8_t *data_){

  if(idstd > 0x7FF){
    Serial.println("Erro IDsdt > 0x7FF");
    return;
  }
  if(idext > 0x3FFFF){
    Serial.println("Erro IDsdt > 0x7FF");
    return;
  }
  if(dlc_ > 8){
    Serial.println("Erro DLC > 8");
    return;
  }

  type = F("Ext. Data");

  IDunion_t idstd_;

  id_std = idstd;
  idstd_.u16 = idstd;
  bts[1] = idstd_.bytes[0];
  bts[0] = idstd_.bytes[1];

  IDunion_t idext_;

  id_ext = idext;
  idext_.u32 = idext;
  bts[4] = idext_.bytes[1];
  bts[3] = idext_.bytes[2];
  bts[2] = idext_.bytes[3];

  dlc = dlc_;
  bts[5] = dlc_;

  for (uint8_t i = 0; i < dlc_; i++){
    bts[i + 6] = data_[i];
    data[i] = data_[i];
  }
}

CANframe::CANframe(uint16_t idstd, uint8_t dlc_, uint8_t *data_){
  if(idstd > 0x7FF){
    Serial.println("Erro IDsdt > 0x7FF");
    return;
  }
  if(dlc_ > 8){
    Serial.println("Erro DLC > 8");
    return;
  }
  id_ext = 0;
  type = F("Std. Data");

  IDunion_t idstd_;

  id_std = idstd;
  idstd_.u16 = idstd;
  bts[1] = idstd_.bytes[0];
  bts[0] = idstd_.bytes[1];

  dlc = dlc_;
  bts[5] = dlc_;

    for (uint8_t i = 0; i < dlc_; i++){
      bts[i + 6] = data_[i];
      data[i] = data_[i];
    }
}
void CANframe::reload(uint16_t idstd, uint8_t dlc_, uint8_t *data_){
  if(idstd > 0x7FF){
    Serial.println("Erro IDsdt > 0x7FF");
    return;
  }
  if(dlc_ > 8){
    Serial.println("Erro DLC > 8");
    return;
  }
  id_ext = 0;
  type = F("Std. Data");
  IDunion_t idstd_;

  id_std = idstd;
  idstd_.u16 = idstd;
  bts[1] = idstd_.bytes[0];
  bts[0] = idstd_.bytes[1];

  dlc = dlc_;
  bts[5] = dlc_;

    for (uint8_t i = 0; i < dlc_; i++){
      bts[i + 6] = data_[i];
      data[i] = data_[i];
    }
}

CANframe::CANframe(){


}
void CANframe::reload(uint8_t dlc_, uint8_t *data_){

  if(dlc_ > 8){
    Serial.println("Erro DLC > 8");
    return;
  }

  dlc = dlc_;
  bts[5] = dlc_;

    for (uint8_t i = 0; i < dlc_; i++){
      bts[i + 6] = data_[i];
      data[i] = data_[i];
    }
}

//MCP2515 object functions

//SPI Construtor
MCP2515::MCP2515(uint8_t spi_cs, unsigned long int spi_speed = 10000000, uint8_t spi_wMode = 0){ //uint8_t clk2515
  SPI_cs = spi_cs;
  SPI_speed = spi_speed;
  SPI_wMode = spi_wMode;
}

//MCP2515 initialize
void MCP2515::begin(){
  if(SPI_speed > 10000000){
    errLog = F("SPI speed must be <= 10 Mhz");
    return;
  }
  if(SPI_wMode != 0 and SPI_wMode != 3){
    errLog = F("SPI mode must be 0 | 3");
    return;
  }
  if(crystalCLK != 4 and crystalCLK != 8 and crystalCLK != 20){
    errLog = F("Cirstal clk supported (MHz): 4 | 8 | 20");
    return;
  }

  pinMode(SPI_cs, OUTPUT);
  digitalWrite(SPI_cs, HIGH);
  SPI.begin();


  reset();
  confMode();
  confINT();
  confRX();
  confTX();
  confFM();
  confCAN();
}

//SPI initialize
void MCP2515::start(){

  SPI.beginTransaction(SPISettings(SPI_speed, MSBFIRST, SPI_wMode));
  digitalWrite(SPI_cs, LOW);
}

//SPI off
void MCP2515::end() {
    digitalWrite(SPI_cs, HIGH);
    SPI.endTransaction();
}

//Reset MCP215
void MCP2515::reset(){
    start();
    SPI.transfer(0xC0);
    end();
    delayMicroseconds(10);
}

//Read bytes from registers
void MCP2515::read(uint8_t reg, uint8_t *data, uint8_t n = 1){
    for(uint8_t i = 0; i < n; i++){
      data[i] = 0;
    }
    start();
    SPI.transfer(0x03);
    SPI.transfer(reg);
    for(uint8_t i = 0; i < n; i++){
      data[i] = SPI.transfer(0x00);
    }
    end();
    return;
}

//Check register value
uint8_t MCP2515::regCheck(uint8_t reg, uint8_t val, uint8_t extraMask = 0xFF){
	uint8_t erroRegCheck = 0;

	uint8_t data[1];
	read(reg, data);
	data[0] = data[0] & extraMask;

	//pode tentar por switch
	//Condicoes para mascaras 0XFF, i. e., sem mascaras
	if ((0x36<=reg && reg <=0x3D) | (0x46<=reg && reg <=0x4D) |
	    (0x56<=reg && reg<=0x5D) | ((reg & 0x0F)==0x0F)){

		if(data[0] != val){
			erroRegCheck = 1;
    	}
		return erroRegCheck;
	}

	//Condicoes sem mascaras continuacao
	//REG de filtros/mascaras nao le/escreve fora do modo conf
	uint8_t FFvet[] = {0x31, 0x33, 0x34, 0x41, 0x43, 0x44, 0x51,
					   0x53, 0x54, 0x18, 0x14, 0x10, 0x08, 0x04,
					   0x00, 0x1B, 0x17, 0x13, 0x0B, 0x07, 0x03,
					   0x1A, 0x16, 0x12, 0x0A, 0x06, 0x02, 0x20,
					   0x24, 0x27, 0x23, 0x26, 0x22, 0x2C, 0x2B,
					   0x2A, 0x29};
	for (uint8_t i = 0; i< sizeof(FFvet); i++){
	  if(reg == FFvet[i]){
	    if(data[0] != val){
		    erroRegCheck = 1;
	    }
	    return erroRegCheck;
	  }
	}

	//Condicoes para os demais valores de mascaras
	//mapa de mascaras para check de escritra
	uint8_t checkMaskMap [25][2];
	//Buff de saida
	checkMaskMap[0][0] = 0x30;  checkMaskMap[0][1] = 0x0B;
	checkMaskMap[1][0] = 0x32;  checkMaskMap[1][1] = 0xEB;
	checkMaskMap[2][0] = 0x35;  checkMaskMap[2][1] = 0x4F;
	checkMaskMap[3][0] = 0x40;  checkMaskMap[3][1] = 0x0B;
	checkMaskMap[4][0] = 0x42;  checkMaskMap[4][1] = 0xEB;
	checkMaskMap[5][0] = 0x45;  checkMaskMap[5][1] = 0x4F;
	checkMaskMap[6][0] = 0x50;  checkMaskMap[6][1] = 0x0B;
	checkMaskMap[7][0] = 0x52;  checkMaskMap[7][1] = 0xEB;
	checkMaskMap[8][0] = 0x55;  checkMaskMap[8][1] = 0x4F;
	checkMaskMap[9][0] = 0x0D;  checkMaskMap[9][1] = 0x07;

	//Buff de entrada
	checkMaskMap[10][0] = 0x60;  checkMaskMap[10][1] = 0x64;
	checkMaskMap[11][0] = 0x70;  checkMaskMap[11][1] = 0x60;
	checkMaskMap[12][0] = 0x0C;  checkMaskMap[12][1] = 0x3F;

	//filtros
	checkMaskMap[13][0] = 0x01;  checkMaskMap[13][1] = 0xEB;
	checkMaskMap[14][0] = 0x05;  checkMaskMap[14][1] = 0xEB;
	checkMaskMap[15][0] = 0x09;  checkMaskMap[15][1] = 0xEB;
	checkMaskMap[16][0] = 0x11;  checkMaskMap[16][1] = 0xEB;
	checkMaskMap[17][0] = 0x15;  checkMaskMap[17][1] = 0xEB;
	checkMaskMap[18][0] = 0x19;  checkMaskMap[18][1] = 0xEB;

	//mascaras
	checkMaskMap[19][0] = 0x21;  checkMaskMap[19][1] = 0xE3;
	checkMaskMap[20][0] = 0x25;  checkMaskMap[20][1] = 0xE3;

	//logger erro
	checkMaskMap[21][0] = 0x1C;  checkMaskMap[21][1] = 0x00;
	checkMaskMap[22][0] = 0x1D;  checkMaskMap[22][1] = 0x00;
	checkMaskMap[23][0] = 0x2D;  checkMaskMap[23][1] = 0xC0;

	//configuracao
	checkMaskMap[24][0] = 0x28;  checkMaskMap[24][1] = 0xC7;

	for(uint8_t i = 0; i< sizeof(checkMaskMap)/sizeof(checkMaskMap[0]); i++){
	  if(checkMaskMap[i][0] == reg){
	    if((data[0] & checkMaskMap[i][1]) != (val & checkMaskMap[i][1])){
	      erroRegCheck = 1;
	    }
	    return erroRegCheck;
	  }
	}

	//Condicoes de registro invalido para escrita
	if (((reg & 0x0F)==0x0E) | (0x61<=reg<=0x6D) |
	     (0x71<=reg<=0x7D) | (0x1C<=reg<=0x1D)){
	  erroRegCheck = 2;
	  return erroRegCheck;
	}
	return erroRegCheck;
}

//Write a byte in a register
void MCP2515::write(uint8_t reg, uint8_t val, uint8_t check = 0){
    start();
    SPI.transfer(0x02);
    SPI.transfer(reg);
    SPI.transfer(val);
    end();

    if(check == 1){
      uint8_t erroWrite =0;
      erroWrite = regCheck(reg, val);

      if(erroWrite == 1){
        errLog = werr;
	      return;
      }

      if(erroWrite == 2){
        errLog = F("Invalid Reg");
		    return;
      }

	  return;
    }
	return;
}

//Write a bits in a register
void MCP2515::bitModify(uint8_t reg, uint8_t mask, uint8_t val, uint8_t check = 0){
	start();
	SPI.transfer(0x05);
	SPI.transfer(reg);
	SPI.transfer(mask);
  SPI.transfer(val);
  end();

  if(check == 1){
    uint8_t erroBitModify =0;
    val = mask & val;

    erroBitModify = regCheck(reg, val);

    if( erroBitModify == 1){
      errLog = werr;
	    return;
    }

    if( erroBitModify == 2){
      errLog = F("Invalid Reg");
		  return;
    }
	  return;
  }
	return;
}

//configuracoes
void MCP2515::confMode(){
  if(regCheck(0x0F, 0x80, 0xE0) != 0){
    uint8_t crt = 0;
    while((regCheck(0x0F, 0x80, 0xE0) != 0) && (crt < 100) ){
      bitModify(0x0F, 0xE0, 0x80);
      crt = crt + 1;
      delayMicroseconds(10);
      if((crt == 100) && (regCheck(0x0F, 0x80, 0xE0) != 0)){
        errLog = F("Conf e1");
        return;
      }
    }
    return;
  }

  if(regCheck(0x0F, 0x80, 0xE0) == 0){
    write(0x0F, wMode, 1);
    if(errLog == werr){
      return;
    }
    return;
  }
}

void MCP2515::confRX(){

  confMode();

  write(0x60, RXB0CTRL, 1);
  if(errLog == werr){
    return;
  }

  write(0x70, RXB1CTRL, 1);
  if(errLog == werr){
    return;
  }

  confMode();
}

void MCP2515::confTX(){
  confMode();
  write(0x30, TXB0CTRL, 1);
  if(errLog == werr){
    return;
  }
  write(0x40, TXB1CTRL, 1);
  if(errLog == werr){
    return;
  }
  write(0x50, TXB2CTRL, 1);
  if(errLog == werr){
    return;
  }
  confMode();
}

void MCP2515::confINT(){
  confMode();
    write(0x2B, CANINTE, 1);
    if(errLog == werr){
      return;
    }
    write(0x0C, BFPCTRL, 1);
    if(errLog == werr){
      return;
    }
    write(0x0D, TXRTSCTRL, 1);
    if(errLog == werr){
      return;
    }
  confMode();
}

void MCP2515::confFM(){

  uint8_t FILreg[6] = {0x0, 0x4, 0x8, 0x10, 0x14, 0x18};
  uint8_t MASreg[2] = {0x20, 0x24};
  IDunion_t Eid, Sid;

  confMode();

  for(uint8_t i = 0; i < 6; i++){
    Eid.u32 = FILext[i];
    Sid.u16 = FILstd[i];

    if(Sid.u16 > 0x7FF){
      errLog = F("confFM: sID must be <= 0x7FF");
      return;
    }
    if(Eid.u32 > 0x3FFFF){
      errLog = F("confFM: eID must be <= 0x3FFFF");
      return;
    }

    //if(Eid.u32 > 0){
      bitWrite(Eid.bytes[2], 3, 1);
    //}else{
      //bitWrite(Eid.bytes[2], 3, 0);
    //}
    write(FILreg[i]+3, Eid.bytes[0], 1);
    write(FILreg[i]+2, Eid.bytes[1], 1);

    Eid.bytes[2] = Eid.bytes[2] | Sid.bytes[0] << 5;
    write(FILreg[i]+1, Eid.bytes[2], 1);
    Sid.bytes[1] = Sid.bytes[1] << 5 | Sid.bytes[0] >> 3;
    write(FILreg[i], Sid.bytes[1], 1);
    if(errLog == werr){
      return;
    }
  }

  for(uint8_t i = 0; i < 2; i++){
    Eid.u32 = MASext[i];
    Sid.u16 = MASstd[i];
    if(Sid.u16 > 0x7FF){
      errLog = F("confFM: sID must be <= 0x7FF");
      return;
    }
    if(Eid.u32 > 0x3FFFF){
      errLog = F("confFM: eID must be <= 0x3FFFF");
      return;
    }
/*
    if(Eid.u32 > 0){
      bitWrite(Eid.bytes[2], 3, 1);
    }else{
      bitWrite(Eid.bytes[2], 3, 0);
    }
*/
    write(MASreg[i]+3, Eid.bytes[0], 1);
    write(MASreg[i]+2, Eid.bytes[1], 1);
    Eid.bytes[2] = Eid.bytes[2] | Sid.bytes[0] << 5;
    write(MASreg[i]+1, Eid.bytes[2], 1);
    Sid.bytes[1] = Sid.bytes[1] << 5 | Sid.bytes[0] >> 3;
    write(MASreg[i], Sid.bytes[1], 1);
    if(errLog == werr){
      return;
    }
  }
  confMode();
  return;
}

void MCP2515::confCAN(){
  //Verifica e coloca em modo de configuracao
  switch (crystalCLK) {
    case 8:
      switch (bitF) {
        case 500:
          break;
        case 250:
          CNF2 = 0x57;
          CNF3 = 0x03;
          break;
        case 125:
          CNF1 = 0x01;
          CNF2 = 0x57;
          CNF3 = 0x03;
          break;
        default:
          errLog = F("Bit frequency must be 500 | 250 | 125 for 8 MHz cristal");
          break;
      };
      break;
    case 4:
      switch (bitF) {
          case 250:
            break;
          case 125:
            CNF2 = 0x57;
            CNF3 = 0x03;
            break;
          default:
            errLog = F("Bit frequency must be 250 | 125 for 4 MHz cristal");
            break;
      };
    break;

    case 20:
      switch (bitF) {
        case 1000:
          CNF2 = 0x44;
          break;
        case 500:
          CNF2 = 0x80;
          CNF2 = 0x6F;
          CNF3 = 0x04;
          break;
        case 250:
          CNF2 = 0x81;
          CNF2 = 0x6F;
          CNF3 = 0x04;
          break;
        case 125:
          CNF2 = 0x83;
          CNF2 = 0x6F;
          CNF3 = 0x04;
          break;
        default:
          errLog = F("Bit frequency must be 1000 | 500 | 250 | 125");
          break;
      };
    break;

    default:
      errLog = F("Cristal clock must be 4 | 8 | 20");
      break;

 };

  confMode();

  write(0x2A, CNF1, 1);
  if(errLog == werr){
    return;
  }

  write(0x29, CNF2, 1);
  if(errLog == werr){
    return;
  }
  write(0x28, CNF3, 1);
  if(errLog == werr){
    return;
  }

  //setando o modo de operacao OPMODE  bitF, crystalCLK
  confMode();
  return;
}

//Contador de erros e status functions
void MCP2515::status(uint8_t *status){
  start();
  SPI.transfer(0xA0);
  status[0] = SPI.transfer(0x00);
  status[0] = SPI.transfer(0x00);
  end();
}

void MCP2515::RXstatus(uint8_t *status){
  start();
  SPI.transfer(0xB0);
  status[0] = SPI.transfer(0x00);
  SPI.transfer(0x00);
  end();
}

void MCP2515::errCount(){
  uint8_t readE[1] = {0};

  read(0x1C, readE);
  TEC = readE[0];

  read(0x1D, readE);
  REC = readE[0];

  read(0x2D, readE);
  if(readE[0] != 0){
    if(bitRead(readE[0], 5) == 1){
      errMode = F("Bus-Off");
    }else{
      if(bitRead(readE[0], 4) == 1){
        errMode = F("Error Passive");
      }else{
        if(bitRead(readE[0], 2) == 1){
          errMode = F("Error Active");
        }
      }
    }
    if(bitRead(readE[0], 3) == 1){
        errMode = F("Error Passive");
    }else{
      if(bitRead(readE[0], 1) == 1){
        errMode = F("Error Active");
      }
    }
  }

  if(bitRead(readE[0], 7) == 1){
    //bitModify(0x2D, 0x80, 0);
    RX1OVR = RX1OVR + 1;
  }
  if(bitRead(readE[0], 6) == 1){
    //bitModify(0x2D, 0x40, 0);
    RX0OVR = RX0OVR + 1;
  }
  write(0x2D, 0);

  read(0x2C, readE);
  if(bitRead(readE[0], 7) == 1){
    //bitModify(0x2C, 0x80, 0);
    MERRF = MERRF + 1;
  }

  if(bitRead(readE[0], 6) == 1){
    //bitModify(0x2C, 0x40, 0);
    WAKIE = 1;
  }else{
    WAKIE = 0;
  }

  if(bitRead(readE[0], 5) == 1){
    //bitModify(0x2C, 0x20, 0);
    multInt = multInt + 1;
  }
  //interrupt flags reset
  bitModify(0x2C, 0xE0, 0x00);
  return;
}

//Writing functions
void MCP2515::writeID(uint16_t sid, uint8_t id_ef = 0, uint32_t eid = 0, uint8_t txb = 0, uint8_t timeOut = 10, uint8_t check = 0){
    uint32_t t = micros();
    uint8_t txcrt[1]={0};

    if(sid > 0x7FF){
      errLog = F("sID must be <= 0x7FF");
      return;
    }
    if(eid > 0x3FFFF){
      errLog = F("eID must be <= 0x3FFFF");
      return;
    }

    switch(txb){
      case 0:
        txb = 0x30;
        break;

      case 1:
        txb = 0x40;
        break;

      case 2:
        txb = 0x50;
        break;

      default:
        errLog = F("TXB must be 0 | 1 | 2");
        return;
    };

    IDunion_t id_ext, id_std;

    id_std.u16 = sid;

    if(id_ef > 0){
      id_ext.u32 = eid;
      bitWrite(id_ext.bytes[2], 3, 1);
    }else{
      id_ext.u32 = 0;
      bitWrite(id_ext.bytes[2], 3, 0);
    }
    do {
      read(txb, txcrt);
      if(bitRead(txcrt[0], 3) == 0 ){
        write(txb+4, id_ext.bytes[0], check);
        write(txb+3, id_ext.bytes[1], check);

        id_ext.bytes[2] = id_ext.bytes[2] | id_std.bytes[0] << 5;

        write(txb+2, id_ext.bytes[2], check);

        id_std.bytes[1] = id_std.bytes[1] << 5 | id_std.bytes[0] >> 3;
        write(txb+1, id_std.bytes[1], check);
        return;
      }
      delayMicroseconds(10);
    } while(micros() - t < timeOut*1000);

}

void MCP2515::loadTX(uint8_t *data, uint8_t n = 8, uint8_t abc = 1){
	/* Descricao da funcao...
	*
	* 	1) Verifica se abc e valido, ele tem que ser que esta no intervalo de
	* 0 a 5 de acordo com a pagina 69 do datasheet.
	*
	* 	2) Realiza a transferencia dos bytes contidos em data
	*
	*	3) futuro ->  possibilidade de abilitar um check.
	*
	* Argumentos, 	data : posicao da memoria onde inicia o vetor de dados
	*				abc  : numero inteiro de 0 a 5, de acordo com a instrucao pg. 69
	*				n    : numero de registros a serem escritos
	*	futuro ->	arg4 : possibilidade de abilitar um check.
	*
	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

  if(abc > 5){
    errLog = F("Invalid abc");
    return;
  }

  start();
  SPI.transfer( 0x40 + abc );

  for (uint8_t i = 0; i < n; i ++){
    SPI.transfer(data[i]);
  }
  end();

  return;
}

void MCP2515::send(uint8_t txBuff = 0x01){

  start();
  SPI.transfer(0x80 + txBuff);
  end();
}

void MCP2515::writeFrame(CANframe frameToSend, uint8_t txb_ = 0, uint8_t timeOut = 10, uint8_t check = 0){

  uint8_t txcrt[1] = {0xFF}, sendCode, loadCode, txb;
  uint32_t  t = micros();

  errLog = F("no error");

  if(frameToSend.dlc > 8){
    errLog == F("The data field size must be <= 8 bytes");
    return;
  }

	switch (txb_){
		case 0:
      txb = 0x30;
      sendCode = 0x01;
      loadCode = 0x01;
		  break;

		case 1:
      txb = 0x40;
      sendCode = 0x02;
      loadCode = 0x03;
		  break;

		case 2:
      txb = 0x50;
		  sendCode = 0x04;
      loadCode = 0x05;
		  break;

		default:
			errLog = F("TXB must be 0 | 1 | 2");
			return;
	};

  do {
    read(txb, txcrt);
    if(bitRead(txcrt[0], 3) == 0 ){
      write(txb+5, frameToSend.dlc, check); // alterar para permitir frame de pedido remoto
      if(errLog == werr){
        return;
      }
      if(frameToSend.type == F("Ext. Data")){
        writeID(frameToSend.id_std, 1, frameToSend.id_ext, txb_);
      }
      if(frameToSend.type == F("Std. Data")){
        writeID(frameToSend.id_std, 0, 0, txb_);
      }else{
        errLog = F("Invalid Frame type");
      }
      loadTX(frameToSend.data, frameToSend.dlc, loadCode);
      send(sendCode);
      return;
    }
    delayMicroseconds(10);
  } while((micros() - t) < timeOut*1000);

	errLog == F("Buffs full");
	return;
}

void MCP2515::abort(uint8_t abortCode = 7){

                              uint8_t rreadd[1];

  if(abortCode == 7){
                              read(0x0F, rreadd);
                              Serial.println(rreadd[0], HEX);
    bitModify(0x0F,  0x10, 0x10);
                              read(0x0F, rreadd);
                              Serial.println(rreadd[0], HEX);
    return;
  }

  confMode();
  if((abortCode & 0x1) == 0x1){
                              read(0x30, rreadd);
                              Serial.println(rreadd[0], HEX);
                bitWrite(TXB0CTRL, 3, 0);
                write(0x30, TXB0CTRL);
                bitWrite(TXB0CTRL, 3, 1);
    bitModify(0x30,  0x08, 0x00);
                              read(0x30, rreadd);
                              Serial.println(rreadd[0], HEX);
  }
  if((abortCode & 0x2) == 0x2){

                              read(0x40, rreadd);
                              Serial.println(rreadd[0], HEX);
                  bitWrite(TXB1CTRL, 3, 0);
                  write(0x40, TXB1CTRL);
                  bitWrite(TXB1CTRL, 3, 1);
    bitModify(0x40,  0x08, 0x00);
                              read(0x40, rreadd);
                              Serial.println(rreadd[0], HEX);

  }
  if((abortCode & 0x4) == 0x4){
                              read(0x50, rreadd);
                              Serial.println(rreadd[0], HEX);
                  bitWrite(TXB2CTRL, 3, 0);
                  write(0x50, TXB2CTRL);
                  bitWrite(TXB2CTRL, 3, 1);
    bitModify(0x50,  0x08, 0x00);
                              read(0x50, rreadd);
                              Serial.println(rreadd[0], HEX);

  }
  confMode();
  return;

}

//Reading functions
void MCP2515::readID(uint8_t *id, uint8_t rxb = 0){

  id[0] = 0;
  id[1] = 0;
  id[2] = 0;
  id[3] = 0;
  id[4] = 0;

  switch(rxb){
    case 0: //RXB0
      rxb = 0x61;
      //read(0x61, id, 4); //SID[0] = SIDH e SID[1] = SIDL
      break;

    case 1:
      rxb = 0x71;
      //read(0x71, id, 4);
      break;

    default:
      errLog  = F("RXB must be 0 | 1");
      return;
  };

  read(rxb, id, 4);
  id[4] = id[3];
  id[3] = id[2];
  id[2] = id[1]  & 0x3;
  id[1] = id[1] >> 5 | id[0] << 3;
  id[0] = id[0] >> 5;

  return;
}

void MCP2515::readFrame(){

  frameRXB0.type = F("No frame");
  frameRXB1.type = F("No frame");

  uint8_t rx_status[1];
  RXstatus(rx_status);
  if((rx_status[0] >> 6) == 0){
    return;
  }

  switch((rx_status[0] >> 6)){
    case 1: //Only RXB0 have a new data
      read(0x65, frameRXB0.bts);

      frameRXB0.dlc = frameRXB0.bts[0] & 0xF;

      read(0x66, frameRXB0.bts, frameRXB0.dlc);

      frameRXB0.bts[13] = frameRXB0.bts[7];
      frameRXB0.bts[12] = frameRXB0.bts[6];
      frameRXB0.bts[11] = frameRXB0.bts[5];
      frameRXB0.bts[10] = frameRXB0.bts[4];
      frameRXB0.bts[9] = frameRXB0.bts[3];
      frameRXB0.bts[8] = frameRXB0.bts[2];
      frameRXB0.bts[7] = frameRXB0.bts[1];
      frameRXB0.bts[6] = frameRXB0.bts[0];
      frameRXB0.bts[5] = frameRXB0.dlc;

      readID(frameRXB0.bts, 0);
      if((rx_status[0] & 0x18) == 0x10){
        frameRXB0.type = F("Ext. Data");
        frameRXB0.reload(frameRXB0.bts, 1);
        //interrupt flags reset
        bitModify(0x2C, 0x01, 0);
        return;
      }
      if((rx_status[0] & 0x18) == 0){
        frameRXB0.type = F("Std. Data");
        frameRXB0.id_ext = 0;
        frameRXB0.reload(frameRXB0.bts);
        //interrupt flags reset
        bitModify(0x2C, 0x01, 0);
        return;
      }
      if((rx_status[0] & 0x18) == 0x08){
        frameRXB0.type = "Std. Remote";
        frameRXB0.id_ext = 0;
        frameRXB0.reload(frameRXB0.bts);
        //interrupt flags reset
        bitModify(0x2C, 0x01, 0);
        return;
      }
      if((rx_status[0] & 0x18) == 0x18){
        frameRXB0.type = F("Ext. Remote");
        frameRXB0.reload(frameRXB0.bts, 1);
        //interrupt flags reset
        bitModify(0x2C, 0x01, 0);
        return;
      }

    case 2:  //Only RXB1 have a new data

      read(0x75, frameRXB1.bts);

      frameRXB1.bts[8] = frameRXB1.bts[0] & 0xF;

      read(0x76, frameRXB1.bts, frameRXB1.bts[8]);

      frameRXB1.bts[13] = frameRXB1.bts[7];
      frameRXB1.bts[12] = frameRXB1.bts[6];
      frameRXB1.bts[11] = frameRXB1.bts[5];
      frameRXB1.bts[10] = frameRXB1.bts[4];
      frameRXB1.bts[9] = frameRXB1.bts[3];
      frameRXB1.bts[8] = frameRXB1.bts[2];
      frameRXB1.bts[7] = frameRXB1.bts[1];
      frameRXB1.bts[6] = frameRXB1.bts[0];
      frameRXB1.bts[5] = frameRXB1.dlc;

      readID(frameRXB1.bts, 1);
      if((rx_status[0] & 0x18) == 0x10){
        frameRXB1.type = F("Ext. Data");
        frameRXB0.reload(frameRXB0.bts, 1);
        //interrupt flags reset
        bitModify(0x2C, 0x02, 0);
        return;
      }
      if((rx_status[0] & 0x18) == 0){
        frameRXB1.type = F("Std. Data");
        frameRXB1.id_ext = 0;
        frameRXB0.reload(frameRXB0.bts);
        //interrupt flags reset
        bitModify(0x2C, 0x02, 0);
        return;
      }
      if((rx_status[0] & 0x18) == 0x08){
        frameRXB1.type = F("Std. Remote");
        frameRXB1.id_ext = 0;
        frameRXB0.reload(frameRXB0.bts);
        //interrupt flags reset
        bitModify(0x2C, 0x02, 0);
        return;
      }
      if((rx_status[0] & 0x18) == 0x18){
        frameRXB1.type = F("Ext. Remote");
        frameRXB0.reload(frameRXB0.bts, 1);
        //interrupt flags reset
        bitModify(0x2C, 0x02, 0);
        return;
      }

    case 3: // Both receivers buffers have a new data

      read(0x65, frameRXB0.bts);
      read(0x75, frameRXB1.bts);

      frameRXB0.dlc = frameRXB0.bts[0] & 0xF;
      frameRXB1.dlc = frameRXB1.bts[0] & 0xF;


      read(0x66, frameRXB0.bts, frameRXB0.dlc);
      read(0x76, frameRXB1.bts, frameRXB1.dlc);

      frameRXB0.bts[13] = frameRXB0.bts[7];
      frameRXB0.bts[12] = frameRXB0.bts[6];
      frameRXB0.bts[11] = frameRXB0.bts[5];
      frameRXB0.bts[10] = frameRXB0.bts[4];
      frameRXB0.bts[9] = frameRXB0.bts[3];
      frameRXB0.bts[8] = frameRXB0.bts[2];
      frameRXB0.bts[7] = frameRXB0.bts[1];
      frameRXB0.bts[6] = frameRXB0.bts[0];
      frameRXB0.bts[5] = frameRXB0.dlc;

      frameRXB1.bts[13] = frameRXB1.bts[7];
      frameRXB1.bts[12] = frameRXB1.bts[6];
      frameRXB1.bts[11] = frameRXB1.bts[5];
      frameRXB1.bts[10] = frameRXB1.bts[4];
      frameRXB1.bts[9] = frameRXB1.bts[3];
      frameRXB1.bts[8] = frameRXB1.bts[2];
      frameRXB1.bts[7] = frameRXB1.bts[1];
      frameRXB1.bts[6] = frameRXB1.bts[0];
      frameRXB1.bts[5] = frameRXB1.dlc;

      readID(frameRXB0.bts, 0);

      read(0x62, rx_status);
      if((rx_status[0] & 0x08) == 0){
        frameRXB0.type = F("Std. Data");
        frameRXB0.id_ext = 0;
        frameRXB0.reload(frameRXB0.bts);
      }
      if((rx_status[0] & 0x08) == 0x08){
        frameRXB0.type = F("Ext. Data");
        read(0x65, rx_status);
        if((rx_status[0] & 0x40) == 0x40){
          frameRXB0.type = F("Ext. Remote");
        }
        frameRXB0.reload(frameRXB0.bts, 1);
      }

      readID(frameRXB1.bts, 1);

      read(0x72, rx_status);
      if((rx_status[0] & 0x08) == 0){
        frameRXB1.type = F("Std. Data");
        frameRXB1.id_ext = 0;
        frameRXB1.reload(frameRXB0.bts);
      }
      if((rx_status[0] & 0x08) == 0x08){
        frameRXB1.type = F("Ext. Data");
        read(0x75, rx_status);
        if((rx_status[0] & 0x40) == 0x40){
          frameRXB1.type = F("Ext. Remote");
        }
        frameRXB1.reload(frameRXB0.bts, 1);
      }
      //interrupt flags reset
      bitModify(0x2C, 0x03, 0x00);
      return;

    default:

      return;
  };

}

void MCP2515::quickread(){
  frameRXB0.type = F("No frame");
  frameRXB1.type = F("No frame");

  uint8_t rx_status[1];
  RXstatus(rx_status);

  if((rx_status[0] >> 6) == 0){
    return;
  }
  switch((rx_status[0] >> 6)){
    case 1: //Only RXB0 have a new data
      read(0x61, frameRXB0.bts, 13);

      for(uint8_t i = 13; i > 2; i--){
        frameRXB0.bts[i] = frameRXB0.bts[i - 1];
      }

      frameRXB0.bts[2] = frameRXB0.bts[1]  & 0x3;
      frameRXB0.bts[1] = frameRXB0.bts[1] >> 5 | frameRXB0.bts[0] << 3;
      frameRXB0.bts[0] = frameRXB0.bts[0] >> 5;

      frameRXB0.type = F("Unknown");
      bitModify(0x2C, 0x01, 0);
      return;

    case 2:  //Only RXB1 have a new data
      read(0x71, frameRXB1.bts, 13);

      for(uint8_t i = 13; i > 2; i--){
        frameRXB1.bts[i] = frameRXB0.bts[i - 1];
      }

      frameRXB1.bts[2] = frameRXB1.bts[1]  & 0x3;
      frameRXB1.bts[1] = frameRXB1.bts[1] >> 5 | frameRXB1.bts[0] << 3;
      frameRXB1.bts[0] = frameRXB1.bts[0] >> 5;

      frameRXB1.type = F("Unknown");
      bitModify(0x2C, 0x02, 0);
      return;

    case 3: // Both receivers buffers have a new data
      read(0x61, frameRXB0.bts, 13);

      for(uint8_t i = 13; i > 2; i--){
        frameRXB0.bts[i] = frameRXB0.bts[i - 1];
      }

      frameRXB0.bts[2] = frameRXB0.bts[1]  & 0x3;
      frameRXB0.bts[1] = frameRXB0.bts[1] >> 5 | frameRXB0.bts[0] << 3;
      frameRXB0.bts[0] = frameRXB0.bts[0] >> 5;

      read(0x71, frameRXB1.bts, 13);

      for(uint8_t i = 13; i > 2; i--){
        frameRXB1.bts[i] = frameRXB0.bts[i - 1];
      }

      frameRXB1.bts[2] = frameRXB1.bts[1]  & 0x3;
      frameRXB1.bts[1] = frameRXB1.bts[1] >> 5 | frameRXB1.bts[0] << 3;
      frameRXB1.bts[0] = frameRXB1.bts[0] >> 5;


      frameRXB0.type = F("Unknown");
      frameRXB1.type = F("Unknown");
      bitModify(0x2C, 0x03, 0x00);
      return;

    default:

      return;
  };
}

//General informations
void MCP2515::digaOi(){
  uint8_t readE[1] = {0};
////////////////////////////////////////////////////////////////////
  Serial.println(F("Status reg"));
  read(0x0E, readE);
  switch (readE[0] & 0xE0){
    case 0:
      Serial.println(F("Normal mode"));
      break;
    case 1:
      Serial.println(F("Sleep mode"));
      break;
    case 2:
      Serial.println(F("Loopback mode"));
      break;
    case 3:
      Serial.println(F("Listen-Only mode"));
      break;
    case 4:
      Serial.println(F("Configuration mode"));
      break;
  };
  switch (readE[0] & 0x0E){
    case 0:
      Serial.println(F("No interrupt flags"));
      break;
    case 1:
      Serial.println(F("Error interrupt"));
      break;
    case 2:
      Serial.println(F("Wake-up interrupt"));
      break;
    case 3:
      Serial.println(F("TXB0 interrupt"));
      break;
    case 4:
      Serial.println(F("TXB1 interrupt"));
      break;
    case 5:
      Serial.println(F("TXB2 interrupt"));
      break;
    case 6:
      Serial.println(F("RXB0 interrupt"));
      break;
    case 7:
      Serial.println(F("RXB1 interrupt"));
      break;
  };
  Serial.println();
/////////////////////////////////////////////////////////////////////////////////
  Serial.println(F("Errors"));
  Serial.print(F("Logg_error: "));
  Serial.println(errLog);

  errCount();
  Serial.print(F("TEC: "));
  Serial.println();
  Serial.print(F("REC: "));
  Serial.println();
  Serial.print(F("Confinament Mode: "));
  Serial.println(errMode);
  Serial.print(F("RB1_Over_Flow_cont: "));
  Serial.println(RX1OVR);
  Serial.print(F("RB0_Over_Flow_cont: "));
  Serial.println(RX0OVR);
  Serial.println();
/////////////////////////////////////////////////////////////////////////////
  Serial.println(F("SPI setted  parameters"));
  Serial.print(F("SPI_max_speed: "));
  Serial.println(SPI_speed);
  Serial.print(F("SPI_work_mode: "));
  Serial.println(SPI_wMode);
  Serial.print(F("SPI_CS: "));
  Serial.println(SPI_cs);
  Serial.println();
///////////////////////////////////////////////////////////////////////////
  Serial.println(F("CAN_2515 setted parameters"));
  Serial.print(F("CLK_2515: "));
  Serial.println(crystalCLK);
  Serial.print(F("Bits_frequency: "));
  Serial.println(bitF);
  Serial.print(F("CAN_work_mode: "));
  switch (wMode) {
    case 0:
      Serial.println(F("Normal mode"));
      break;
    case 1:
      Serial.println(F("Sleep mode"));
      break;
    case 2:
      Serial.println(F("Loopback mode"));
      break;
    case 3:
      Serial.println(F("Listen-Only mode"));
      break;
    case 4:
      Serial.println(F("Configuration mode"));
      break;
  };
  Serial.println();
////////////////////////////////////////////////////////////////////////////////
  Serial.println(F("RX, TX and INT setted parameters in HEX"));
  Serial.print(F("RXB0: "));
  Serial.println(RXB0CTRL, HEX);
  Serial.print(F("RXB1: "));
  Serial.println(RXB1CTRL, HEX);
  Serial.print(F("TXB0: "));
  Serial.println(TXB0CTRL, HEX);
  Serial.print(F("TXB1: "));
  Serial.println(TXB1CTRL, HEX);
  Serial.print(F("TXB2: "));
  Serial.println(TXB2CTRL, HEX);
  Serial.print(F("CANINTE: "));
  Serial.println(CANINTE, HEX);
  Serial.print(F("BFPCTRL: "));
  Serial.println(BFPCTRL, HEX);
  Serial.print(F("TXRTSCTRL: "));
  Serial.println(TXRTSCTRL, HEX);
  Serial.println();
///////////////////////////////////////////////////////////
Serial.println(F("Setted filters in HEX"));
Serial.print(F("Filter_0: "));
Serial.print(FILstd[0], HEX);
Serial.print(F(", "));
Serial.println(FILext[0], HEX);

Serial.print(F("Filter_1: "));
Serial.print(FILstd[1], HEX);
Serial.print(F(", "));
Serial.println(FILext[1], HEX);

Serial.print(F("Filter_2: "));
Serial.print(FILstd[2], HEX);
Serial.print(F(", "));
Serial.println(FILext[2], HEX);

Serial.print(F("Filter_3: "));
Serial.print(FILstd[3], HEX);
Serial.print(F(", "));
Serial.println(FILext[3], HEX);

Serial.print(F("Filter_4: "));
Serial.print(FILstd[4], HEX);
Serial.print(F(", "));
Serial.println(FILext[4], HEX);

Serial.print(F("Filter_5: "));
Serial.print(FILstd[5], HEX);
Serial.print(F(", "));
Serial.println(FILext[5], HEX);
Serial.println();
//////////////////////////////////////////////////////////////////
Serial.println(F("Setted Maks in HEX"));
Serial.print(F("Mask_0: "));
Serial.print(MASstd[0], HEX);
Serial.print(F(", "));
Serial.println(MASext[0], HEX);
Serial.print(F("Mask_1: "));
Serial.print(MASstd[1], HEX);
Serial.print(F(", "));
Serial.println(MASext[1], HEX);

Serial.println();
  return;
}

void MCP2515::digaOi(char *oi){
  uint8_t readE[1] = {0};

////////////////////////////////////////////////////////////////////
if(oi[1] == 't'){
  Serial.println(F("Status reg"));
  read(0x0E, readE);
  switch (readE[0] & 0xE0){
    case 0:
      Serial.println(F("Normal mode"));
      break;
    case 1:
      Serial.println(F("Sleep mode"));
      break;
    case 2:
      Serial.println(F("Loopback mode"));
      break;
    case 3:
      Serial.println(F("Listen-Only mode"));
      break;
    case 4:
      Serial.println(F("Configuration mode"));
      break;
  };
  switch (readE[0] & 0x0E){
    case 0:
      Serial.println(F("No interrupt flags"));
      break;
    case 1:
      Serial.println(F("Error interrupt"));
      break;
    case 2:
      Serial.println(F("Wake-up interrupt"));
      break;
    case 3:
      Serial.println(F("TXB0 interrupt"));
      break;
    case 4:
      Serial.println(F("TXB1 interrupt"));
      break;
    case 5:
      Serial.println(F("TXB2 interrupt"));
      break;
    case 6:
      Serial.println(F("RXB0 interrupt"));
      break;
    case 7:
      Serial.println(F("RXB1 interrupt"));
      break;
  };
  Serial.println();
  return;
}
/////////////////////////////////////////////////////////////////////////////////
if(oi[1] == 'r'){
  Serial.println(F("Errors"));
  Serial.print(F("Logg_error: "));
  Serial.println(errLog);

  errCount();
  Serial.print(F("TEC: "));
  Serial.println();
  Serial.print(F("REC: "));
  Serial.println();
  Serial.print(F("Confinament Mode: "));
  Serial.println(errMode);
  Serial.print(F("oflw_RB1_cont: "));
  Serial.println(RX1OVR);
  Serial.print(F("oflw_RB0_cont: "));
  Serial.println(RX0OVR);
  Serial.println();
  return;
}
/////////////////////////////////////////////////////////////////////////////
if(oi[1] == 'P'){
  Serial.println(F("SPI setted  parameters"));
  Serial.print(F("SPI_max_speed: "));
  Serial.println(SPI_speed);
  Serial.print(F("SPI_work_mode: "));
  Serial.println(SPI_wMode);
  Serial.print(F("SPI_CS: "));
  Serial.println(SPI_cs);
  Serial.println();
  return;
}
///////////////////////////////////////////////////////////////////////////
if(oi[1] == 'A'){
  Serial.println(F("CAN_2515 setted parameters"));
  Serial.print(F("CLK_2515: "));
  Serial.println(crystalCLK);
  Serial.print(F("Bits_freq: "));
  Serial.println(bitF);
  Serial.print(F("CAN_work_mode: "));
  switch (wMode) {
    case 0:
      Serial.println(F("Normal mode"));
      break;
    case 1:
      Serial.println(F("Sleep mode"));
      break;
    case 2:
      Serial.println(F("Loopback mode"));
      break;
    case 3:
      Serial.println(F("Listen-Only mode"));
      break;
    case 4:
      Serial.println(F("Configuration mode"));
      break;
  };
  Serial.println();
  return;
}
////////////////////////////////////////////////////////////////////////////////
if(oi[1] == 'o'){
  Serial.println(F("RX, TX and INT setted parameters in HEX"));
  Serial.print(F("RXB0: "));
  Serial.println(RXB0CTRL, HEX);
  Serial.print(F("RXB1: "));
  Serial.println(RXB1CTRL, HEX);
  Serial.print(F("TXB0: "));
  Serial.println(TXB0CTRL, HEX);
  Serial.print(F("TXB1: "));
  Serial.println(TXB1CTRL, HEX);
  Serial.print(F("TXB2: "));
  Serial.println(TXB2CTRL, HEX);
  Serial.print(F("CANINTE: "));
  Serial.println(CANINTE, HEX);
  Serial.print(F("BFPCTRL: "));
  Serial.println(BFPCTRL, HEX);
  Serial.print(F("TXRTSCTRL: "));
  Serial.println(TXRTSCTRL, HEX);
  Serial.println();
  return;
}
///////////////////////////////////////////////////////////
if(oi[1] == 'i'){
  Serial.println(F("Setted filters in HEX"));
  Serial.print(F("Filter_0: "));
  Serial.print(FILstd[0], HEX);
  Serial.print(F(", "));
  Serial.println(FILext[0], HEX);

  Serial.print(F("Filter_1: "));
  Serial.print(FILstd[1], HEX);
  Serial.print(F(", "));
  Serial.println(FILext[1], HEX);

  Serial.print(F("Filter_2: "));
  Serial.print(FILstd[2], HEX);
  Serial.print(F(", "));
  Serial.println(FILext[2], HEX);

  Serial.print(F("Filter_3: "));
  Serial.print(FILstd[3], HEX);
  Serial.print(F(", "));
  Serial.println(FILext[3], HEX);

  Serial.print(F("Filter_4: "));
  Serial.print(FILstd[4], HEX);
  Serial.print(F(", "));
  Serial.println(FILext[4], HEX);

  Serial.print(F("Filter_5: "));
  Serial.print(FILstd[5], HEX);
  Serial.print(F(", "));
  Serial.println(FILext[5], HEX);
  Serial.println();
  return;
}
//////////////////////////////////////////////////////////////////
if(oi[1] == 'a'){
  Serial.println(F("Setted Maks in HEX"));

  Serial.print(F("Mask_0: "));
  Serial.print(MASstd[0], HEX);
  Serial.print(F(", "));
  Serial.println(MASext[0], HEX);

  Serial.print(F("Mask_1: "));
  Serial.print(MASstd[1], HEX);
  Serial.print(F(", "));
  Serial.println(MASext[1], HEX);

  Serial.println();
  return;
}

Serial.println(F("Invalid Parameter for digaOi() \ntry: nothing | Status? | Errors? | SPI? | CAN? | ConfRegs? | Filters? | Masks?"));
return;
}
