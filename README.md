# MCP2515 (em contrução)

 >John Welvins Barros de Araujo¹ <br /> 
 >Prof. Fabricio Ferrari¹ <br /> 
 >Prof. Edson M. Kakuno² <br /> 

¹ [IMEF](https://imef.furg.br/) - Universidade Federal do Rio Grande <br /> 
² [Física](http://cursos.unipampa.edu.br/cursos/licenciaturaemfisica/) - Unipampa Bagé <br /> 


A biblioteca MCP2515 foi feita para auxiliar/facilitar o controle do CI da Microchip, de mesmo nome, por plataformas Arduino, ou compatíveis. As funções de configuração possuem valores padrão, de modo a fornecer uma configuração simples e rápida. O CI MCP2515 configurado no modo padrão dessa biblioteca estará preparado para operar em uma rede de 125 k bit/s, sem a implementação de filtros de aceite nos buffers de entrada, com rollover do buffer de saída RXB0 para o RXB1, e além disso são habilitadas interrupções que indicam o sucesso de recebimento e envio. Para setar outra configuração o usuário deverá atualizar os valores das [variáveis de configuração](#varpubs_conf), e em seguida executar a função correspondente aos registros modificados, como a [confMode()](#confMode), [confRX()](#confRX), [confTX()](#confTX), [confINT()](#confINT), [confFM()](#confFM) e [confCAN()](#confCAN). Ou ainda as usando as funções de escrita básica, [write (REG, VAL, CHECK = 1)](#write) e [bitModify (REG, MASK, VAL, check = 0)](#bitModify).



Os frames podem ser escritos no barramento com a função [writeFrame(frameToSend, txb\_ = 0, timeOut = 10, check = 0)](#writeFrame) e podem ser lidos pela função [readFrame ()](#readFrame) que atualiza os valores dos frames internos [frameRXB0](#frameRXB0) e [frameRXB1](#frameRXB1).
As informações da mensagem são armazenadas na estrutura apresentada na figura [Frame](#frame), e podem ser acessados com as [variáveis do frame](#frames_var).
Também é possível obter os dados acessando diretamente os registros correspondentes aos buffers de entrada do MCP2515 com a função [read(REG, data, n = 1)](#read).
De forma análoga pode-se escrever nos buffers de saída do MCP2515 e solicitar o envio usando as funções de escrita básicas.


<div id='frame'/>
<p align="center">
<img src="FURGCAN_frame.png" width="700">
</p>



A principal referência desta biblioteca é o datasheet [MCP2515 Stand-Alone CAN Controller with SPI Interface](http://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-CAN-Controller-with-SPI-20001801J.pdf).
Por questões de comodidade muitos dos códigos numéricos relacionados a comunicação do MCP2515 com a plataforma Arduino foram expressos em hexadecimal indicado por 0x, e quando não estarão em decimal.



Um exemplo de conexão com um Arduino Nano e um módulo CAN, o MCP2515 e o transceptor [TJA1050](https://www.nxp.com/docs/en/data-sheet/TJA1050.pdf) da Phillips, pode ser visto na figura [Conexão](#MCPxNano).


<div id='MCPxNano'/> 
<p align="center">
<img src="example/CANMon/CANMon_cx.png" width="700">
</p>



O barramento é composto por pelo menos dois dispositivos, na figura [Bus](#bus) pode-se ver um diagrama simplificado de um barramento CAN.


<div id='bus'/>
<p align="center">
<img src="600_FURGCAN_barramento.png" width="700">
</p>


São fornecidos 6 exemplos de rotinas, dois para transmissão [CANTX.ino](https://github.com/KakiArduino/MCP2515/blob/version_1/example/CANTX/CANTX.ino) e [CANTXshort](https://github.com/KakiArduino/MCP2515/blob/version_1/example/CANTXshort/CANTXshort.ino), um para recepção [CANRX.ino](https://github.com/KakiArduino/MCP2515/blob/version_1/example/CANRX/CANRX.ino), dois exemplos de nodos sink (receptores), o [CANMon.ino](https://github.com/KakiArduino/MCP2515/tree/version_1/example/CANMon) envia para as mensagens para um computador pela porta USB-Serial, e o [CANSave.ino](https://github.com/KakiArduino/MCP2515/tree/version_1/example/CANSave) que salvas as mensagens recebidas em um cartão SD usando SPI via software.
Também é fornecido um template para nodos sensores, o exemplo [CANSensor.ino](https://github.com/KakiArduino/MCP2515/tree/version_1/example/CANSensor).


*******
Sumário:
 1. [Variáveis de um frame](#frames_var)   
 2. [Declaração de frames](#frames_fun)
 3. [Variaives públicas](#MCP_var)
 4. [Funções públicas](#MCP_fun)

*******

<div id='frames_var'/> 

## Variáveis de um frame

* `uint16_t id_std;`<br/>
 Variável de 2 bytes que armazena o ID padrão do frame.
 
* `uint32_t id_ext;`<br/>
 Variável de 4 bytes que armazena a extensão de ID do frame.

* `uint8_t dlc;`<br/>
Variável de 1 byte que armazena o código de comprimento, número de bytes de dados no frame

* `uint8_t data[8];`<br/>
Lista com os bytes} de dados do frame.
 
* `uint8_t bts[14];`<br/>
Lista com todos (no máximo 14) os bytes do frame.
 
* `String type = "Unknown";`<br/>
String com o tipo do frame, padrão ("Std. Data"), estendido ("Ext. Data") ou "No frame" que é usado para indicar que há novos frames nos buffers de entrada do MCP2515.

 
<div id='frames_fun'/>

## Declaração de frames

* `CANframe();`<br/>
Função para declaração de uma estrutura de variáveis do tipo CANframe sem argumentos.

Exemplo de uso:
```C++
CANframe frm();
frm.id_std = 0x7FF;
```
>  Declaração de um frame sem fornecer parâmetros de entrada, seguida, na linha de baixo, pela atribuição de 0x7FF para o ID padrão, o maior valor possível, do frame criado acima.

* `CANframe(uint8_t *frameBytes, uint8_t extFlag = 0);`<br/>
Função para criação de frame, a partir de uma lista com todos os bytes do frame.

Parâmentros de entrada:
  1. **frameBytes**: lista com todos os bytes do a serem atribuídos ao frame.
  2. **extFlag**: sinalizador de extensão de ID, se 0 o frame é padrão, se 1 é estendido.

Exemplo de uso:
```C++
frameBytes [10] = {0x07, 0xFF, 
                   0x03, 0xFF, 0xFF,
                   0x04,
                   0x3, 0xFF, 0xF7, 0x21};
            
CANframe frm(frameBytes, 1);
Serial.println(frm.id_std);
```
>  Primeiro é declarado a lista *frameBytes*, os dois primeiros bytes (0 e 1) compõem o ID padrão, concatenando tem-se 0x7FF o maior valor possível. 
>  Os três bytes seguintes da lista *frameBytes*, posições 2, 3 e 4, correspondem a extensão de ID, que neste caso vale o valor máximo 0x3FFFF.
>  O byte seguinte, posição 5, é o código dlc, que no caso indica quatro bytes de dados.
>  Após a declaração da lista com os bytes, é feito a declaração do frame estendido com a lista *frameBytes*.
>  Por fim o ID padrão é impresso através da variável *id_std* na última linha, isto é possível, pois ao declarar o frame, todos suas variáveis são atribuídas.


* `CANframe(uint16_t idstd, uint32_t idext, uint8_t dlc_, uint8_t *data);`<br/>
   Função para criação de frame estendido com declaração do ID padrão, extensão de ID, DLC - Data Len Code e uma lista de bytes de dados, **data**.

<div id='CANframe_ext'/>  

Parâmetros de entrada:
1.**idstd**: variável de 2 bytes onde deve ser informado o valor do ID padrão do frame, no máximo 0x7FF.
2.**idext**: variável de 4 bytes onde deve ser informado o valor da extensão de ID do frame, no máximo 0x3FFFF.
3.**dlc_**: é o número de bytes de dados.
4.**data**: é a lista que contem os bytes de dados dos frames.

Exemplo de uso:
```C++
uint8_t data[2] = {0, 10};
CANframe frm(1, 6, 2, data);
```
> Na primeira linha é declarada uma lista com os bytes de dados.
> Seguido, na linha abaixo, da declaração de um frame estendido com ID padrão igual a 1, extensão de ID igual a 6, e com dois bytes de dados (dlc_ = 2), sendo 0 e 10.


* `CANframe(uint16_t idstd, uint8_t dlc_, uint8_t *data);`<br/>
Função para criação de frames padrões, devem ser informados o ID padrão, o DLC, e de uma lista com os bytes dos dados.

<div id='CANframe_pad'/> 

Parâmetros de entrada:
1. **idstd**: variável de 2 bytes onde deve ser informado o valor do ID padrão do frame, no máximo 0x7FF.
2. **dlc\_**: número de \textit{bytes} de dados.
3. **data**:  lista contendo os bytes de dados dos frames.

Exemplo de uso:
```C++
uint8_t data[2] = {0, 10};
CANframe frm(1, 2, data);
```
> Na primeira linha é declarada uma lista com os bytes de dados.
> Seguido, na linha abaixo, da declaração de um frame padrão com ID padrão igual a 1, e com dois bytes de dados (dlc_ = 2), sendo 0 e 10.

* `reload(uint16_t idstd, uint32_t idext, uint8_t dlc_, uint8_t *data);`<br/>
Função para recarregar um frame qualquer com estendido, atribuindo os valores fornecidos com entrada nas variáveis correspondentes.
Os parâmetros de entrada são os mesmos, e em mesma ordem, da função [CANframe(idstd, idext, dlc_, data)](#CANframe_ext), descrita um pouco acima.

Exemplo de uso:
```C++
uint8_t data[2] = {0, 10};
CANframe frm(1, 2, data);
frm.reload(1, 10, 2, data);
```
> As duas primeiras linhas deste exemplo já foram apresentadas como exemplo para a função [CANframe(idstd, dlc_, data)](#CANframe_pad), descrito anteriormente, nele é criado um frame padrão.
> Na última linha o frame frm é recarregado como estendido com a função *reload(..)*, a única diferença é a inserção do valor da extensão de ID (10).


* `reload(uint16_t idstd, uint8_t dlc_, uint8_t *data);`<br/>
Função para recarregar um frame qualquer com padrão, atribuindo os valores fornecidos com entrada nas variáveis correspondentes.
Os parâmetros de entrada são os mesmos, e em mesma ordem, da função [CANframe(idstd, dlc_, data)](#CANframe_pad), descrita um pouco acima.

Exemplo de uso:
```C++
uint8_t data[2] = {0, 10};
CANframe frm(1, 10, 2, data);
frm.reload(1, 2, data);
```
> Na primeira linha é declarada uma lista com dois bytes de dados.
> Na segunda linha um frame estendido é declarado, tendo o ID padrão 1, a extensão de ID 10, o DLC 2, e os dados atribuídos a lista data na linha de cima.
> Na última linha o frame *frm* é recarregado com padrão, com a função *reload(..)*, a diferença é a ausência da extensão de ID.


* `reload(uint8_t dlc_, uint8_t *data_);`<br/>
Função para recarregar o campo de dados de frame qualquer, essa função não altera os valores de ID, também pouco o tipo de frame, também é alterado o DLC.
Essa função pode ser usada em um loop, onde os dados do frame são atualizados periodicamente, mas seus valores de ID não.

Parâmetros de entrada:
1. **dlc\_**: número de \textit{bytes} de dados.
2. **data_**:  lista contendo os bytes de dados dos frames.

Exemplo de uso:
```C++
uint8_t data[4] = {0, 10, 10, 0};
frm.reload(4, data);
```
> Na primeira linha é declarada uma lista com quatro bytes de dados.
> Na segunda linha um frame anteriormente criado (frm) é recarregado com a lista de bytes de dados data.

<div id='MCP_var'/>  

## Variaives públicas

<div id='MCP_var_SPI'/>  

### SPI

* `unsigned long int SPI_speed = 10000000;`<br/>
Máxima frequência do clock do SPI usado na comunicação entre o Arduino e o MCP2515, que é limitada pelo último em 10 M Hz. O valor praticado pelo Arduino é definido em função do seu clock interno e do limite fornecido em *SPI_speed*.
    
* `uint8_t SPI_wMode = 0;`<br/>
Modo de operação da comunicação SPI entre o Arduino e o MCP2515, o controlador CAN suporta dois modos o [0,0] (0) e o [1,1] (3).
    
 * `uint8_t SPI_cs;`<br/>
Número do pino do Arduino usado como *chip select* na comunicação SPI entre o Arduino e o MCP2515.

### Configurações gerais do MCP2515

<div id='MCP_var_conf'/>  

 * `uint8_t crystalCLK = 8;`<br/>
Frequência do oscilador que fornece a base de clock para o MCP2515, em mega hertz (M Hz).
    
 * `uint16_t bitF = 125;`<br/>
Taxa de bits do barramento, em k bit/s.
    
 * `uint8_t wMode = 0x00;`<br/>
Está variável é usada para manipular o registro 0x0F do controlador CAN, 0x00 é modo de operação padrão da biblioteca, modo de operação *Normal*, encerra a solicitação para cancelar o envio de todas as transmissões, modo *One-Shot* desabilitado, pino *CLKOUT* do MCP25 desabilitado e seta o *CLKOUT = System Clock/1*.
Para outras opções consultar o [datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-CAN-Controller-with-SPI-20001801J.pdf).
    
* `uint8_t RXB0CTRL = 0x66;`<br/>
Está variável é usada para manipular o registro 0x60 do MCP2515, ele configura o buffer de entrada *RXB0*.
O valor padrão 0x66, desabilita as mascaras e filtros e ativa o rollover do buffer de entrada *RXB0* para o *RXB1*.
    
* `uint8_t RXB1CTRL = 0x60;`<br/>
Está variável é usada para manipular o registro de configuração do buffer de saída *RXB1* do MCP2515 (0x70).
O valor padrão 0x60, desabilita as mascaras e filtros.
        
    
* `uint8_t TXB0CTRL = 0x00;`<br/>
Está variável manipula o registro 0x30, que corresponde a configuração do buffer de saída *TXB0* do MCP2515.
O valor padrão 0x00, atribui o nível mais baixo de prioridade (0) e aborta possível frame alocado buffer *TXB0*.
 > Obs.: Quando todos os buffers de saída possuem a mesma prioridade o, o TXB0 se torna o menos prioritário.
    
* `uint8_t TXB1CTRL = 0x00;`<br/>
Está variável manipula o registro 0x40, que corresponde a configuração do buffer de saída *TXB1* do MCP2515.
O valor padrão 0x00, atribui o nível mais baixo de prioridade (0) e aborta possível frame alocado buffer *TXB0*.
> Obs.: Quando todos os buffers de saída possuem a mesma prioridade, o *TXB1* se torna mais prioritário do que o *TXB0* e menos prioritário do que o *TXB2*.
        
* `uint8_t TXB2CTRL = 0x00;`<br/>
Está variável manipula o registro 0x50, que corresponde a configuração do buffer de saída *TXB2* do MCP2515.
O valor padrão 0x00, atribui o nível mais baixo de prioridade (0) e aborta possível frame alocado buffer *TXB0*.
> Obs.: Quando todos os buffers de saída possuem a mesma prioridade, o *TXB2* se torna o mais prioritário.
        
* `uint8_t CANINTE = 0xBF;`<br/>
Está variável manipula o registro 0x2B do MCP2515.
O valor padrão 0xBF no registro 0x2B habilita interrupções, quando uma mensagem é recebida no *RXB0* ou no *RXB1*, quando qualquer um dos buffers de saída  (*TXB0*, *TXB1* e *TXB2*) ficar vazio, quando ocorrer erros de múltiplas fontes indicadas no registro *EFLG* do MCP2515, e quando uma transmissão de mensagem for interrompida.
    
* `uint8_t BFPCTRL = 0x0F;`<br/>
Está variável manipula o registro 0x0C do MCP2515.
O valor padrão 0x0F no registro 0x0C configura os pinos do MCP2515, o *RX0BF* é habilitado e configurado como interrupção que indica quando uma nova mensagem valida é carregado no *RXB0*, e de forma análoga o *RX1BF* é configurado para o buffer *RXB1*. 
    
* `uint8_t TXRTSCTRL = 0x00;`<br/>
Está variável manipula o registro 0x0D do MCP2515.
O valor padrão \textit{0x00} não habilita nenhum interrupção com relação os pinos TX0RTS, TX1RTS e TX2RTS.        
        
* `uint8_t CNF1 = 0x00;`<br/>
Está variável salva o valor grava no registro 0x2A do MCP2515, ele faz parte da configuração do Bit-Timing.
Há algumas opções pré definidas, estas podem ser configuradas atribuindo valores as variáveis *crystalCLK* e *bitF*, e dessa forma as variáveis *CNF1*, *CNF2* e *CNF3* são atualizadas durante a inicialização feita pela função *begin()* ou pela função de configuração específica, *confCAN()*.

Os casos pré-definidos são: 
1. *crystalCLK* = 4: com duas possíveis taxas 125 k bit/s e 250 k bit/s.

2. *crystalCLK* = 8: com três possíveis taxas 125 k bit/s, 250 k bit/s e 500 k bit/s.
> As distancias estimas foram calculadas (não testadas) para o cristal de 8 M Hz, como sendo de até 275 m para 125 k bit/s, 125 m para 250 k bit/s e 50 m para 500 k bit/s.

3. *crystalCLK* = 20: com quatro possíveis taxas 125 k bit/s, 250 k bit/s, 500 k bit/s e 1000 k bit/s.

Para outros casos deve-se alterar *CNF1* diretamente, mais detalhes consultar o [datasheet](http://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-CAN-Controller-with-SPI-20001801J.pdf).
    
* `uint8_t CNF2 = 0x42;`<br/>
Está variável salva o valor gravado no registro 0x29 do MCP2515. 
Sua configuração segue de forma análoga a da varaiável *CNF1*.

* `uint8_t CNF3 = 0x02;`<br/>
        Está variável salva o valor grava no registro 0x28 do MCP2515.
Sua configuração segue de forma análoga a da varaiável *CNF1*.


### Filtros e Mascaras

<div id='MCP_var_filMask'/>  

* `uint16_t MASstd[2] = {0x00, 0x00};`<br/>
Lista com os valores dos ID padrões das mascaras 0 (*MASstd[0]*) e 1 (*MASstd[1]*) do MCP2515.
    
* `uint32_t MASext[2] = {0x00, 0x00};`<br/>
Lista com os valores das extensões de ID das mascaras 0 (*MASext[0]*) e 1 (*MASext[1]*) do MCP2515.
    
* `uint16_t FILstd[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};`<br/>
Lista com os valores de ID padrão dos filtros do MCP2515, *FILstd[0]* corresponde ao filtro 0, e assim por diate, até *FILstd[6]* que correnponde ao filtro 6.
    
* `uint32_t FILext[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};`<br/>
Lista com os valores das extensões de ID dos filtros do MCP2515, *FILext[0]* corresponde ao filtro 0, e assim por diate, até *FILext[6]* que correnponde ao filtro 6.


### Erros

<div id='MCP_var_erros'/>  

* `String errLog = "no error";`<br/>
Armazena o último erro genérico ocorrido. Pode-se atualizar o valor de *errLog* chamando a função *errCont()*.
    
* `String errMode = "Error Active";`<br/>
Armazena o modo de confinamento de erro,  "Error Active", "Error Passive" ou "Bus-Off". Pode-se atualizar o valor de *errMode* chamando a função *errCont()*.
    
* `uint16_t RX0OVR = 0;`<br/>
Armazena o número de *overload* no buffer *RXB0*. Pode-se atualizar o valor de *RX0OVR8 chamando a função *errCont()*.

* `uint16_t RX1OVR = 0;`<br/>
Armazena o número de \textit{overload} no buffer *RXB1*. Pode-se atualizar o valor de *RX1OVR* chamando a função *errCont()*.
        
* `uint8_t multInt = 0;`<br/>
Armazena o número de erros de multiplicas fontes detectados, indicados pelo registro *ERROR FLAG REGISTER* (0x2D).Pode-se atualizar o valor de \textit{multInt} chamando a função *errCont()*.
    
* `uint8_t  MERRF = 0;`<br/>
Armazena o número de erros de mensagens detectados. Pode-se atualizar o valor de *MERRF* chamando a função *errCont()*.
        
* `uint8_t  WAKIE;`<br/>
Armazena o valor da \textit{Wake-up flag} de MCP2515. Pode-se atualizar o valor de *WAKIE* chamando a função *errCont()*.
    
* `uint8_t TEC = 0;`<br/>
Armazena o valor do contador de erros de transmissão TEC. Pode-se atualizar o valor de *TEC* chamando a função *errCont()*.
    
* `uint8_t REC = 0;`<br/>
Armazena o valor do contador de erros de transmissão REC. Pode-se atualizar o valor de *REC* chamando a função *errCont()*.


### Frames

<div id='MCP_var_frm'/>  

* `CANframe frameRXB0;`<br/>
Variável do tipo *CANframe*, criada para receber os frames recebidos no buffer *RXB0*. Pode-se atualizar o valor de *frameRXB0* chamando a função *readFrame()*.
        
* `CANframe frameRXB1;`<br/>
Variável do tipo *CANframe*, criada para receber os frames recebidos no buffer *RXB1*. Pode-se atualizar o valor de *frameRXB1* chamando a função *readFrame()*.

## Funções públicas

<div id='MCP_fun'/>  

* `mcp.reset(uint8_t spi_cs, unsigned long int spi_speed = 10000000, uint8_t spi_wMode = 0);`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```


* `mcp.begin();`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```


* `mcp.reset();`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```


* `mcp.read(uint8_t REG, uint8_t *data, uint8_t n = 1);`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```

* `mcp.regCheck(uint8_t REG, uint8_t VAL, uint8_t extraMask = 0xFF);`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```

* `mcp.write(uint8_t REG, uint8_t VAL, uint8_t CHECK = 1);`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```



* `mcp.bitModify(uint8_t REG, uint8_t MASK, uint8_t VAL, uint8_t check = 0);`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```

* `mcp.confMode();`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```


* `mcp.confRX();`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```

* `mcp.confTX();`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```


* `mcp.confINT();`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```



* `mcp.confFM();`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```


* `mcp.confCAN();`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```


* `mcp.status(uint8_t *status);`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```


* `mcp.errCont();`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```


* `writeID(uint16_t sid, uint8_t id_ef = 0, uint32_t eid = 0, uint8_t txb = 0, uint8_t timeOut = 10, uint8_t check = 0);`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```


* `mcp.loadTX(uint8_t *data, uint8_t n = 8,  uint8_t abc = 1);`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```


* `mcp.send(uint8_t);`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```


* `writeFrame(CANframe frameToSend, uint8_t txb_ = 0, uint8_t timeOut = 10, uint8_t check = 0)`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```

* `mcp.abort(uint8_t abortCode = 7);`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```

* `mcp.readID(uint8_t *id, uint8_t rxb = 0);`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```

* `mcp.readFrame()`<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```



* `mcp.digaOi(char *oi);`<br/>

Parâmetros de entrada:
1. **oi**: é o char que informa quais variáveis serão impressas, se informado "status", será impresso somente as variáveis relacionadas ao estado do MCP2515 e seus buffers de saída e entrada. Se informado "eros" ou "errors", serão impressos valores de erros. Se informado "SPI", serão impressos os parâmetros do SPI. Se informado "CAN", serão impressos os parâmetros da configuração da CAN. Se informado "controle", serão impressos os parâmetros de controle do MCP2515. Se informado "filtros", serão impressos os filtros setados no MCP2515. Se informado "filtros", serão impressos as mascaras setados no MCP2515.

Exemplos de usos:

```C++
Serial.begin(9600);
mcp.digaOi();
```
> Neste exemplo a função *digaOI()* imprimirá todas as variáveis.

```C++
Serial.begin(9600);
mcp.digaOi("error");
```
> Exemplo de uso da função *digaOi("error")* imprimindo somente os parâmetros relacionados a erros.
