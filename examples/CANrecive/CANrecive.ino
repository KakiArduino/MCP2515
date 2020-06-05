#include <MCP2515_1.h>
/*               Siglas:
 * ID - identificador do despositivo
 * 2515CLK - hardware clock usado pelo chip MCP2515 em MHz 
 * CS - numero do pino digital usado como Chip Select SPI
 * RXB0 - Buffer de recebimento zero
 * RXB1 - Buffer de recebimento um
 * ID0 - idendificador da mensagem recebida no RXB0
 * ID1 - idendificador da mensagem recebida no RXB1
 */

/* Criando um obejto MCP2515, (ID, 2515CLK, CS), 
*  e realiza uma configuracao padrao 
*/
MCP2515 mcp(0xD2, 8, 4);

void setup() {
  Serial.begin(9600);
  
  //Verifica se houve erro durante a configuracao paadra do MCP2515
   Serial.println(mcp.erroLog);
}

void loop() {
 
  //Vetores para os 8 bytes recebidos, + um para o comprimento em bytes
  uint8_t BX0[13], BX1[13];   

  //Verifica se ha nova mensagem, caso tenha salva
  mcp.readDATA(BX0, BX1);

  //Se dado tiver sido recebido em TB0 imprime
  if (BX0[4] > 0){
    for(uint8_t i=0; i < 4; i++){
      Serial.print(BX0[i], HEX);
    }
    Serial.print(": ");
    for(uint8_t i=0; i < BX0[4]+1; i++){
      Serial.print(BX0[i+4], HEX);
      Serial.print(' ');
    }
    Serial.println();   
    }
  
  //Se dado tiver sido recebido em TB1 imprime
  if(BX1[4] > 0){
    for(uint8_t i=0; i < 4; i++){
      Serial.print(BX1[i], HEX);
    }
    Serial.print(": ");
    for( uint8_t i=0; i < BX1[4]+1; i++){
      Serial.print( BX1[i+4], HEX);
      Serial.print(' ');
    }
    Serial.println();
  }

}
