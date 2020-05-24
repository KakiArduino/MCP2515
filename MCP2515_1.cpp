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
	if ((0x36<=REG && REG <=0x3D) | (0x46<=REG && REG <=0x4D) | (0x56<=REG && REG<=0x5D) | ((REG & 0x0F)==0x0F)){
	
		if(data != VAL){
			erro = 1;
    	}
		return erro;
	}
	
	//Condicoes sem mascaras continuacao
	uint8_t FFvet[] = {0x31, 0x33, 0x34, 0x41, 0x43, 0x44, 0x51, 0x53, 0x54, 0x18, 0x14, 0x10, 0x08, 0x04, 0x00, 0x1B, 0x17, 0x13, 0x0B, 0x07, 0x03, 0x1A, 0x16, 0x12, 0x0A, 0x06, 0x02, 0x20, 0x24, 0x27, 0x23, 0x26, 0x22, 0x2C, 0x2B, 0x2A, 0x29};
	for (uint8_t i = 0; i< sizeof(FFvet)/sizeof(FFvet[0]); i++){
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
	checkMaskMap[2][0] = 0x35;  checkMaskMap[2][1] = 0x04;
	checkMaskMap[3][0] = 0x40;  checkMaskMap[3][1] = 0x0B;
	checkMaskMap[4][0] = 0x42;  checkMaskMap[4][1] = 0xEB;
	checkMaskMap[5][0] = 0x45;  checkMaskMap[5][1] = 0x04;
	checkMaskMap[6][0] = 0x50;  checkMaskMap[6][1] = 0x0B;
	checkMaskMap[7][0] = 0x52;  checkMaskMap[7][1] = 0xEB;
	checkMaskMap[8][0] = 0x55;  checkMaskMap[8][1] = 0x04;
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
	checkMaskMap[23][0] = 0x60;  checkMaskMap[23][1] = 0xC0;
		
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
	if (((REG & 0x0F)==0x0E) or (0x61<=REG<=0x6D) or (0x71<=REG<=0x7D) or (0x1C<=REG<=0x1D)){
		
	  erro = 2;
	  return erro;
	}

	return erro;
}

/* Opcional no lugar da de cima, mais lenta e ocupa mais bytes
uint8_t MCP2515::regCheck(uint8_t REG, uint8_t VAL, uint8_t data){
	
	/* mapa de mascaras para check de escritra, por map, mas num deu
	map<uint8_t, uint8_t> checkMasks;
	//CheckMasks Buff's saida										  Bit register			
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x30, 0x0B)); // 0 0 1 1  0 0 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x40, 0x0B)); // 0 1 0 0  0 0 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x50, 0x0B)); // 0 1 0 1  0 0 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x0D, 0x07)); // 0 0 0 0  1 1 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x31, 0xFF)); // 0 0 1 1  0 0 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x41, 0xFF)); // 0 1 0 0  0 0 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x51, 0xFF)); // 0 1 0 1  0 0 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x32, 0xEB)); // 0 0 1 1  0 0 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x42, 0xEB)); // 0 1 0 0  0 0 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x52, 0xEB)); // 0 1 0 1  0 0 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x33, 0xFF)); // 0 0 1 1  0 0 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x43, 0xFF)); // 0 1 0 0  0 0 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x53, 0xFF)); // 0 1 0 1  0 0 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x34, 0xFF)); // 0 0 1 1  0 1 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x44, 0xFF)); // 0 1 0 0  0 1 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x54, 0xFF)); // 0 1 0 1  0 1 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x35, 0x04)); // 0 0 1 1  0 1 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x45, 0x04)); // 0 1 0 0  0 1 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x55, 0x04)); // 0 1 0 1  0 1 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x36, 0xFF)); // 0 0 1 1  0 1 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x46, 0xFF)); // 0 1 0 0  0 1 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x56, 0xFF)); // 0 1 0 1  0 1 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x37, 0xFF)); // 0 0 1 1  0 1 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x47, 0xFF)); // 0 1 0 0  0 1 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x57, 0xFF)); // 0 1 0 1  0 1 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x38, 0xFF)); // 0 0 1 1  1 0 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x48, 0xFF)); // 0 1 0 0  1 0 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x58, 0xFF)); // 0 1 0 1  1 0 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x39, 0xFF)); // 0 0 1 1  1 0 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x49, 0xFF)); // 0 1 0 0  1 0 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x59, 0xFF)); // 0 1 0 1  1 0 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x3A, 0xFF)); // 0 0 1 1  1 0 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x4A, 0xFF)); // 0 1 0 0  1 0 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x5A, 0xFF)); // 0 1 0 1  1 0 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x3B, 0xFF)); // 0 0 1 1  1 0 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x4B, 0xFF)); // 0 1 0 0  1 0 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x5B, 0xFF)); // 0 1 0 1  1 0 1 1
 	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x3C, 0xFF)); // 0 0 1 1  1 1 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x4C, 0xFF)); // 0 1 0 0  1 1 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x5C, 0xFF)); // 0 1 0 1  1 1 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x3D, 0xFF)); // 0 0 1 1  1 1 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x4D, 0xFF)); // 0 1 0 0  1 1 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x5D, 0xFF)); // 0 1 0 1  1 1 0 1
	//checkMasks Buff's de entrada
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x60, 0x64)); // 0 1 1 0  0 0 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x70, 0x60)); // 0 1 1 1  0 0 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x0C, 0x3F)); // 1 1 0 0  0 0 0 0	 
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x61, 0x00)); // 0 1 1 0  0 0 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x71, 0x00)); // 0 1 1 1  0 0 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x62, 0x00)); // 0 1 1 0  0 0 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x72, 0x00)); // 0 1 1 1  0 0 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x63, 0x00)); // 0 1 1 0  0 0 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x73, 0x00)); // 0 1 1 1  0 0 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x64, 0x00)); // 0 1 1 0  0 1 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x74, 0x00)); // 0 1 1 1  0 1 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x65, 0x00)); // 0 1 1 0  0 1 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x75, 0x00)); // 0 1 1 1  0 1 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x66, 0x00)); // 0 1 1 0  0 1 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x76, 0x00)); // 0 1 1 1  0 1 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x67, 0x00)); // 0 1 1 0  0 1 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x77, 0x00)); // 0 1 1 1  0 1 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x68, 0x00)); // 0 1 1 0  1 0 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x78, 0x00)); // 0 1 1 1  1 0 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x69, 0x00)); // 0 1 1 0  1 0 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x79, 0x00)); // 0 1 1 1  1 0 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x6A, 0x00)); // 0 1 1 0  1 0 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x7A, 0x00)); // 0 1 1 1  1 0 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x6B, 0x00)); // 0 1 1 0  1 0 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x7B, 0x00)); // 0 1 1 1  1 0 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x6C, 0x00)); // 0 1 1 0  1 1 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x7C, 0x00)); // 0 1 1 1  1 1 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x6D, 0x00)); // 0 1 1 0  1 1 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x7D, 0x00)); // 0 1 1 1  1 1 0 1
	//checkMasks filtros
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x00, 0xFF)); // 0 0 0 0  0 0 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x04, 0xFF)); // 0 0 0 0  0 1 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x08, 0xFF)); // 0 0 0 0  1 0 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x10, 0xFF)); // 0 0 0 1  0 0 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x14, 0xFF)); // 0 0 0 1  0 1 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x18, 0xFF)); // 0 0 0 1  1 0 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x01, 0xEB)); // 0 0 0 0  0 0 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x05, 0xEB)); // 0 0 0 0  0 1 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x09, 0xEB)); // 0 0 0 0  1 0 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x11, 0xEB)); // 0 0 0 1  0 0 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x15, 0xEB)); // 0 0 0 1  0 1 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x19, 0xEB)); // 0 0 0 1  1 0 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x02, 0xFF)); // 0 0 0 0  0 0 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x06, 0xFF)); // 0 0 0 0  0 1 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x0A, 0xFF)); // 0 0 0 0  1 0 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x12, 0xFF)); // 0 0 0 1  0 0 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x16, 0xFF)); // 0 0 0 1  0 1 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x1A, 0xFF)); // 0 0 0 1  1 0 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x03, 0xFF)); // 0 0 0 0  0 0 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x07, 0xFF)); // 0 0 0 0  0 1 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x0B, 0xFF)); // 0 0 0 0  1 0 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x13, 0xFF)); // 0 0 0 1  0 0 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x17, 0xFF)); // 0 0 0 1  0 1 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x1B, 0xFF)); // 0 0 0 1  1 0 1 1
	//checkMasks mascaras
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x20, 0xFF)); // 0 0 1 0  0 0 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x24, 0xFF)); // 0 0 1 0  0 1 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x21, 0xE3)); // 0 0 1 0  0 0 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x25, 0xE3)); // 0 0 1 0  0 1 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x22, 0xFF)); // 0 0 1 0  0 0 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x26, 0xFF)); // 0 0 1 0  0 1 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x23, 0xFF)); // 0 0 1 0  0 0 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x27, 0xFF)); // 0 0 1 0  0 1 1 1
	//checkMasks configuracao
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x2A, 0xFF)); // 0 0 1 0  1 0 1 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x29, 0xFF)); // 0 0 1 0  1 0 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x28, 0xC7)); // 0 0 1 0  1 0 0 0
	//checkMasks logger de erro
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x1C, 0x00)); // 0 0 0 1  1 1 0 0
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x1D, 0x00)); // 0 0 0 1  1 1 0 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x2D, 0xC0)); // 0 0 1 0  1 1 0 1
	//checkMasks interrupcoes
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x2B, 0xFF)); // 0 0 1 0  1 0 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0x2C, 0xFF)); // 0 0 1 0  1 1 0 0
	//checkMasks controle
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0xFF, 0xFF)); // 1 1 1 1  1 1 1 1
	checkMasks.insert (std::pair<uint8_t, uint8_t>(0xEE, 0x00)); // 1 1 1 0  1 1 1 0
	*
	
  uint8_t checkMaskMap [145][2];
  
  //CheckMasks Buff's saida
  checkMaskMap[0][0] = 0x30;  checkMaskMap[0][1] = 0x0B;
  checkMaskMap[1][0] = 0x40;  checkMaskMap[1][1] = 0x0B;
  checkMaskMap[2][0] = 0x50;  checkMaskMap[2][1] = 0x0B;
  checkMaskMap[3][0] = 0x0D;  checkMaskMap[3][1] = 0x07;
  checkMaskMap[4][0] = 0x31;  checkMaskMap[4][1] = 0xFF;
  checkMaskMap[5][0] = 0x41;  checkMaskMap[5][1] = 0xFF;
  checkMaskMap[6][0] = 0x51;  checkMaskMap[6][1] = 0xFF;
  checkMaskMap[7][0] = 0x32;  checkMaskMap[7][1] = 0xEB;
  checkMaskMap[8][0] = 0x42;  checkMaskMap[8][1] = 0xEB;
  checkMaskMap[9][0] = 0x52;  checkMaskMap[9][1] = 0xEB;
  checkMaskMap[10][0] = 0x33;  checkMaskMap[10][1] = 0xFF;
  checkMaskMap[11][0] = 0x34;  checkMaskMap[11][1] = 0xFF;
  checkMaskMap[12][0] = 0x35;  checkMaskMap[12][1] = 0xFF;
  checkMaskMap[13][0] = 0x36;  checkMaskMap[13][1] = 0xFF;
  checkMaskMap[14][0] = 0x37;  checkMaskMap[14][1] = 0xFF;
  checkMaskMap[15][0] = 0x38;  checkMaskMap[15][1] = 0xFF;
  checkMaskMap[16][0] = 0x39;  checkMaskMap[16][1] = 0xFF;
  checkMaskMap[17][0] = 0x3A;  checkMaskMap[17][1] = 0xFF;
  checkMaskMap[18][0] = 0x3B;  checkMaskMap[18][1] = 0xFF;
  checkMaskMap[19][0] = 0x3C;  checkMaskMap[19][1] = 0xFF;
  checkMaskMap[20][0] = 0x3D;  checkMaskMap[20][1] = 0xFF;
  checkMaskMap[21][0] = 0x43;  checkMaskMap[21][1] = 0xFF;
  checkMaskMap[22][0] = 0x44;  checkMaskMap[22][1] = 0xFF;
  checkMaskMap[23][0] = 0x45;  checkMaskMap[23][1] = 0xFF;
  checkMaskMap[24][0] = 0x46;  checkMaskMap[24][1] = 0xFF;
  checkMaskMap[25][0] = 0x47;  checkMaskMap[25][1] = 0xFF;
  checkMaskMap[26][0] = 0x48;  checkMaskMap[26][1] = 0xFF;
  checkMaskMap[27][0] = 0x49;  checkMaskMap[27][1] = 0xFF;
  checkMaskMap[28][0] = 0x4A;  checkMaskMap[28][1] = 0xFF;
  checkMaskMap[29][0] = 0x4B;  checkMaskMap[29][1] = 0xFF;
  checkMaskMap[30][0] = 0x4C;  checkMaskMap[30][1] = 0xFF;
  checkMaskMap[31][0] = 0x4D;  checkMaskMap[31][1] = 0xFF;
  checkMaskMap[32][0] = 0x50;  checkMaskMap[32][1] = 0xFF;
  checkMaskMap[33][0] = 0x53;  checkMaskMap[33][1] = 0xFF;
  checkMaskMap[34][0] = 0x54;  checkMaskMap[34][1] = 0xFF;
  checkMaskMap[35][0] = 0x55;  checkMaskMap[35][1] = 0xFF;
  checkMaskMap[36][0] = 0x56;  checkMaskMap[36][1] = 0xFF;
  checkMaskMap[37][0] = 0x57;  checkMaskMap[37][1] = 0xFF;
  checkMaskMap[38][0] = 0x58;  checkMaskMap[38][1] = 0xFF;
  checkMaskMap[39][0] = 0x59;  checkMaskMap[39][1] = 0xFF;
  checkMaskMap[40][0] = 0x5A;  checkMaskMap[40][1] = 0xFF;
  checkMaskMap[41][0] = 0x5B;  checkMaskMap[41][1] = 0xFF;
  checkMaskMap[42][0] = 0x5C;  checkMaskMap[42][1] = 0xFF;
  checkMaskMap[43][0] = 0x5D;  checkMaskMap[43][1] = 0xFF;

  //checkMasks Buff's de entrada
  checkMaskMap[44][0] = 0x60;  checkMaskMap[44][1] = 0x64;
  checkMaskMap[45][0] = 0x70;  checkMaskMap[45][1] = 0x60;
  checkMaskMap[46][0] = 0x0C;  checkMaskMap[46][1] = 0x3F;
  checkMaskMap[47][0] = 0x61;  checkMaskMap[47][1] = 0x00;
  checkMaskMap[48][0] = 0x62;  checkMaskMap[48][1] = 0x00;
  checkMaskMap[49][0] = 0x63;  checkMaskMap[49][1] = 0x00;
  checkMaskMap[50][0] = 0x64;  checkMaskMap[50][1] = 0x00;
  checkMaskMap[51][0] = 0x65;  checkMaskMap[51][1] = 0x00;
  checkMaskMap[52][0] = 0x66;  checkMaskMap[52][1] = 0x00;
  checkMaskMap[53][0] = 0x67;  checkMaskMap[53][1] = 0x00;
  checkMaskMap[54][0] = 0x68;  checkMaskMap[54][1] = 0x00;
  checkMaskMap[55][0] = 0x69;  checkMaskMap[55][1] = 0x00;
  checkMaskMap[56][0] = 0x6A;  checkMaskMap[56][1] = 0x00;
  checkMaskMap[57][0] = 0x6B;  checkMaskMap[57][1] = 0x00;
  checkMaskMap[58][0] = 0x6C;  checkMaskMap[58][1] = 0x00;
  checkMaskMap[59][0] = 0x6D;  checkMaskMap[59][1] = 0x00;
  checkMaskMap[60][0] = 0x71;  checkMaskMap[60][1] = 0x00;
  checkMaskMap[61][0] = 0x72;  checkMaskMap[61][1] = 0x00;
  checkMaskMap[62][0] = 0x73;  checkMaskMap[62][1] = 0x00;
  checkMaskMap[63][0] = 0x74;  checkMaskMap[63][1] = 0x00;
  checkMaskMap[64][0] = 0x75;  checkMaskMap[64][1] = 0x00;
  checkMaskMap[65][0] = 0x76;  checkMaskMap[65][1] = 0x00;
  checkMaskMap[66][0] = 0x77;  checkMaskMap[66][1] = 0x00;
  checkMaskMap[67][0] = 0x78;  checkMaskMap[67][1] = 0x00;
  checkMaskMap[68][0] = 0x79;  checkMaskMap[68][1] = 0x00;
  checkMaskMap[69][0] = 0x7A;  checkMaskMap[69][1] = 0x00;
  checkMaskMap[70][0] = 0x7B;  checkMaskMap[70][1] = 0x00;
  checkMaskMap[71][0] = 0x7C;  checkMaskMap[71][1] = 0x00;
  checkMaskMap[72][0] = 0x7D;  checkMaskMap[72][1] = 0x00;

  //checkMasks filtros
  checkMaskMap[73][0] = 0x00;  checkMaskMap[73][1] = 0xFF;
  checkMaskMap[74][0] = 0x04;  checkMaskMap[74][1] = 0xFF;
  checkMaskMap[75][0] = 0x08;  checkMaskMap[75][1] = 0xFF;
  checkMaskMap[76][0] = 0x10;  checkMaskMap[76][1] = 0xFF;
  checkMaskMap[77][0] = 0x14;  checkMaskMap[77][1] = 0xFF;
  checkMaskMap[78][0] = 0x18;  checkMaskMap[78][1] = 0xFF;
  checkMaskMap[79][0] = 0x01;  checkMaskMap[79][1] = 0xEB;
  checkMaskMap[80][0] = 0x05;  checkMaskMap[80][1] = 0xEB;
  checkMaskMap[81][0] = 0x09;  checkMaskMap[81][1] = 0xEB;
  checkMaskMap[82][0] = 0x11;  checkMaskMap[82][1] = 0xEB;
  checkMaskMap[83][0] = 0x15;  checkMaskMap[83][1] = 0xEB;
  checkMaskMap[84][0] = 0x19;  checkMaskMap[84][1] = 0xEB;
  checkMaskMap[85][0] = 0x02;  checkMaskMap[85][1] = 0xFF;
  checkMaskMap[86][0] = 0x06;  checkMaskMap[86][1] = 0xFF;
  checkMaskMap[87][0] = 0x0A;  checkMaskMap[87][1] = 0xFF;
  checkMaskMap[88][0] = 0x12;  checkMaskMap[88][1] = 0xFF;
  checkMaskMap[89][0] = 0x16;  checkMaskMap[89][1] = 0xFF;
  checkMaskMap[90][0] = 0x1A;  checkMaskMap[90][1] = 0xFF;
  checkMaskMap[91][0] = 0x03;  checkMaskMap[91][1] = 0xFF;
  checkMaskMap[92][0] = 0x07;  checkMaskMap[92][1] = 0xFF;
  checkMaskMap[93][0] = 0x0B;  checkMaskMap[93][1] = 0xFF;
  checkMaskMap[94][0] = 0x13;  checkMaskMap[94][1] = 0xFF;
  checkMaskMap[95][0] = 0x17;  checkMaskMap[95][1] = 0xFF;
  checkMaskMap[96][0] = 0x1B;  checkMaskMap[96][1] = 0xFF;

  //checkMasks mascaras
  checkMaskMap[97][0] = 0x20;  checkMaskMap[97][1] = 0xFF;
  checkMaskMap[98][0] = 0x24;  checkMaskMap[98][1] = 0xFF;
  checkMaskMap[99][0] = 0x21;  checkMaskMap[99][1] = 0xE3;
  checkMaskMap[100][0] = 0x25;  checkMaskMap[100][1] = 0xE3;
  checkMaskMap[101][0] = 0x22;  checkMaskMap[101][1] = 0xFF;
  checkMaskMap[102][0] = 0x26;  checkMaskMap[102][1] = 0xFF;
  checkMaskMap[103][0] = 0x23;  checkMaskMap[103][1] = 0xFF;
  checkMaskMap[104][0] = 0x27;  checkMaskMap[104][1] = 0xFF;

  //checkMasks configuracao
  checkMaskMap[105][0] = 0x2A;  checkMaskMap[105][1] = 0xFF;
  checkMaskMap[106][0] = 0x29;  checkMaskMap[106][1] = 0xFF;
  checkMaskMap[107][0] = 0x28;  checkMaskMap[107][1] = 0xC7;

  //checkMasks logger de erro
  checkMaskMap[108][0] = 0x1C;  checkMaskMap[108][1] = 0x00;
  checkMaskMap[109][0] = 0x1D;  checkMaskMap[109][1] = 0x00;
  checkMaskMap[110][0] = 0x2D;  checkMaskMap[110][1] = 0xC0;
  
  //checkMasks interrupcoes
  checkMaskMap[111][0] = 0x2B;  checkMaskMap[111][1] = 0xFF;
  checkMaskMap[112][0] = 0x2C;  checkMaskMap[112][1] = 0xFF;

  //checkMasks controle
  checkMaskMap[113][0] = 0x0F;  checkMaskMap[113][1] = 0xFF;
  checkMaskMap[114][0] = 0x1F;  checkMaskMap[114][1] = 0xFF;
  checkMaskMap[115][0] = 0x2F;  checkMaskMap[115][1] = 0xFF;
  checkMaskMap[116][0] = 0x3F;  checkMaskMap[116][1] = 0xFF;
  checkMaskMap[117][0] = 0x4F;  checkMaskMap[117][1] = 0xFF;
  checkMaskMap[118][0] = 0x5F;  checkMaskMap[118][1] = 0xFF;
  checkMaskMap[119][0] = 0x6F;  checkMaskMap[119][1] = 0xFF;
  checkMaskMap[120][0] = 0x7F;  checkMaskMap[120][1] = 0xFF;
  checkMaskMap[121][0] = 0x8F;  checkMaskMap[121][1] = 0xFF;
  checkMaskMap[122][0] = 0x9F;  checkMaskMap[122][1] = 0xFF;
  checkMaskMap[123][0] = 0xAF;  checkMaskMap[123][1] = 0xFF;
  checkMaskMap[124][0] = 0xBF;  checkMaskMap[124][1] = 0xFF;
  checkMaskMap[125][0] = 0xCF;  checkMaskMap[125][1] = 0xFF;
  checkMaskMap[126][0] = 0xDF;  checkMaskMap[126][1] = 0xFF;
  checkMaskMap[127][0] = 0xEF;  checkMaskMap[127][1] = 0xFF;
  checkMaskMap[128][0] = 0xFF;  checkMaskMap[128][1] = 0xFF;
  checkMaskMap[129][0] = 0x0E;  checkMaskMap[129][1] = 0x00;
  checkMaskMap[130][0] = 0x1E;  checkMaskMap[130][1] = 0x00;
  checkMaskMap[131][0] = 0x2E;  checkMaskMap[131][1] = 0x00;
  checkMaskMap[132][0] = 0x3E;  checkMaskMap[132][1] = 0x00;
  checkMaskMap[133][0] = 0x4E;  checkMaskMap[133][1] = 0x00;
  checkMaskMap[134][0] = 0x5E;  checkMaskMap[134][1] = 0x00;
  checkMaskMap[135][0] = 0x6E;  checkMaskMap[135][1] = 0x00;
  checkMaskMap[136][0] = 0x7E;  checkMaskMap[136][1] = 0x00;
  checkMaskMap[137][0] = 0x8E;  checkMaskMap[137][1] = 0x00;
  checkMaskMap[138][0] = 0x9E;  checkMaskMap[138][1] = 0x00;
  checkMaskMap[139][0] = 0xAE;  checkMaskMap[139][1] = 0x00;
  checkMaskMap[140][0] = 0xBE;  checkMaskMap[140][1] = 0x00;
  checkMaskMap[141][0] = 0xCE;  checkMaskMap[141][1] = 0x00;
  checkMaskMap[142][0] = 0xDE;  checkMaskMap[142][1] = 0x00;
  checkMaskMap[143][0] = 0xEE;  checkMaskMap[143][1] = 0x00;
  checkMaskMap[144][0] = 0xFE;  checkMaskMap[144][1] = 0x00;
	
	uint8_t erro = 0;
	
	volatile uint8_t *data;
    read(REG, data);
	
	for (uint8_t i = 0; i <sizeof(checkMaskMap)/sizeof(checkMaskMap[0]); i++){
		if (checkMaskMap[i][0] == REG){
			if(checkMaskMap[i][1] == 0x00){
				erro = 2;
				return erro;
			}
			
			if((*data & checkMaskMap[i][1]) != (VAL & checkMaskMap[i][1])){
				erro = 1;
				return erro;
			}
			return erro;
		}
	}
	//return erro;
}*/

//Funcao de escrita em registros
void MCP2515::write(uint8_t REG, uint8_t VAL, uint8_t CHECK = 0){
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
  //}
  
  //if (rxb1 != RXB1){
    //rxb1 = RXB1;  
    write(0x70, RXB1, 1);
    if(erroLog == "Erro na escrita"){
      return;
    }
  //}
  
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
/*	
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
*/	
      default:
			erroLog = "Clock inva";
    };
  //}
	
	
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

//Escrita
void MCP2515::writeID(uint32_t ID, uint8_t TXBUFF = 3, uint8_t CHECK = 1){
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
  
    if(ID >= 536870912){
	erroLog = "ID invalido, tente um ID < 536870912";
	return;
    }
	
    if(CHECK == 1){
    if (ID > 2047){ //Extendido
    uint8_t msSID, lsSID, lsEID, msEID;
    lsEID = ID & 0xFF;
    msEID = ID >> 8 & 0XFF;
    lsSID = (((ID >> 13) & 0xE0) + 0x8) + ((ID >> 16) & 0x3);
    msSID = ID >> 21 & 0xFF;
	
    switch(TXBUFF){
      case 0:
	write(0x34, lsEID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x33, msEID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x32, lsSID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x31, msSID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	break;
	
	case 1:
	write(0x44, lsEID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x43, msEID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x42, lsSID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x41, msSID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	break;	
	
	case 2:
	write(0x54, lsEID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x53, msEID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x52, lsSID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x51, msSID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	break;	
	
	default:
	write(0x34, lsEID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x33, msEID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x32, lsSID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x31, msSID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}		
	write(0x44, lsEID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x43, msEID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x42, lsSID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x41, msSID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}	
	write(0x54, lsEID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x53, msEID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x52, lsSID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
	write(0x51, msSID, 1);
	if(erroLog == "Erro na escrita"){
	  return;
	}
    };		
	
      }else{//Padrao
	uint8_t msSID = ID >> 3, lsSID = (ID & 7)*0x20;  
		
	switch (TXBUFF){
	  case 0:
	    write(0x32, lsSID, 1);
	    if(erroLog == "Erro na escrita"){
	      return;
	    }
	    write(0x31, msSID, 1);
	    if(erroLog == "Erro na escrita"){
	      return;
	    }	
	    break;
	  
	  case 1:
	    write(0x42, lsSID, 0);
	    if(erroLog == "Erro na escrita"){
	      return;
	    }
	    write(0x41, msSID, 1);
	    if(erroLog == "Erro na escrita"){
	      return;
	    }			
	    break;
	
	  case 2:
	    write(0x52, lsSID, 1);
	    if(erroLog == "Erro na escrita"){
	      return;
	    }
	    write(0x51, msSID, 1);
	    if(erroLog == "Erro na escrita"){
	      return;
	    }		
	    break;	
		
	  default:
	    write(0x32, lsSID, 1);
	    if(erroLog == "Erro na escrita"){
	      return;
	    }
	    write(0x31, msSID, 1);
	    if(erroLog == "Erro na escrita"){
	      return;
	    }			
	    
	    write(0x42, lsSID, 1);
	    if(erroLog == "Erro na escrita"){
	      return;
	    }
	    write(0x41, msSID, 1);
	    if(erroLog == "Erro na escrita"){
	      return;
	    }
		
	    write(0x52, lsSID, 1);
	    if(erroLog == "Erro na escrita"){
	      return;
	    }
	    write(0x51, msSID, 1);
	    if(erroLog == "Erro na escrita"){
	      return;
	    }	  
	};	

      }	
  
    }else{
      if (ID > 2047){ //Extendido
	uint8_t msSID, lsSID, lsEID, msEID;
	lsEID = ID & 0xFF;
	msEID = ID >> 8 & 0XFF;
	lsSID = (((ID >> 13) & 0xE0) + 0x8) + ((ID >> 16) & 0x3);
	msSID = ID >> 21 & 0xFF;
	
	switch(TXBUFF){
	  case 0:
	    write(0x34, lsEID);
	    write(0x33, msEID);
	    write(0x32, lsSID);
	    write(0x31, msSID);
	    break;
			
	  case 1:
	    write(0x44, lsEID);
	    write(0x43, msEID);
	    write(0x42, lsSID);
	    write(0x41, msSID);
	    break;	
	
	  case 2:
	    write(0x54, lsEID);
	    write(0x53, msEID);
	    write(0x52, lsSID);
	    write(0x51, msSID);
	    break;	
	
	  default:
	    write(0x34, lsEID);
	    write(0x33, msEID);
	  
	    write(0x32, lsSID);
	    write(0x31, msSID);
	
	    write(0x44, lsEID);
	    write(0x43, msEID);
	    write(0x42, lsSID);
	    write(0x41, msSID);
	
	    write(0x54, lsEID);
	    write(0x53, msEID);
	    write(0x52, lsSID);
	    write(0x51, msSID);
	};		
	
      }else{//Padrao
	uint8_t msSID = ID >> 3, lsSID = (ID & 7)*0x20;  
	
	switch (TXBUFF){
	  case 0:
	    write(0x32, lsSID);
	    write(0x31, msSID);
	    break;
	  
	  case 1:
	    write(0x42, lsSID);
	    write(0x41, msSID);
	    break;
	
	  case 2:
	    write(0x52, lsSID);
	    write(0x51, msSID);		
	    break;	
		
	  default:
	    write(0x32, lsSID);
	    write(0x31, msSID);
		
	    write(0x42, lsSID);
	    write(0x41, msSID);

	    write(0x52, lsSID);
	    write(0x51, msSID);  
	};	

      }		
	
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


void MCP2515::writeDATA(uint8_t n, uint8_t *data, uint8_t TXB = 0,uint8_t CHECK = 1){
  
  uint8_t abc;
  
  if( n > 8){
    erroLog == "n bytes invalido";
    return;
  } 
  
  switch (TXB){
    case 0:
      TXB = 0x35;
      abc  = 1;
      break;
    
    case 1:
      TXB = 0x45;
      abc  = 3;
      break;
      
    case 2: 
      TXB = 0x55;
      abc  = 5;
      break;
    
    default:
      erroLog = "TXB invalido";
      return;
  };

  
  write(TXB, n, CHECK);
  if(erroLog == "Erro na escrita"){
    return;
  }
  
  loadTX(data, n, abc);
  
  return;
}


void MCP2515::send(uint8_t TXbuff = 0x01){
  start();
  SPI.transfer(0x80 + TXbuff);
  end();
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

void MCP2515::readID(uint32_t *IDIN, uint8_t RXB = 0){

  uint8_t lsb, msb;
  uint8_t SID[2];
  uint32_t idin = 0;
  
  switch(RXB){
    case 0: //RXB0     
      read(0x61, SID, 2); //SID[0] = SIDH e SID[1] = SIDL
      
      if (bitRead(SID[1], 3) == 0){ //ID Padrão
      
	lsb = byte(0xEB) & byte(SID[1]);
	idin = (word(byte(SID[0]), byte(lsb))) >> 5;
  
      }else{//ID Estendido

	uint32_t MSB, nbits;          
	MSB = (byte(SID[0]) << 5) + (byte(byte(byte(224) & byte(SID[1])) >> 3) + byte(byte(3) & byte(SID[1])));
    
	uint8_t EID[2];
	read(0x63, EID, 2); //EID[0] = EIDH e EID[1] = EIDL
    
	nbits = EID[0];
	nbits = (nbits << 8) + EID[1];
	MSB = (MSB << 16) + nbits;
	idin = MSB; 
      } 
	break;

    case 1: //RXB1      
      read(0x71, SID, 2); //SID[0] = SIDH e SID[1] = SIDL

      if (bitRead(SID[1], 3) == 0){ //ID Padrão
	lsb = byte(224) & byte(SID[1]);
	idin = (word (byte(SID[0]),byte(lsb)) >> 5);
  
      }else{//ID Estendido

	uint32_t MSB, nbits;          
	MSB = (byte(SID[0]) << 5) + (byte(byte(byte(224) & byte(SID[1])) >> 3) + byte(byte(3) & byte(SID[1])));
    
	uint8_t EID[2];
	read(0x73, EID, 2); //EID[0] = EIDH e EID[1] = EIDL
    
	nbits = EID[0];
	nbits = (nbits << 8) + EID[1];
	MSB = (MSB << 16) + nbits;
	idin = MSB;    
      } 
      break;
    
    default:

      erroLog  = "RXB invalido";


  };
  
  IDIN[0] = idin;
  
  return;

}

void MCP2515::readDATA(uint32_t *IDIN0, uint8_t *DATAIN0, uint32_t *IDIN1, uint8_t *DATAIN1){

  uint8_t dIN0[8] = {0, 0, 0, 0, 0, 0, 0, 0}, 
	  dIN1[8] = {0, 0, 0, 0, 0, 0, 0, 0}, 
	  nbytes0[1]= {0}, nbytes1[1]= {0}; 
  uint32_t idIN0[1] = {0}, idIN1[1] = {0};     
  
  uint8_t check = checkDATA();
  
  switch(check){
    case 1: //Dado em RXB0
      
      read(0x65, nbytes0);
      nbytes0[0] = nbytes0[0] & 0xF;
      readID(idIN0, 0);
      read(0x66, dIN0, nbytes0[0]);
      
      write(0x2C, 0x00, 1);
      break;
      
    case 2: //Dado em  RXB1
  
      read(0x75, nbytes1);
      nbytes1[0] = nbytes1[0] & 0xF;
      readID(idIN1, 1);
      read(0x76, dIN1, nbytes1[0]);
      
      write(0x2C, 0x00, 1);      
      break;
      
    case 3: //Dado em  RXB0 e RXB1
    
      read(0x65, nbytes0);
      nbytes0[0] = nbytes0[0] & 0xF;
      readID(idIN0, 0);
      read(0x66, dIN0, nbytes0[0]);
        
      read(0x75, nbytes1);
      nbytes1[0] = nbytes1[0] & 0xF;
      readID(idIN1, 1);
      read(0x76, dIN1, nbytes1[0]);
      
      write(0x2C, 0x00, 1);      
      break;   
      
    default:
      break;
  };
  
  IDIN0[0] = idIN0[0];
  DATAIN0[0] = nbytes0[0];
  for(uint8_t i = 0; i <  nbytes0[0]; i++){
    DATAIN0[i+1] = dIN0[i];
  }
  
  IDIN1[0] = idIN1[0];
  DATAIN1[0] = nbytes1[0];
  for(uint8_t i = 0; i< nbytes1[0]; i++){
    DATAIN1[i+1] = dIN1[i];
  }
  
  return;
  
}















