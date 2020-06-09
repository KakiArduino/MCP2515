
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
  Serial.begin(9600);
  Serial.println(mcp.erroLog);
  Serial.println('\n');  
  
}

void loop() {
  
  //Vetor com os bytes a serem enviados, max 8
  uint8_t TX0[8] ={0x36, 0x37, 0x38, 0x39,
                  0x3A, 0x3B, 0x3C, 0x3D};
  
  /* Escrevendo no TXB0, 
   * mcp.write(nbytes, vetorDados, TXbuff = 0)
  */
  mcp.writeDATA(8, TX0);
  
  //check, o seucesso do envio pelo TXB0(0x30)
  //0x04: verifica se o bit 3 do REG 0x30 é 1
  uint8_t n= 0;
  do{
  
    if(n > 10) {
      Serial.println("Erro de checagem de envio");
      break;
    }
    delay(10);
       n = n + 1;
  }while(mcp.regCheck(0x30, 0x04, 0x04)!=0);
  
  Serial.println("Enviado!");

  delay(1000);

}
