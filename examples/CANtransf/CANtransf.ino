
#include <MCP2515_1.h>

//criando um obejto MCP2515, (ID, 2515CLK, CS)
MCP2515 mcp(0xBB8, 8, 4);

/*              Siglas:
 * ID - identificador do despositivo
 * 2515CLK - hardware clock usado pelo chip MCP2515 em MHz 
 * CS - numero do pino digital usado como Chip Select SPI
 * TXB0 - Buffer de transmissao zero
 * TXB1 - Buffer de transmissao um
 * TXB2 - Buffer de transmissao dois
 * 
 */


void setup() {
  
}

void loop() {
  
  //Vetor com os bytes a serem enviados, max 8
  uint8_t TX0[8] ={0x36, 0x37, 0x38, 0x39,
                  0x3A, 0x3B, 0x3C, 0x3D};
  
  /* Escrevendo no TXB0, 
   * mcp.write(nbytes, vetorDados, TXbuff = 0)
  */
  mcp.writeDATA(8, TX0);
  
  /*  Enviando o TXB0
   *  mcp.send(code), code  = TXB2 TXB1 TXB0
   *  Envia somente TXB0:  code =  001 = 0x01
   *  Envia somente TXB1:  code =  010 = 0x02
   *  Envia TXB0 e TXB1 :  code =  011 = 0x03
   *  Envia somente TXB2:  code =  100 = 0x04
   *  Envia TXB0 e TXB2 :  code =  101 = 0x05
   *  Envia TXB1 e TXB2 :  code =  110 = 0x06
   *  Envia todos buffs :  code =  111 = 0x07
   */
  mcp.send(0x01);

  delay(1000);

}
