/* Biblioteca para o CI MCP2515, datasheet disponivel em:
 * http://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-CAN-Controller-with-SPI-20001801J.pdf
 *
*/

//guarda de inclusao, evita duplo chamamento pelo usr final
#ifndef MCP2515_1_h
#define MCP2515_1_h

#include <Arduino.h>
#include <SPI.h>


class MCP2515{
     
private:
     //SPI constantes
     uint8_t CS;
     unsigned long int spi_max_speed;
     uint8_t spi_mode;
     
     //CAN
     uint8_t CLK2515; //em MHz, 8, 16 ou 20, 4 MHz nao ainda nao foi implementado
     uint16_t CANBDS; 
     
     //Constantes do nodo
     uint32_t ID;
/*
     uint8_t rxb0; //valore padrao do 0x60
     uint8_t rxb1; //valore padrao do 0x70

     uint8_t txb0; //valore padrao do 0x70	
     uint8_t txb1; //valore padrao do 0x70
     uint8_t txb2; //valore padrao do 0x70
     uint8_t intrp; //valore padrao do 0x2B
     uint8_t intrx;//valor padrao	
     uint8_t inttx;//valor padrao     
*/
     //SPI funcoes de inicializacao
     void start();
     void end();

 public:


     void digaOi(String S = "Ola");
     
     String erroLog = "Sem erro";
     
     MCP2515(uint32_t ID, uint8_t clk2515, uint8_t cs = 10, unsigned long int speed = 10000000, uint8_t mode = 0);
     
     void reset(); // instrucao reset 0xC0
     
     void read(uint8_t REG, uint8_t *data, uint8_t n = 1);
     
     uint8_t regCheck(uint8_t REG, uint8_t VAL, uint8_t extraMask = 0xFF);  
     
     void write(uint8_t REG, uint8_t VAL, uint8_t CHECK = 0);
     
     void bitModify(uint8_t REG, uint8_t MASK, uint8_t VAL, uint8_t CHECK = 0);

     void confRX(uint8_t RXB0 = 0x64, uint8_t RXB1 = 0x60);
     
     void confTX(uint8_t TXB0=0x00, uint8_t TXB1=0x00, uint8_t TXB2=0x00);

     void confINT(uint8_t INTRP=0xBF, uint8_t INTRX=0x0F, uint8_t INTTX=0x00);

     void conf(uint8_t OPMODE=0x00, uint16_t CANbds=500, uint8_t RESET=1);

     void status(uint8_t *status);

     void writeID(uint32_t ID, uint8_t TXBUFF = 3, uint8_t CHECK = 1);
     
     void loadTX(uint8_t *data, uint8_t n = 8,  uint8_t abc = 1);
     
     void writeDATA(uint8_t n, uint8_t *data, uint8_t TXB = 0,uint8_t CHECK = 1);
     
     void send(uint8_t TXbuff = 0x01);

     uint8_t checkDATA();

     void readID(uint8_t *ID, uint8_t RXB = 0);

     void readDATA(uint8_t *RXB0, uint8_t *RXB1);
     
};


/* Lista de Abreviaturas:
 * 
 * Register (registro)   REG
 * Chip Select           CS 
 * 
*/
#endif
