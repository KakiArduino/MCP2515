# MCP2515 (em contrução)

 >John Welvins Barros de Araujo¹ <br /> 
 >Prof. Fabricio Ferrari¹ <br /> 
 >Prof. Edson M. Kakuno² <br /> 

¹ [IMEF](https://imef.furg.br/) - Universidade Federal do Rio Grande <br /> 
² [Física](http://cursos.unipampa.edu.br/cursos/licenciaturaemfisica/) - Unipampa Bagé <br /> 

A biblioteca MCP2515 foi feita para auxiliar/facilitar o controle do CI da Microchip, de mesmo nome, por plataformas Arduino, ou compatíveis.
As funções de configuração possuem valores padrão, de modo a fornecer uma configuração simples e rápida.
O CI MCP2515 configurado no modo padrão dessa biblioteca estará preparado para operar em uma rede de 125 k bit/s, sem a implementação de filtros de aceite nos buffers de entrada, com rollover do buffer de saída RXB0 para o RXB1, e além disso são habilitadas interrupções que indicam o sucesso de recebimento e envio.
Para setar outra configuração o usuário deverá atualizar os valores das [variáveis de configuração](#varpubs_conf), e em seguida executar a função correspondente aos registros modificados, como a [*confMode()*](#confMode), [*confRX()*](#confRX), [*confTX()*](#confTX), [*confINT()*](#confINT), [*confFM()*](#confFM) e [*confCAN()*](#confCAN).
Ou ainda as usando as funções de escrita básica, [*write (REG, VAL, CHECK = 1)*](#write) e [*bitModify (REG, MASK, VAL, check = 0)*](#bitModify).





*******
Sumário:
 1. [Variáveis de um frame](#frames_var)   
 3. [Declaração de frames](#frames_fun)
 4. [Variaives publicas](#MCP_var)
    4.1 [SPI] (#MCP_var_SPI)
    4.2 [Configurações gerais do MCP2515] (#MCP_var_conf)
    4.3 [Filtros e Mascaras](#MCP_var_filMask)
    4.4 [Erros](#MCP_var_erros)
    4.5 [Frames](#MCP_var_frm)
 5. [Funções publicas](#MCP_fun)

*******

<div id='frames_var'/>  
## Variáveis de um frame
 
<div id='frames_fun'/> 
## Declaração de frames


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
