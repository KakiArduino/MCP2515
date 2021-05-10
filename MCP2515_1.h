/* Biblioteca para o CI MCP2515, datasheet disponivel em:
 * http://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-CAN-Controller-with-SPI-20001801J.pdf
 *
*/
#ifndef MCP2515_1_h
#define MCP2515_1_h
#include <Arduino.h>
#include <SPI.h>

typedef union {
 uint16_t u16;
 uint32_t u32;
 uint8_t bytes[4];
} IDunion_t;

const String werr = "Reg writing error";

struct CANframe{
  uint16_t id_std = 0;
  uint32_t id_ext = 0;
  uint8_t dlc = 0;
  uint8_t data[8] = {0,0,0,0,0,0,0,0};
  uint8_t bts[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  String type = "Unknown";

 CANframe();

 CANframe(uint8_t *frameBytes, uint8_t extFlag = 0);
 void reload(uint8_t *frameBytes, uint8_t extFlag = 0);

 CANframe(uint16_t idstd, uint32_t idext, uint8_t dlc_, uint8_t *data);
 void reload(uint16_t idstd, uint32_t idext, uint8_t dlc_, uint8_t *data);

 CANframe(uint16_t idstd, uint8_t dlc_, uint8_t *data);
 void reload(uint16_t idstd, uint8_t dlc_, uint8_t *data);

 void reload(uint8_t dlc_, uint8_t *data_);

};

class MCP2515{

private:
  void start();
  void end();

public:

   //SPI valores conf.
   unsigned long int SPI_speed = 10000000;
   uint8_t SPI_wMode = 0;
   uint8_t SPI_cs;

   //Valores padrões de configuração do MCP2515
   uint8_t crystalCLK = 8;
   uint16_t bitF = 125; // k Hertz
   uint8_t wMode = 0x00;

   uint8_t RXB0CTRL = 0x66;//whit RollOver to RXB1
   uint8_t RXB1CTRL = 0x60;
   uint8_t TXB0CTRL = 0x00;
   uint8_t TXB1CTRL = 0x00;
   uint8_t TXB2CTRL = 0x00;
   uint8_t CANINTE = 0xBF;
   uint8_t BFPCTRL = 0x0F;
   uint8_t TXRTSCTRL = 0x00;

   uint8_t CNF1 = 0x00;
   uint8_t CNF2 = 0x42;
   uint8_t CNF3 = 0x02;

   uint16_t MASstd[2] = {0x00, 0x00};//{0x400, 0x400};
   uint32_t MASext[2] = {0x00, 0x00};

   uint16_t FILstd[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};//0x7FF
   uint32_t FILext[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};//0x3FFFF

   //Error Loggers
   String errLog = "no error";
   String errMode = "Error Active";
   uint16_t RX0OVR = 0;
   uint16_t RX1OVR = 0;
   uint8_t multInt = 0;
   uint8_t  MERRF = 0;
   uint8_t  WAKIE = 0;
   uint8_t TEC = 0;
   uint8_t REC = 0;

   // frames
   CANframe frameRXB0;
   CANframe frameRXB1;

   //funções publicas
     MCP2515(uint8_t spi_cs, unsigned long int spi_speed = 10000000, uint8_t spi_wMode = 0); //

     void begin();

     void reset();

     void read(uint8_t reg, uint8_t *data, uint8_t n = 1);

     uint8_t regCheck(uint8_t reg, uint8_t val, uint8_t extraMask = 0xFF);

     void write(uint8_t reg, uint8_t val, uint8_t check = 0);

     void bitModify(uint8_t reg, uint8_t mask, uint8_t val, uint8_t check = 0);

     void confMode();

     void confRX();

     void confTX();

     void confINT();

     void confCAN();

     void confFM();

     void status(uint8_t *status);

     void RXstatus(uint8_t *status);

     void errCount();

     void writeID(uint16_t sid, uint8_t id_ef = 0, uint32_t eid = 0, uint8_t txb = 0, uint8_t timeOut = 10, uint8_t check = 0);

     void loadTX(uint8_t *data, uint8_t n = 8,  uint8_t abc = 1);

     void send(uint8_t txBuff = 0x01);

     void writeFrame(CANframe frameToSend, uint8_t txb = 0, uint8_t timeOut = 10, uint8_t check = 0);

     void abort(uint8_t abortCode = 7);

     void readID(uint8_t *id, uint8_t rxb = 0);

     void readFrame();
     void quickread();

     void digaOi(); //char S = "Ola"
     void digaOi(char *oi);
};


#endif
