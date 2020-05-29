/* Biblioteca para o CI MCP2515, datasheet disponivel em:
 * ref[1] = 
 * http://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-
   CAN-Controller-with-SPI-20001801J.pdf
 *
*/

#include "MCP2515_1.h"

/* Função para testes!
 *
*/
void MCP2515::digaOi(String S = "Ola"){
    
   Serial.print("Chip Select: ");
   Serial.println(CS);

   Serial.print("Velocidade maxima do SPI: ");
   Serial.println(spi_max_speed);

   Serial.print("Modo do SPI: ");
   Serial.println(spi_mode);
   
   Serial.print("Clock do 2515: ");
   Serial.print(CLK2515);
   Serial.println(" MHz\n");
   
}


//Construtor
MCP2515::MCP2515(uint32_t ID, uint8_t clk2515, uint8_t cs = 10, unsigned long int speed = 10000000, uint8_t mode = 0){
/* Descricao do construtor...   
 * Construtor, MCP2515::MCP2515, é a função define valores
 * de operação da classe MCP2515, os valores padrao foram re-
 * tirados de [1]. Ainda nessa funcao é configurado o pino do
 * Chip Select como OUTPUT, e seu estado e alterado para HIGH
 * garantido que o MCP2515 não seja selecionado por padrão.
 */
  if(speed <= 10000000){
  spi_max_speed = speed;
  }else{
    erroLog = "Velocidade do SPI invalida, tente =< 10000000";
    return;
  }
  
  if(mode == 0 | mode == 3){
   spi_mode = mode;  
  }else{
    erroLog = "Modo SPI invalid0, tente 0 ou 3";
    return;
  }
  
  if(clk2515 == 8 | clk2515 == 16 | clk2515 == 20) {//ainda nao funciona para 4 MHz
	CLK2515 = clk2515;
  }else{
    erroLog = "Clock invalido, tente 8, 16 ou 20";
    return;
  }
  
  CS = cs;
  pinMode(CS, OUTPUT);
  digitalWrite(CS, HIGH);
  SPI.begin();
  
  /*
  rxb0 = 0x64; //valore padrao do 0x60
  rxb1 = 0x60; //valore padrao do 0x70
  
  txb0 = ; //valore padrao do 0x70	
  txb1 = ; //valore padrao do 0x70
  txb2 = ; //valore padrao do 0x70
  intrp = ; //valore padrao do 0x2B
  intrx = ;//valor padrao	
  inttx = ;//valor padrao 
  */
  //Conf padrao do MCP2515
  conf();
  
  //Salva o ID
  writeID(ID);
  
}


//Funcao inicializacao do SPI
void MCP2515::start(){
/* Descricao da funcao start()...
 * Configura o barramento SPI para
*/
  SPI.beginTransaction(SPISettings(spi_max_speed, MSBFIRST, spi_mode));
  digitalWrite(CS, LOW);
}


//Funcao de finalizacao do SPI
void MCP2515::end() {
 /* Descricao da funcao end()...
  * Desoculpa o barramento SPI
  */
    digitalWrite(CS, HIGH);
    SPI.endTransaction();
}


//Funcao Reset
void MCP2515::reset(){
  /* Descricao da funcao...
	 * Reseta o CI MCP2515 pelo envio da instrucao.
	 * Atencao ao voltar do reset o CI se encontra
	 * em modo de configuracao e com valores padrao
	 * nos registros. E aconselhavel dar um delay 
	 * antes do uso do CI, no minimo 2us. Essa funcao
	 * e util para setar os valores padrao nos registros
	 * e por tanto se aconselha usala logo apos a 
	 * inicializacao do CI.
	 * * * * * * * * * * * * * * * *  * * * * * * *  * * */
    start();
    SPI.transfer(0xC0); 
    end();
    delayMicroseconds(10);
}


//Funcao de leitura de registros 
void MCP2515::read(uint8_t REG, uint8_t *data, uint8_t n = 1){
  /* Descricao da funcao read(REG, data, n = 1)...
   * Parametros de entrada: REG e o endereco
   * do registro, *data e o endereco de me-
   * moria a ser alocado pelos valores lidos,
   * n e o numero de registros a serem lidos,
   * n pode ser omitido, em leitura unitaria.

   * Pode-se realizar a leitura de um registro
   * fornecendo o endereço por um ponteiro,
   * ou o endereço por um veotr unitario, data[1].
   * Nao pode-se ultilizar no lugar de *data, 
   * uma variavel, pois espara-se um endereço de 
   * memoria.
   * Dica use volatile uint8_t *data;

   * A leitura de varios (n) registros em se-
   * guencia, pode ser feita fornendo o endereço
   * do primeiro registro, um vetor com n posicoes, 
   * data[n], e informando o valor n.
   * 0x03 e o codigo da instrucao de leitura
   * * * * * * * * * * * * * * * * * * * * * * */
  
  /* Inserir função de erro que identifica a entrada
   * do endereço, data[1] ou *data.
   * * * * * * * * * * * * * * * * * * * * * * */
    for(uint8_t i = 0; i < n; i++){
      data[i] = 0;
    }
    start();
    SPI.transfer(0x03); 
    SPI.transfer(REG);
    for(uint8_t i = 0; i < n; i++){
      data[i] = SPI.transfer(0x00);
    }
    end();
    return;

}


//Funcao de verificacao de valores salvos em registros
uint8_t MCP2515::regCheck(uint8_t REG, uint8_t VAL, uint8_t extraMask = 0xFF){
	/* Descricao da funcao regCheck(REG, VAL)...
	 * Essa funcao verifica se valor salvo no registro (REG),
	 * e igual ao byte VAL. Se sim retorna zero, caso contrario	 
	 * retorna 1 indicando que o valor salvo no registro e 
	 * diferente de VAL, ou ainda retorna 2, indicando que nao
	 * se pode escrever em REG. 
	 * Caso queira verifica um registro apos a escrita, basta
	 * sinalizar na propria funcao de escrita. Essa funcao ocupa
	 * 378, pois ela possui valores de mascaras para realizar as
	 * comparacoes. +++ extraMask
	 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	uint8_t erro = 0;
	
	volatile uint8_t *Data;
	read(REG, Data);
	
	uint8_t data = *Data & extraMask;
	VAL = VAL & extraMask;
	
	//pode tentar por switch
	//Condicoes para mascaras 0XFF, i. e., sem mascaras
	if ((0x36<=REG && REG <=0x3D) | (0x46<=REG && REG <=0x4D) | 
	    (0x56<=REG && REG<=0x5D) | ((REG & 0x0F)==0x0F)){
	
		if(data != VAL){
			erro = 1;
    	}
		return erro;
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
	  if(REG == FFvet[i]){
	    if(data != VAL){
		erro = 1;
	    }
	    return erro;
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
	checkMaskMap[21][0] = 0x60;  checkMaskMap[21][1] = 0x00;
	checkMaskMap[22][0] = 0x60;  checkMaskMap[22][1] = 0x00;
	checkMaskMap[23][0] = 0x2D;  checkMaskMap[23][1] = 0xC0;
		
	//configuracao
	checkMaskMap[24][0] = 0x28;  checkMaskMap[24][1] = 0xC7;
		
	for(uint8_t i = 0; i< sizeof(checkMaskMap)/sizeof(checkMaskMap[0]); i++){
	  if(checkMaskMap[i][0] == REG){
	    if((data & checkMaskMap[i][1]) != (VAL & checkMaskMap[i][1])){
	
	      erro = 1;
	    }
				
	    return erro;
	  }
	}

	//Condicoes de registro invalido para escrita
	if (((REG & 0x0F)==0x0E) | (0x61<=REG<=0x6D) | 
	     (0x71<=REG<=0x7D) | (0x1C<=REG<=0x1D)){
		
	  erro = 2;
	  return erro;
	}

	return erro;
}


//Funcao de escrita em registros
void MCP2515::write(uint8_t REG, uint8_t VAL, uint8_t CHECK = 1){
  /* Descricao da funcao write (REG, VAL, CHECK=0)...
   * tem como argumento, o endereço do 
   * registro (REG) e o valor (VAL), com
   * tamanho de um byte a ser escrito. 
   * 0x02 é o código da instrução de
   * escrita. A funcao pode opcionalmente
   * verificar a escrita, para isso deve-se
   * fazer CHECK = 1, e nesse caso a funcao write
   * chama a funcao regCheck, que verifica 
   * o valor salvo em REG, e caso ele nao seja
   * igual a VAL, retorna 1, que indica erro de 
   * escrita, esse erro é entao salvo no loggger
   * de erro. regCheck ainda acusa quando REG nao 
   * é um registro valido para escrita, retornando 2,
   * Reg invalido, que e salvo no logger de erro.
   * Por hora o padrao e nao verificar, CHECK = 0. 
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    start();
    SPI.transfer(0x02);
    SPI.transfer(REG);
    SPI.transfer(VAL);
    end();
    
    if(CHECK == 1){
    
      uint8_t ERRO =0;
      ERRO = regCheck(REG, VAL);
 
      if( ERRO == 1){
       erroLog = "Erro na escrita";
	   return;
      }
    
      if( ERRO == 2){
        erroLog = "Reg invalido";
		return;
      }  
	  
	  return;
    }
	return;
}


//Funcao para modificacao de bits em registros
void MCP2515::bitModify(uint8_t REG, uint8_t MASK, uint8_t VAL, uint8_t CHECK = 0){
 /* Descricao da funcao...
  * 	Essa funcao modifica apenas os bits desejados em um registro
  * espeficado por REG, preservando o valor de seus vizinhos. 
  * O coidigo interno (ao MCP2515) dessa intrucao e 0000 0101, 0x05. 
  * 
  * Argumentos, REG : Endereco (byte) do registro
  * 			MASK: Byte para filtrar os bits desejados
  * 			VAL : Byte que contem os valores dos bits a serem escritos
  * 
  * Exemplo, setar modo de configuracao.
  * 	Os bits associados aos modos de operacao, sao localizados nas
  * tres posicoes mais significantes (7:5), do registro CANCTRL, 0xnF,
  * n pode ser qualquer valor hexa de 0 ate F. Para setar modo de configuracao
  * deve-se escrever 1 0 0, nas posicoes 7 6 5, o que equivale a ao valor 0x80,
  * 1000 0000. 
  *
  * 	bitModify(0x0F, 0xE0, 0x80);
  * 
  * Caso queira realizar uma verificacao desse procedimento, pode fazer
  *
  * 	bitModify(0x0F, 0xE0, 0x80, 1);
  *
  *	nesse caso a funcao verifica se o sucesso e se houver falha  ela 
  * atualiza o erroLog, que pode ser observado de fora por obj_nome.erroLog.
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
   
	start();
	SPI.transfer(0x05);
	SPI.transfer(REG);
	SPI.transfer(MASK);
    SPI.transfer(VAL);   
    end();
	
    if(CHECK == 1){
    
      uint8_t ERRO =0;
	  //VAL = VAL;
	  
	  //Serial.println(VAL, HEX);
	  
      ERRO = regCheck(REG, VAL, MASK);
 
      if( ERRO == 1){
       erroLog = "Erro na escrita";
	   return;
      }
    
      if( ERRO == 2){
        erroLog = "Reg invalido";
		return;
      }  
	  
	  return;
    }
	return;
}


//configuracoes
void MCP2515::confRX(uint8_t RXB0 = 0x64, uint8_t RXB1 = 0x60){

    //if (rxb0 != RXB0){
    //rxb0 = RXB0;
    write(0x60, RXB0, 1);// Verificação falahndo!	RXB0
    if(erroLog == "Erro na escrita"){
      return;
    }
    
  
    //if (rxb1 != RXB1){
    //rxb1 = RXB1;  
    write(0x70, RXB1, 1);
    if(erroLog == "Erro na escrita"){
      return;
    }
    
  
}


void MCP2515::confTX(uint8_t TXB0=0x00, uint8_t TXB1=0x00, uint8_t TXB2=0x00){


    write(0x30, TXB0, 1);// Verificação falahndo!	RXB0
    if(erroLog == "Erro na escrita"){
    return;
    }

    write(0x40, TXB1, 1);
    if(erroLog == "Erro na escrita"){
      return;
    }


     write(0x50, TXB2, 1);
    if(erroLog == "Erro na escrita"){
      return;
    }
 

}


void MCP2515::confINT(uint8_t INTRP=0xBF, uint8_t INTRX=0x0F, uint8_t INTTX=0x00){
  
    write(0x2B, INTRP, 1);				
    if(erroLog == "Erro na escrita"){
      return;
    }
  
    write(0x0C, INTRX, 1);				
    if(erroLog == "Erro na escrita"){
      return;
    }


    write(0x0D, INTTX, 1);// Verificação falahndo!	
    if(erroLog == "Erro na escrita"){
      return;
    }	

}


void MCP2515::conf(uint8_t OPMODE=0x00, uint16_t CANbds=500, uint8_t RESET=1){
   /* Descricao da funcao...
	* 	
	* 	1) A funcao verifica a opcao de reset, se RESET = 1 realiza
	* um reset, pela funcao reset(), o que deve restaurara os valores
	* de todos os registros e ainda setar o modo configuracao. 
	* Se RESET = 0, passa para o procedimento seguinte.
	*
	* 	2) Verifica se o MCP2515 esta no modo configucaracao, caso o 
	* ela entra em um loop de X cilcos, no qual tenta setar o modo de 
	* configucarao pela funcao bitModify(0x0F, 0xE0, 0x80) verificando 
	* o sucesso do procedimento a cada vez para poder sair do loop. 
	* Se isso não acontece a funcao atualiza o erroLog para Conf e1 e
	* retorna.
	*
	*		3) Configura a velocidade do barramento CAN, para CANbds.
	* Se houver algum erro durante esse procedimento salva Conf e2
	* em erroLog. 
	*		
	*		4) Configura os pinos de interrupcoes fisicas, 0x2B, 0x0C e 0x0D,
	* por padrao 0x2B valera 0xBF, 0x0C valera 0x0F e 0x0D valera 0x00. 
	* Assim teremos interrupcoes físicas no pino fisico INT (12)  quando houver
	* erros ou quando ums dos buffs de transmissao envaziar indicando sucesso 
	* de transmissao. Alem disso ficam abilitadas as interrupcoes nos pinos 
	* fisicos RX0BF (11) e RX1BF (10) indicando quando houver messagem valida
	* nos respectivos resgistros associados aos buffers de entrada.
	*
	* Argumentos, 	RXB0 : Byte escrito no registro 0x60 pg.27
	*				RXB1 : Byte escrito no registro 0x70 p.28
	*				INT : valor a ser escrito em 0x2B, registro que controla a interrupcoes pag. 53
	*				intRX : valor a ser escrito em 0x0C, registro das interrupcoes do RX pg. 29
	*				intTX : valor a ser escrito em 0x0D, registro das interrupcoes do TX pg. 19
	*				CAN_speed : 5, 10, 20, 31, 33, 40, 80, 100, 125, 200, 250, 500, 1000 em kbds
	*				RESET : 1 ou 0, indica se sera realizado o reset ou nao
	*
	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  
  //Reset opcional
  if (RESET == 1){
    reset();
  }
  
  //Verifica e coloca em modo de configuracao
  if(regCheck(0x0F, 0x80, 0xE0) != 0){
    uint8_t crt = 0;
    while((regCheck(0x0F, 0x80, 0xE0) != 0) && (crt < 100) ){
      
      bitModify(0x0F, 0xE0, 0x80);
      crt = crt + 1;
      delayMicroseconds(10);
    }

    if((crt == 100) && (regCheck(0x0F, 0x80, 0xE0) != 0)){
      erroLog = "Conf e1";	
    }	
  }

  //Velocidade do barramento	
  //if (CANbds != CANBDS){
    CANBDS = CANbds;
    switch(CLK2515){
      case 8 :
		switch(CANBDS){
	    case 1000 :
	      write(0x2A, 0x00, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x29, 0x80, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x28, 0x80, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      break;
	    
	    case 500 :
	      write(0x2A, 0x00, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x29, 0x90, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x28, 0x82, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
    
	      break;					
	    
	    case 250 :
	      write(0x2A, 0x00, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x29, 0xB1, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x28, 0x85, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
		}
	      break;
	    
	    case 200 :
	      write(0x2A, 0x00, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x29, 0xB4, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x28, 0x86, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      break;					
	    
	    case 125 :
	      write(0x2A, 0x01, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x29, 0xB1, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x28, 0x85, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      break;					
	    
	    case 100 :
	      write(0x2A, 0x01, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x29, 0xB4, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x28, 0x86, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      break;					
	    
	    case 80 :
	      write(0x2A, 0x01, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x29, 0xBF, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x28, 0x87, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      break;					
	    
	    case 50 :
	      write(0x2A, 0x03, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x29, 0xB4, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x28, 0x86, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      break;					
	    
	    case 40 :
	      write(0x2A, 0x03, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x29, 0xBF, 1);				
	      if(erroLog == "Erro na escrita"){
	      return;
	      }
	      write(0x28, 0x87, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      break;					
	    
	    case 33 :
	      write(0x2A, 0x47, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x29, 0xE2, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x28, 0x85, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      break;					
	 
	    case 31 :
	      write(0x2A, 0x07, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x29, 0xA4, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x28, 0x84, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      break;					
	  
	    case 20 :
	      write(0x2A, 0x07, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x29, 0xBF, 1);				
	      if(erroLog == "Erro na escrita"){
	      return;
	      }
	      write(0x28, 0x87, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      break;					
	  
	    case 10 :
	      write(0x2A, 0x0F, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x29, 0xBF, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x28, 0x87, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      break;					
	    
	    case 5 :
	      write(0x2A, 0x1F, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x29, 0xBF, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      write(0x28, 0x87, 1);				
	      if(erroLog == "Erro na escrita"){
		return;
	      }
	      break;					
	      
	    default:
	      erroLog = "Conf e2";
	  };
		break;

      case 16 :
		switch(CANbds){
			
				case 1000 :
					write(0x2A, 0x00, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xD0, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x82, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 500 :
					write(0x2A, 0x00, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xF0, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x86, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 250 :
					write(0x2A, 0x41, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xF1, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x85, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 200 :
					write(0x2A, 0x01, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFA, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 125 :
					write(0x2A, 0x03, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xF0, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x86, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;
				case 100 :
					write(0x2A, 0x03, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFA, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 83 :
					write(0x2A, 0x03, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xBE, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x07, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 80 :
					write(0x2A, 0x03, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFF, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 50 :
					write(0x2A, 0x07, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFA, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 40 :
					write(0x2A, 0x07, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFF, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 33 :
					write(0x2A, 0x4E, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xF1, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x85, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 20 :
					write(0x2A, 0x0F, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFF, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 10 :
					write(0x2A, 0x1F, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFF, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 5 :
					write(0x2A, 0x3F, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFF, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				default:
				erroLog = "Conf e2";			
			};
		break;
	
      case 20 :
		switch(CANbds){
			
				case 1000 :
					write(0x2A, 0x00, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xD9, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x82, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 500 :
					write(0x2A, 0x00, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFA, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 250 :
					write(0x2A, 0x41, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFB, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x86, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 200 :
					write(0x2A, 0x01, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFF, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 125 :
					write(0x2A, 0x03, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFA, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;
				case 100 :
					write(0x2A, 0x04, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFA, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 83 :
					write(0x2A, 0x04, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFE, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 80 :
					write(0x2A, 0x04, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFF, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 50 :
					write(0x2A, 0x09, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFA, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 40 :
					write(0x2A, 0x09, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFF, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					break;					
				case 33 :
					write(0x2A, 0x0B, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x29, 0xFF, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}
					write(0x28, 0x87, 1);				
					if(erroLog == "Erro na escrita"){
						return;
					}				
					break;
				default:
				erroLog = "Conf e2";
			};
		break;

      default:
		erroLog = "Clock inva";
    };
  
	
  //setando o modo de operacao OPMODE
  write(0x0F, OPMODE, 1);
  if(erroLog == "Erro na escrita"){
    return;
  }
	
  //Configfura as interrupcoes
  confINT();
	
  //Configura o recebimento
  confRX();
	
  //Configura a transmissao
  confTX();
  
  return;
}


void MCP2515::status(uint8_t *status){
  start();
  SPI.transfer(0xA0);
  status[0] = SPI.transfer(0x00);
  end();
}


//Contador de erros do barramento CAN
void MCP2515::errorCont(uint8_t *cont){
  
  static uint8_t crt = 0;
  
  read(0x2D, cont);
  cont[3] = cont[0];  
  read(0x1C, cont, 2);
  
  
  if(cont[0] == 255){
     if((crt & 0xF) == 0xF){
		crt = crt - 0xF;
     }
     crt = crt + 0x1;
  }

  if(cont[1] == 255){

     crt = crt + 0x10;
  }
  
  if(bitRead(cont[3], 6) == 1){

    bitModify(0x2D, 0x40, 0xFF, 1);
    if(erroLog != String("Sem erro")){
        return;
    }
  }

  if(bitRead(cont[3], 6) == 1){
    
    bitModify(0x2D, 0x80, 0x80, 1);
    if(erroLog != String("Sem erro")){
        return;
    }
  }
  
  cont[2] = crt;
  
  return;  
}



//Escrita
void MCP2515::writeID(uint32_t ID, uint8_t TXBUFF = 3, uint8_t timeOut = 10, uint8_t CHECK = 1){
	/* Descricao da funcao...
	*
	* 	1)  
	*
	* 	2) 
	*
	*	3) 
	*
	* Argumentos, 	ID : 
	*				TXBUFF : 
	*				CHECK :
	*
	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	uint8_t TXCRT;
	long t = millis();
	
	
    if(ID >= 536870912){
		erroLog = "ID invalido, tente um ID < 536870912";
		return;
    }
	
	
    if (ID > 2047){ //Extendido
		uint8_t msSID, lsSID, lsEID, msEID;
		lsEID = ID & 0xFF;
		msEID = ID >> 8 & 0XFF;
		lsSID = (((ID >> 13) & 0xE0) + 0x8) + ((ID >> 16) & 0x3);
		msSID = ID >> 21 & 0xFF;
	
		switch(TXBUFF){
			case 0:
				while(millis() - t < timeOut){
					read(0x30, TXCRT);
					if(bitRead(TXCRT, 3)==0 ){
						write(0x34, lsEID, CHECK);
						write(0x33, msEID, CHECK);
						write(0x32, lsSID, CHECK);
						write(0x31, msSID, CHECK);
						if(erroLog == "Erro na escrita"){
							return;
						}				
						return;
					}
				}
	
			case 1:
				while(millis() - t < timeOut){
					read(0x40, TXCRT);
					if(bitRead(TXCRT, 3)==0 ){
						write(0x44, lsEID, CHECK);
						write(0x43, msEID, CHECK);
						write(0x42, lsSID, CHECK);
						write(0x41, msSID, CHECK);
						if(erroLog == "Erro na escrita"){
							return;
						}				
						return;
					}
				}
	
			case 2:
				while(millis() - t < timeOut){
					read(0x50, TXCRT);
					if(bitRead(TXCRT, 3)==0 ){
						write(0x54, lsEID, CHECK);
						write(0x53, msEID, CHECK);
						write(0x52, lsSID, CHECK);
						write(0x51, msSID, CHECK);
						if(erroLog == "Erro na escrita"){
							return;
						}				
						return;
					}
				}			
	
			default:
				while(millis() - t < timeOut){
					read(0x30, TXCRT);
					if(bitRead(TXCRT, 3)==0){
						write(0x34, lsEID, CHECK);
						write(0x33, msEID, CHECK);
						write(0x32, lsSID, CHECK);
						write(0x31, msSID, CHECK);
						if(erroLog == "Erro na escrita"){
							return;
						}
						break;
					}	
				}
				while(millis() - t < timeOut){
					read(0x30, TXCRT);
					if(bitRead(TXCRT, 3)==0){
						write(0x44, lsEID, CHECK);
						write(0x43, msEID, CHECK);
						write(0x42, lsSID, CHECK);
						write(0x41, msSID, CHECK);
						if(erroLog == "Erro na escrita"){
							return;
						}
						break;
					}	
				}
				while(millis() - t < timeOut){
					read(0x50, TXCRT);
					if(bitRead(TXCRT, 3)==0){
						write(0x54, lsEID, CHECK);
						write(0x53, msEID, CHECK);
						write(0x52, lsSID, CHECK);
						write(0x51, msSID, CHECK);
						if(erroLog == "Erro na escrita"){
							return;
						}
						return;
					}	
				}				
		};		
    }


	if (ID < 2048){ //Padrao
		uint8_t msSID = ID >> 3, lsSID = (ID & 7)*0x20;  
		
		switch (TXBUFF){
			case 0:
				write(0x32, lsSID, CHECK);
				if(erroLog == "Erro na escrita"){
					return;
				}
				write(0x31, msSID, CHECK);
				if(erroLog == "Erro na escrita"){
					return;
				}	
				return;	
	  
			case 1:
				write(0x42, lsSID, CHECK);
				if(erroLog == "Erro na escrita"){
					return;
				}
				write(0x41, msSID, CHECK);
				if(erroLog == "Erro na escrita"){
					return;
				}			
				return;	
	
			case 2:
				write(0x52, lsSID, CHECK);
				if(erroLog == "Erro na escrita"){
					return;
				}
				write(0x51, msSID, CHECK);
				if(erroLog == "Erro na escrita"){
					return;
				}		
				return;	
		
			default:
				write(0x32, lsSID, CHECK);
				if(erroLog == "Erro na escrita"){
					return;
				}
				write(0x31, msSID, CHECK);
				if(erroLog == "Erro na escrita"){
					return;
				}			
	    
				write(0x42, lsSID, CHECK);
				if(erroLog == "Erro na escrita"){
					return;
				}
				write(0x41, msSID, CHECK);
				if(erroLog == "Erro na escrita"){
					return;
				}
		
				write(0x52, lsSID, CHECK);
				if(erroLog == "Erro na escrita"){
					return;
				}
				write(0x51, msSID, CHECK);
				if(erroLog == "Erro na escrita"){
					return;
				}
				return;
		};	
	}	
 
	
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
    erroLog = "abc invalido";
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


void MCP2515::send(uint8_t TXbuff = 0x01){
  start();
  SPI.transfer(0x80 + TXbuff);
  end();
}


void MCP2515::writeDATA(uint8_t n, uint8_t *data, uint8_t TXB = 0, uint8_t timeOut = 10, uint8_t CHECK = 1){
  
  uint8_t TXCRT[1] = {0xFF};
  long t = millis();
  
  erroLog = "Sem erro";
  
  if( n > 8){
    erroLog == "n bytes invalido";
    return;
  } 
  
	switch (TXB){
		case 0:
		  while(millis() - t < timeOut){
			read(0x30, TXCRT);
			if(bitRead(TXCRT[0], 3) == 0 ){
				write(0x35, n, CHECK);
				if(erroLog == "Erro na escrita"){
					return;
				}
				loadTX(data, n, 1);
				send(0x01);
				return;
			}
		  }
		  break;
    
		case 1:
		  while(millis() - t < timeOut){
			read(0x40, TXCRT);
			if(bitRead(TXCRT[0], 3)==0 ){
				write(0x45, n, CHECK);
				if(erroLog == "Erro na escrita"){
					return;
				}
				loadTX(data, n, 3);
				send(0x02);
				return;
			}
		  }
		  break;
		case 2:    
		  
		  while(millis() - t < timeOut){
			read(0x50, TXCRT);
			if(bitRead(TXCRT[0], 3)==0 ){
				write(0x55, n, CHECK);
				if(erroLog == "Erro na escrita"){
					return;
				}
				loadTX(data, n, 5);
				send(0x04);
				return;
			}
		  }
		  break;
		default:
			erroLog = "TXB invalido";
			return;
	};
	
	erroLog == "Buff cheio";
	return;
}



//Leitura
uint8_t MCP2515::checkDATA(){
  
  uint8_t checkDATA[1];
  read(0x2C, checkDATA);
  
  if(bitRead(checkDATA[0], 0) == 1){
    
    return 1; 
    
    if (bitRead(checkDATA[0], 1) == 1){
      return 3;
    }
  }else
    if (bitRead(checkDATA[0], 1) == 1){
      return 2;
    }else{
      
      return 0;
    }

  if (bitRead(checkDATA[0], 5) == 1){
    write(0x2C, 0x00, 1);
    erroLog = "Message invalida";
    return 4;
  }
  
  if (bitRead(checkDATA[0], 7) == 1){
    write(0x2C, 0x00, 1);
    erroLog = "Overflow erro,";
    return 5;
  }
  
  return;
}



void MCP2515::readID(uint8_t *ID, uint8_t RXB = 0){


  ID[0] = 0;
  ID[1] = 0;
  ID[2] = 0;
  ID[3] = 0;
  
 
  
  switch(RXB){
    case 0: //RXB0     
      read(0x61, ID, 4); //SID[0] = SIDH e SID[1] = SIDL
      
      if (bitRead(ID[1], 3) == 0){ //ID Padrão
			ID[2] = ID[0];
			ID[3] = ID[1];
			ID[0] = 0;
			ID[1] = 0;
	
			ID[3] = (ID[2] << 3) + (ID[3] >> 5);
			ID[2] = ID[2] >> 5;
  
      }else{//ID Estendido
	
			ID[1] = (((ID[1] & 0xE3) >> 3) | (ID[1] & 0x03)) | (ID[0] << 5) ;
			ID[0] = ID[0] >> 3;
	
      } 
		break;

    case 1: //RXB1      
      read(0x71, ID, 4); //SID[0] = SIDH e SID[1] = SIDL
      
      if (bitRead(ID[1], 3) == 0){ //ID Padrão
			ID[2] = ID[0];
			ID[3] = ID[1];
			ID[0] = 0;
			ID[1] = 0;
	
			ID[3] = (ID[2] << 3) + (ID[3] >> 5);
			ID[2] = ID[2] >> 5;
  
      }else{//ID Estendido
	
			ID[1] = (((ID[1] & 0xE3) >> 3) | (ID[1] & 0x03)) | (ID[0] << 5) ;
			ID[0] = ID[0] >> 3;
	
      } 
		break;
    
    default:

      erroLog  = "RXB invalido";
      return;

  };
 
  
  return;

}


void MCP2515::readDATA(uint8_t *RXB0, uint8_t *RXB1){

  for(uint8_t i=0; i < 13; i++){
    RXB0[i] = 0; 
    RXB1[i] = 0;
  }

  switch(checkDATA()){
    case 1: //Dado em RXB0
      
      read(0x65, RXB0);
      
      RXB0[8] = RXB0[0] & 0xF;
      
      if(RXB0[8] > 8){
		RXB0[8] = 8;
      }
      
      read(0x66, RXB0, RXB0[8]);
      
      RXB0[12] = RXB0[7];
      RXB0[11] = RXB0[6];
      RXB0[10] = RXB0[5];
      RXB0[9] = RXB0[4];
      RXB0[4] = RXB0[8];
      RXB0[8] = RXB0[3];
      RXB0[7] = RXB0[2];
      RXB0[6] = RXB0[1];
      RXB0[5] = RXB0[0];  
      
      readID(RXB0, 0);

      write(0x2C, 0x00, 1);      
 
      return;
      
    case 2: //Dado em  RXB1
  
      read(0x75, RXB1);
      
      RXB1[8] = RXB1[0] & 0xF;
      
      if(RXB1[8] > 8){
		RXB1[8] = 8;
      }
      
      read(0x76, RXB1, RXB1[8]);
      
      RXB1[12] = RXB1[7];
      RXB1[11] = RXB1[6];
      RXB1[10] = RXB1[5];
      RXB1[9] = RXB1[4];
      RXB1[4] = RXB1[8];
      RXB1[8] = RXB1[3];
      RXB1[7] = RXB1[2];
      RXB1[6] = RXB1[1];
      RXB1[5] = RXB1[0];  
      
      readID(RXB1, 0);

      write(0x2C, 0x00, 1); 
      
      return;
      
    case 3: //Dado em  RXB0 e RXB1
    
      read(0x65, RXB0);
      read(0x75, RXB1);
      
      RXB0[8] = RXB0[0] & 0xF;
      RXB1[8] = RXB1[0] & 0xF;
      
      if(RXB0[8] > 8){
		RXB0[8] = 8;
      }
      if(RXB1[8] > 8){
		RXB1[8] = 8;
      }
      
      read(0x66, RXB0, RXB0[8]);
      read(0x76, RXB1, RXB1[8]);
      
      RXB0[12] = RXB0[7];
      RXB0[11] = RXB0[6];
      RXB0[10] = RXB0[5];
      RXB0[9] = RXB0[4];
      RXB0[4] = RXB0[8];
      RXB0[8] = RXB0[3];
      RXB0[7] = RXB0[2];
      RXB0[6] = RXB0[1];
      RXB0[5] = RXB0[0];  
      
      RXB1[12] = RXB1[7];
      RXB1[11] = RXB1[6];
      RXB1[10] = RXB1[5];
      RXB1[9] = RXB1[4];
      RXB1[4] = RXB1[8];
      RXB1[8] = RXB1[3];
      RXB1[7] = RXB1[2];
      RXB1[6] = RXB1[1];
      RXB1[5] = RXB1[0];  
      
      readID(RXB0, 0);
      readID(RXB1, 0);

      write(0x2C, 0x00, 1);       
      return;   
      
    default:
      return;
  };
  
}














