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
 3. [Variaives publicas](#MCP_var)
 4. [Funções publicas](#MCP_fun)

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

## Variaives publicas

<div id='MCP_var_SPI'/>  

### SPI

<div id='MCP_var_conf'/>  

### Configurações gerais do MCP2515

<div id='MCP_var_filMask'/>  

### Filtros e Mascaras

<div id='MCP_var_erros'/>  

### Erros

<div id='MCP_var_frm'/>  

### Frames


<div id='MCP_fun'/>  

## Funções publicas

* ` `<br/>

Parâmetros de entrada:
1. ** **:
2. ** **:
3. ** **:

Exemplo de uso:
```C++

```
