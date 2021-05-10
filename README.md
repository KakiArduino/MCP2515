# MCP2515 (under construction)

 >John Welvins Barros de Araujo¹ <br /> 
 >Prof. Fabricio Ferrari¹ <br /> 
 >Prof. Edson M. Kakuno² <br /> 

¹ [IMEF](https://imef.furg.br/) - Universidade Federal do Rio Grande <br /> 
² [Física](http://cursos.unipampa.edu.br/cursos/licenciaturaemfisica/) - Unipampa Bagé <br /> 

A biblioteca MCP2515 foi feita para auxiliar/facilitar o controle do CI da Microchip, de mesmo nome, por plataformas Arduino, ou compatíveis.
As funções de configuração possuem valores padrão, de modo a fornecer uma configuração simples e rápida, porém está é limitada.
O CI MCP2515 configurado no modo padrão dessa biblioteca estará preparado para operar em uma rede de 125 k bit/s, sem a implementação de filtros de aceitança nos buffers de entrada, com rollover do buffer de saída **RXB0** para o **RXB1**, e além disso são habilitadas interrupções de recebimento e sucesso de envio.
Para setar outra configuração o usuário pode utilizar as funções *confMode()*, *confRX()*, *confRX()*, *confTX()* e *confINT()*.
Ou ainda as funções de escrita básica, *write(...)* e *bitModify(...)*.

A principal referencia desta biblioteca é o datasheet [MCP2515 Stand-Alone CAN Controller with SPI Interface](http://ww1.microchip.com/downloads/en/DeviceDoc/MCP2515-Stand-Alone-CAN-Controller-with-SPI-20001801J.pdf).
Por questões de comodidade muitos dos códigos numéricos relacionados a comunicação do MCP2515 com a plataforma Arduino foram expressos em hexadecimal.

## Frames

### Variáveis de um frame
* `uint16_t id_std;`
   > Variável de 2 bytes que armazena o valor do ID padrão do  frame.

* `uint32_t id_ext;`
   > Variável de \num{4} \textit{bytes} que armazena o valor da extensão do ID do \textit{frame}.
    
* `uint8_t dlc;`
   > Variável de \num{1} \textit{byte} que armazena o código de comprimento, número de \textit{bytes} de dados, do \textit{frame}.
    
* `uint8_t data[8];`
   > Lista de \textit{bytes} de dados do \textit{frame}.
    
* `uint8_t bts[14];`
   > Lista com todos os \textit{bytes} do \textit{frame}.
    
* `String type = "Unknown";`
   > String com o tipo do frame, padrão ("Std. Data"), estendido ("Ext. Data") ou "No frame" que é usado para indicar que não há novos frames nos buffers de entrada do MCP2515.



\subsection{\textbf{CANframe()}}
\mint{C++}{CANframe();}

Função para declaração de uma estrutura de variáveis do tipo \textit{CANframe} sem argumentos.

\begin{itemize}
  \item Exemplo de uso:
    \begin{enumerate}
        \item Declaração de um \textit{frame} CAN.
            \begin{minted}{C++} 
            CANframe frm();
            frm.id_std = 0x7FF;
            \end{minted}
            Declaração de um \textit{frame} sem fornecer parâmetros de entrada, seguida, na linha de baixo, pela atribuição de 0x7FF para o ID padrão, o maior valor possível, do \textit{frame} criado acima.
            
        \end{enumerate}
\end{itemize}


\subsection{\textbf{CANframe} (frameBytes, extFlag)}
\mint{C++}{CANframe(uint8_t *frameBytes, uint8_t extFlag = 0);}

Função para criação de \textit{frame}, a partir de uma lista com todos os \textit{bytes} do \textit{frame}.

\begin{itemize}
  \item Parâmetros de entrada:
    \begin{itemize}
        \item \textbf{frameBytes} lista com todos os \textit{bytes} do a serem atribuídos ao \textit{frame}. 

        \item \textbf{extFlag} sinalizador de extensão de ID, se \num{0}  o \textit{frame} é padrão, se \num{1} é estendido.

    \end{itemize}
    
  \item Exemplo de uso:
    \begin{enumerate}
        \item Declaração de um \textit{frame} CAN.
            \begin{minted}{C++}
            frameBytes [10] = {0x07, 0xFF, 
                               0x03, 0xFF, 0xFF,
                               0x04,
                               0x3, 0xFF, 0xF7, 0x21};
            
            CANframe frm(frameBytes, 1);
            
            Serial.println(frm.id_std);
            \end{minted}
            Primeiro é declarado a lista \textit{frameBytes}, os dois primeiros \textit{bytes} (0 e 1) compõem o ID padrão, concatenando tem-se 0x7FF o maior valor possível.
            Os três \textit{bytes} seguintes da lista \textit{frameBytes}, posições 2, 3 e 4, correspondem a extensão de ID, que neste caso vale o valor máximo 0x3FFFF.
            O \textit{byte} seguinte, posição 5, é o código dlc, que no caso indica quatro \textit{bytes} de dados.
        
            Após a declaração da lista com os \textit{bytes}, é feito a declaração do \textit{frame} estendido com a lista \textit{frameBytes}.
            Por fim o ID padrão é impresso através da variável \textit{id\_std} na última linha, isto é possível, pois ao declarar o \textit{frame}, todos suas variáveis são atribuídas.
            
        \end{enumerate}
\end{itemize}


\subsection{\textbf{CANframe} (idstd, idext, dlc\_, data)}
\mint{C++}{CANframe(uint16_t idstd, uint32_t idext, uint8_t dlc_, uint8_t *data);}

Função para criação de \textit{frame} estendido com declaração do ID padrão, extensão de ID, DLC - \textit{Data Len Code} e uma lista de bytes de dados, data.


\begin{itemize}
  \item Parâmetros de entrada:
    \begin{itemize}
        \item \textbf{idstd} variável de \num{2} \textit{bytes} onde deve ser informado o valor do ID padrão do \textit{frame}, no máximo 0x7FF.
        \item \textbf{idext} variável de \num{4} \textit{bytes} onde deve ser informado o valor da extensão de ID do \textit{frame}, no máximo 0x3FFFF.
        \item \textbf{dlc\_} número de \textit{bytes} de dados.
        \item \textbf{data} lista contendo os \textit{bytes} de dados dos \textit{frames}.

    \end{itemize}
    
  \item Exemplo de uso:
    \begin{enumerate}
        \item Declaração de um \textit{frame} CAN estendido.
            \begin{minted}{C++}
            uint8_t data[2] = {0, 10};
            CANframe frm(1, 6, 2, data);
            \end{minted}
                Na primeira linha é declarada um lista com os \textit{bytes} de dados.
                Seguido, na linha abaixo, da declaração de um \textit{frame} estendido com ID padrão igual a \num{1}, extensão de ID igual a \num{6}, e com dois \textit{bytes} de dados (dlc\_ = 2), sendo \num{0} e \num{10}.
            
        \end{enumerate}
\end{itemize}


\subsection{\textbf{CANframe} (idstd, dlc\_, data)}
\mint{C++}{CANframe(uint16_t idstd, uint8_t dlc_, uint8_t *data);}

Função para criação de \textit{frames} padrões pela a declaração do ID padrão, do DLC, e de um lista com os \textit{bytes} dos dados.

\begin{itemize}
  \item Parâmetros de entrada:
    \begin{itemize}
        \item \textbf{idstd} variável de \num{2} \textit{bytes} onde deve ser informado o valor do ID padrão do \textit{frame}, no máximo 0x7FF.
        \item \textbf{dlc\_} número de \textit{bytes} de dados.
        \item \textbf{data} lista contendo os \textit{bytes} de dados dos \textit{frames}.

    \end{itemize}
    
  \item Exemplo de uso:
    \begin{enumerate}
        \item Declaração de um \textit{frame} CAN estendido.
            \begin{minted}{C++}
            uint8_t data[2] = {0, 10};
            CANframe frm(1, 2, data);
            \end{minted}
                Na primeira linha é declarada um lista com os \textit{bytes} de dados.
                Seguido, na linha abaixo, da declaração de um \textit{frame} padrão com ID padrão igual a \num{1}, e com dois \textit{bytes} de dados (dlc\_ = 2), sendo \num{0} e \num{10}.
            
        \end{enumerate}
\end{itemize}


\subsection{\textbf{reload} (idstd, idext, dlc\_, data)}
\mint{C++}{reload(uint16_t idstd, uint32_t idext, uint8_t dlc_, uint8_t *data);}
Função para recarregar um \textit{frame} qualquer com estendido, atribuindo os valores fornecidos com entrada nas variáveis correspondentes.
Os parâmetros de entrada são os mesmo, e em mesma ordem, da função \textit{CANframe(idstd, idext, dlc\_, data)}, descrita um puco acima.

\begin{itemize}
  \item Exemplo de uso:
    \begin{enumerate}
        \item Declaração de um \textit{frame} CAN estendido.
            \begin{minted}{C++}
            uint8_t data[2] = {0, 10};
            CANframe frm(1, 2, data);
            frm.reload(1, 10, 2, data)
            \end{minted}
            As duas primeiras linhas deste exemplo já foram apresentadas como exemplo para a função \textit{CANframe(idstd, dlc\_, data)}, descrito anteriormente, nele é criado um \textit{frame} padrão.
            Na última linha o \textit{frame} \textit{frm} é recarregado como estendido com a função \textit{reload(..)}, a única diferença é a inserção do valor da extensão de ID (\num{10}).
                
            
        \end{enumerate}
\end{itemize}



\subsection{\textbf{reload} (idstd, dlc\_, data)} 
\mint{C++}{reload(uint16_t idstd, uint8_t dlc_, uint8_t *data);}

Função para recarregar um \textit{frame} qualquer com padrão, atribuindo os valores fornecidos com entrada nas variáveis correspondentes.
Os parâmetros de entrada são os mesmo, e em mesma ordem, da função \textit{CANframe(idstd, dlc\_, data)}, descrita um puco acima.

\begin{itemize}
  \item Exemplo de uso:
    \begin{enumerate}
        \item Declaração de um \textit{frame} CAN estendido.
            \begin{minted}{C++}
            uint8_t data[2] = {0, 10};
            CANframe frm(1, 10, 2, data);
            frm.reload(1, 2, data);
            \end{minted}
            Na primeiras linha é declarada uma lista com dois \textit{bytes} de dados.
            Na segunda linha um \textit{frame} estendido é declarado, tendo o ID padrão \num{1}, a extensão de ID \num{10}, o DLC \num{2}, e os dados atribuídos a lista \textit{data} na linha de cima.
            Na última linha o \textit{frame} \textit{frm} é recarregado com padrão, com a função \textit{reload(..)}, a diferença a ausência da extensão de ID.
            
        \end{enumerate}
\end{itemize}


%%%%
\subsection{\textbf{reload} (dlc\_, data\_)}
\mint{C++}{void reload(uint8_t dlc_, uint8_t *data_);}

Função para recarregar o campo de dados de \textit{frame} qualquer, essa função não altera os valores de ID, também pouco o tipo de \textit{frame}, também são alterado o DLC.
Essa função pode ser usada em um \textit{loop}, onde os dados do \textit{frame} são atualizados periodicamente, mas seus valores de ID não.

\begin{itemize}
  \item Exemplo de uso:
    \begin{enumerate}
        \item Declaração de um \textit{frame} CAN estendido.
            \begin{minted}{C++}
            
            uint8_t data[4] = {0, 10, 10, 0};
            frm.reload(4, data);
            
            \end{minted}
            Na primeiras linha é declarada uma lista com quatro \textit{bytes} de dados.
            Na segunda linha um \textit{frame} anteriormente criado (\textit{frm}) é recarregado com a lista de \textit{bytes} de dados \textit{data}.
            
        \end{enumerate}
\end{itemize}



\section{Variáveis Públicas} \label{bibMCP2515_varpubs}

\subsection{SPI}

\begin{itemize}
    \item \mint{C++}{unsigned long int SPI_speed = 10000000;}
        Máxima frequência do \textit{clock} do SPI na comunicação entre o Arduino e o MCP2515, que é limitada pelo último em \SI{10}{\mega \hertz}.
        O valor praticado pelo Arduino é outro, definido em função do seu \textit{clock} interno e do limite fornecido em \textit{SPI\_speed}.
    
    \item \mint{C++}{uint8_t SPI_wMode = 0;}
        Modo de operação da comunicação SPI entre o Arduino e o MCP2515, o controlador CAN suporta dois modos o [0,0] (0) e o [1,1] (3).
    
    \item  \mint{C++}{uint8_t SPI_cs;}
        Número do pino do Arduino usado como \textit{chip select} na comunicação SPI entre o Arduino e o MCP2515.
        
\end{itemize}
  
\subsection{Configuração gerais do MCP2515}\label{bibMCP2515_varpubs_conf}

\begin{itemize}
    \item \mint{C++}{uint8_t crystalCLK = 8;}
        Frequência do oscilador que fornece a base de \textit{clock} para o MCP2515, em \si{\mega\hertz}.
    
    \item \mint{C++}{uint16_t bitF = 125;}
        Taxa de \textit{bits} do barramento, em \si{\kilo} \textit{bits}/\si{\second}.
    
    \item \mint{C++}{uint8_t wMode = 0x00;}
        Está variável é usada para manipular o registro 0x0F do controlador CAN, 0x00 é modo de operação padrão da biblioteca, modo de operação \textit{Normal}, encerra a solicitação para cancelar o envio de todas as transmissões, modo \textit{One-Shot} desabilitado, pino \textit{CLKOUT} do MCP25 desabilitado e setando o \(F_{CLKOUT} = System Clock/1\).
        Para outras opções consultar o \textit{datasheet} do controlador CAN \citep{MCP2515}.
    
    \item \mint{C++}{uint8_t RXB0CTRL = 0x66;}
        Está variável é usada para manipular o registro 0x60 do MCP2515, ele configura o \textit{buffer} de entrada RXB0.
        O valor padrão 0x66, desabilita as mascaras e filtros e ativa o \textit{rollover} do \textit{buffer} de entrada RXB0 para o RXB1.
    
    \item \mint{C++}{uint8_t RXB1CTRL = 0x60;}
        Está variável é usada para manipular o registro de configuração do \textit{buffer} de saída RXB1 do MCP2515 (0x70).
        O valor padrão 0x60, desabilita as mascaras e filtros.
        
    
    \item \mint{C++}{uint8_t TXB0CTRL = 0x00;}
        Está variável manipula o registro 0x30, que corresponde a configuração do \textit{buffer} de saída TXB0 do MCP2515.
        O valor padrão 0x00, atribui o nível mais baixo de prioridade (\num{0}) e aborta possível \textit{frame} alocado buffer TXB0.
        Obs.: Quando todos os \textit{buffers} de saída possuem a mesma prioridade o, o TXB0 se torna o menos prioritário.
    
    \item \mint{C++}{uint8_t TXB1CTRL = 0x00;}
        Está variável manipula o registro 0x40, que corresponde a configuração do \textit{buffer} de saída TXB1 do MCP2515.
        O valor padrão 0x00, atribui o nível mais baixo de prioridade (\num{0}) e aborta possível \textit{frame} alocado buffer TXB0.
        Obs.: Quando todos os \textit{buffers} de saída possuem a mesma prioridade o, o TXB1 se torna mais prioritário do que o TXB0 e menos prioritário do que o TXB2.
        
    \item \mint{C++}{uint8_t TXB2CTRL = 0x00;}
        Está variável manipula o registro 0x50, que corresponde a configuração do \textit{buffer} de saída TXB2 do MCP2515.
        O valor padrão 0x00, atribui o nível mais baixo de prioridade (\num{0}) e aborta possível \textit{frame} alocado buffer TXB0.
        Obs.: Quando todos os \textit{buffers} de saída possuem a mesma prioridade o, o TXB2 se torna o mais prioritário.
        
    \item \mint{C++}{uint8_t CANINTE = 0xBF;}
        Está variável manipula o registro 0x2B do MCP2515.
        O valor padrão \textit{0xBF} no registro 0x2B habilita interrupções, quando uma mensagem é recebida no RXB0 ou no RXB1, quando qualquer um dos \textit{buffers} de saída  (TXB0, TXB1 e TXB2) ficar vazio, quando ocorrer erros de múltiplas fontes indicadas pelo registro EFLG, e quando uma transmissão de mensagem for interrompida.
    
    \item \mint{C++}{uint8_t BFPCTRL = 0x0F;}
        Está variável manipula o registro 0x0C do MCP2515.
        o valor padrã \textit{0x0F} no registro 0x0C configura os pinos do MCP2515, o RX0BF é habilitado e configurado como interrupção quando um mensagem valida é carregado no RXB0, e de forma análoga o RX1BF é para o \textit{buffer} RXB1. 
    
    \item \mint{C++}{uint8_t TXRTSCTRL = 0x00;}
        Está variável manipula o registro 0x0D do MCP2515.
        O valor padrão \textit{0x00} não habilita nenhum interrupção com relação os pinos TX0RTS, TX1RTS e TX2RTS.        
        
    \item \mint{C++}{uint8_t CNF1 = 0x00;}
        Está variável salva o valor grava no registro 0x2A do MCP2515, ele faz parte da configuração do \textit{Bit-Timing}.
        Há algumas opções pré definidas, estas podem ser configuradas  atribuindo valores as variáveis \textit{crystalCLK} e \textit{bitF}
        \num{125} \si{\kilo} \textit{bit}/\si{\second}, e dessa forma as variáveis \textit{CNF1}, \textit{CNF2} e \textit{CNF3} são atualizadas durante a inicialização feita pela função \textit{begin()} ou pela função de configuração especifica, \textit{confCAN()}.
        Os casos pré-definidos são: \textit{crystalCLK} = \num{4} com duas possíveis taxas \num{125} \si{\kilo} \textit{bit}/\si{\second} e  \num{250} \si{\kilo} \textit{bit}/\si{\second}.
        \textit{crystalCLK} = \num{8} com três possíveis taxas \num{125} \si{\kilo} \textit{bit}/\si{\second},  \num{250} \si{\kilo} \textit{bit}/\si{\second} e  \num{500} \si{\kilo} \textit{bit}/\si{\second}.
        \textit{crystalCLK} = \num{20} com quatro possíveis taxas \num{125} \si{\kilo} \textit{bit}/\si{\second},  \num{250} \si{\kilo} \textit{bit}/\si{\second}, \num{500} \si{\kilo} \textit{bit}/\si{\second} e \num{1000} \si{\kilo} \textit{bit}/\si{\second}.
        Para outros casos deve-se alterar \textit{CNF1} diretamente, mais detalhes consultar o \textit{datasheet} do MCP2515 \citep{MCP2515}.
    
    \item \mint{C++}{uint8_t CNF2 = 0x42;}
        Está variável salva o valor gravado no registro 0x29 do MCP2515. 
        Sua configuração segue de forma análoga a \textit{CNF1}.

    \item \mint{C++}{uint8_t CNF3 = 0x02;}
        Está variável salva o valor grava no registro 0x28 do MCP2515.
        Sua configuração segue de forma análoga a \textit{CNF1}.
        
\end{itemize}
 
\subsection{Filtros e Mascaras do MCP2515}
    
\begin{itemize}
    \item \mint{C++}{uint16_t MASstd[2] = {0x00, 0x00};}
        Lista com os valores dos ID padrões das mascaras 0 (MASstd[0]) e 1 (MASstd[1]) do MCP2515.
    
    \item \mint{C++}{uint32_t MASext[2] = {0x00, 0x00};}
        Lista com os valores das estações de ID das mascaras 0 (MASext[0]) e 1 (MASext[1]) do MCP2515.
    
    \item \mint{C++}{uint16_t FILstd[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};}
        Lista com os valores de ID padrão dos filtros do MCP2515, FILstd[0] corresponde ao filtro 0, e assim por diate, até FILstd[6] para o filtro 6.
    
    \item \mint{C++}{uint32_t FILext[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};}
        Lista com os valores de extensão ID dos filtros do MCP2515, FILext[0] corresponde ao filtro 0, e assim por diate, até FILext[6] para o filtro 6.
    
\end{itemize}

\subsection{Erros}

\begin{itemize}
    \item \mint{C++}{String errLog = "no error";}
        Armazena o último erro genérico ocorrido.
        Pode-se atualizar o valor de \textit{errLog} chamando a função \textit{errCont()}.
    
    \item \mint{C++}{String errMode = "Error Active";}
        Armazena o modo de confinamento de erro,  "Error Active", "Error Passive" e "Bus-Off".
        Pode-se atualizar o valor de \textit{errMode} chamando a função \textit{errCont()}.
    
    \item \mint{C++}{uint16_t RX0OVR = 0;}
        Armazena o número de \textit{overload} no \textit{buffer} RXB0.
        Pode-se atualizar o valor de \textit{RX0OVR} chamando a função \textit{errCont()}.
        
    \item \mint{C++}{uint16_t RX1OVR = 0;}
        Armazena o número de \textit{overload} no \textit{buffer} RXB1.
        Pode-se atualizar o valor de \textit{RX1OVR} chamando a função \textit{errCont()}.
        
    \item \mint{C++}{uint8_t multInt = 0;}
        Armazena o número de erros de multiplicas fontes detectados, indicados pelo registro \textit{ERROR FLAG REGISTER} (0x2D).
        Pode-se atualizar o valor de \textit{multInt} chamando a função \textit{errCont()}.
    
    \item \mint{C++}{uint8_t  MERRF = 0;}
        Armazena o número de erros de mensagens detectados.
        Pode-se atualizar o valor de \textit{MERRF} chamando a função \textit{errCont()}.
        
    \item \mint{C++}{uint8_t  WAKIE;}
        Armazena o valor da \textit{Wake-up flag} de MCP2515.
        Pode-se atualizar o valor de \textit{WAKIE} chamando a função \textit{errCont()}.
    
    \item \mint{C++}{uint8_t TEC = 0;}
        Armazena o valor do contador de erros de transmissão TEC.
        Pode-se atualizar o valor de \textit{TEC} chamando a função \textit{errCont()}.
    
    \item \mint{C++}{uint8_t REC = 0;}
        Armazena o valor do contador de erros de transmissão REC.
        Pode-se atualizar o valor de \textit{REC} chamando a função \textit{errCont()}.
        
\end{itemize}
  

\subsection{Frames}
\begin{itemize}
    \item \mint{C++}{CANframe frameRXB0;}
        Variável do tipo \textit{frame}, criada para receber os \textit{frames} recebidos no \textit{buffer} RXB0.
        Pode-se atualizar o valor de \textit{frameRXB0} chamando a função \textit{readFrame()}.
        
    \item \mint{C++}{CANframe frameRXB1;}
        Variável do tipo \textit{frame}, criada para receber os \textit{frames} recebidos no \textit{buffer} RXB1.
         Pode-se atualizar o valor de \textit{frameRXB1} chamando a função \textit{readFrame()}.
         
\end{itemize}

\section{Funções Públicas}

\subsection{Função: \textbf{MCP2515(...)}}\label{bibMCP_build}

\mint{C++}{mcp.reset(uint8_t spi_cs, 
                    unsigned long int spi_speed = 10000000, 
                    uint8_t spi_wMode = 0);}

A função \textit{MCP2515(...)} criar um objeto MCP2515, que herda as funções da biblioteca MCP2515.
Todos os parâmetros de entrada da função \textit{MCP2515(...)} são relacionados a comunicação SPI, e são listados abaixo, juntamente de dois exemplos de uso.

\begin{itemize}
  \item Parâmetros de entrada:
    \begin{itemize}
        \item \textbf{spi\_cs} é o número do pino do Arduino usado como \textit{chip select} do MCP2515.
  
        \item \textbf{spi\_speed} é a máxima frequência do \textit{clock} da comunicação SPI, seu valor padrão é 10000000.
        Obs.: Esse é o valor máximo suportado pelo MCP2515 e serve apenas como limite superior, a frequência do \textit{clock} é setada automaticamente pelo Arduino, dentro do limite informado.
        
        \item \textbf{spi\_wMode}  indica o modo de operação do SPI, o MCP2515 suporta o modo [\num{0},\num{0}] e o modo [\num{1},\num{1}] \citep{MCP2515}, que equivalem respectivamente, o \num{0} e \num{3} do Arduino \citep{SPIino}. O valor padrão é \num{0}.
    \end{itemize}
  
  \item Exemplos de uso:
    \begin{enumerate}
        \item Declaração de um objeto \textit{MCP2515}.
            \begin{minted}{C++} 
            #include <MCP2515_1.h> 
            MCP2515 mon(4);
            \end{minted}
        Neste exemplo foi, na primeira linha, incluído a versão 1 da biblioteca MCP2515, através do arquivo \textit{MCP2515\_1}. 
        Na segunda linha foi declarado um objeto \textit{MCP2515} chamado\textit{mon}, que tem como \textit{chip select} o pino digital 4 do Arduino.
        
        \item Outro exemplo de declaração de objeto \textit{MCP2515}.
            \begin{minted}{C++} 
            MCP2515 mcp(7, 10000000, 3);
            \end{minted}
        Desta vez foi declarado um objeto chamado \textit{mcp}, que tem como \textit{chip select} o pino digital 7 do Arduino, e usa o modo 3 do SPI do Arduino, que equivale [1, 1].
        \end{enumerate}
\end{itemize}

\subsection{Função: \textbf{begin()}}\label{bibMCP_begin}

\mint{C++}{mcp.begin();}

A função \textit{begin()} inicializa a comunicação SPI entre o Arduino e o MCP2515, e configura o controlador CAN para operar de acordo com os valores setados nas variáveis de configuração.
As variáveis de configuração, bem como seus valores padrões, foram descritas na secção anterior \ref{bibMCP2515_varpubs}.

\begin{itemize}
   \item Exemplos de uso:
    \begin{enumerate}
        \item Parte inicial da função \textit{void setup()} do CAN Mon.
            \begin{minted}{C++} 
            void setup() {
                Serial.begin(9600);
                mon.bitF = 125; // k bits/s
                mon.begin();
            \end{minted}
        Estas são as quatro primeiras linhas da função \textit{void setup()} do nodo CAN Mon (\textit{CANMon.ino}).
        Na penúltima linha é setado a taxa de \textit{bits} para 125 \si{\kilo} \textit{bits}/\si{\second}, \textbf{bitF} é a variável de controle que armazena o valor da taxa \textit{bits}. 
        Na última o MCP2515 é inicializado e configurado com a função \textit{mon.begin()}.
        
        \item Configuração d.
            \begin{minted}{C++} 
                sensor.begin();
            \end{minted}
        Neste exemplo o MCP2515 é inicializado e configurado no modo padrão, \textit{sensor} é o nome do objeto.
        \end{enumerate}
\end{itemize}

\subsection{Função: \textbf{reset()}}\label{bibMCP_reset}

\mint{C++}{mcp.reset();}
A função \textbf{reset()} não possui argumentos, ela reinicia o CI MCP2515 enviando a instrução \textbf{0xC0} pela comunicação SPI.
Atenção ao voltar do \textit{reset} o CI se encontra em modo de configuração e com valores padrão nos registros, e deve-se esperar algo entrono de \SI{2}{\micro \second} antes de usa-lo \citep{MCP2515}, isso pode ser feito através da função \textbf{delayMicroseconds()} do Arduino \citep{ArduinoRef}.
É aconselhável o uso dessa função logo após a inicialização do CI, antes de configura-lo, pois assim tem-se certeza dos valores salvos em seus registros.

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% - - - read

\subsection{Função: \textbf{read(...)}}\label{bibMCP_read}

\mint{C++}{mcp.read(uint8_t REG, uint8_t *data, uint8_t n = 1);}

A função \textbf{read(...)} realiza \textbf{n} (de \num{1} à \num{128}) leituras sequencias de registros do MCP2515, a partir do registro \textbf{REG} informado, e aloca os \textbf{n} \textit{bytes} em uma lista previamente criada e indicada pelo endereço \textbf{*data},  a lista data deve ter o tamanho de \textbf{n} \textit{bytes}.
Abaixo segue a descrição dos parâmetros de entradas da função \textbf{read(...)}, e na sequencia dois exemplos, um lendo um registro e o outro lendo \num{13} registros.

\begin{itemize}\label{bibMCP_readPa}
  \item Parâmetros de entrada:
    \begin{itemize}
        \item \textbf{REG} é o endereço do primeiro registro do MCP2515 a ser lido, deve ter uma tamanho de \num{1} \textit{byte} e seu valor vai 0x0 (\num{0}) até 0x80 (\num{128}).
  
        \item \textbf{*data} é o endereço da lista criada para armazenamento dos valores lidos, \SI{1}{\byte} para cada registro lido. 
        A lista \textbf{data} deve ter o tamanho de \textbf{n} \textit{bytes}.
        
        \item \textbf{n} é o número de registros a serem lidos, contando a partir do \textbf{REG}, por padrão \textbf{n}\(=1\), logo se não alterado a função \textbf{read(..)} lerá apenas um registro.
    \end{itemize}
  
  \item Exemplos de uso:
    \begin{enumerate}
        \item Leitura do registro TEC do MCP2515.
            \begin{minted}{C++} 
            uint8_t data[1]; 
            mcp.read(0x1C, data);
            \end{minted}
        Neste exemplo foi primeiro declaro uma lista (\textbf{data[1]}) com \num{1} \textit{byte} e na sequencia é realizado a leitura do registro \textbf{0x1C}, que armazena a contagem de erro de transmissão (TEC) do MCP2515.
        
        \item Leitura do \textit{buffer} de entrada RXB1 do MCP2515.
            \begin{minted}{C++} 
            uint8_t data[13]; 
            mcp.read(0x61, data, 2);
            \end{minted}
        Neste exemplo foi primeiro declaro uma lista com \num{13} \textit{bytes}, e na sequencia é realizado a leitura dos \num{13} registros do \textit{buffer} de entrada RB1 do MCP2515.
        \end{enumerate}
\end{itemize}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% - - - regcheck

\subsection{Função: \textbf{regCheck(...)}}\label{bibMCP_regCheck}

\mint{C++}{mcp.regCheck(uint8_t REG, uint8_t VAL, uint8_t extraMask = 0xFF);}

A função \textbf{regCheck(...)} confere se o \textit{byte} salvo no registro \textbf{REG} é igual ao \textit{byte} \textbf{VAL}, se for a função retorna \num{0}, se não ela retorna \num{1}. Se não for possível escrever no registro \textbf{REG} a função retorna \num{2}.
Pode-se usar essa função também para verificar o valor de um ou mais \textit{bits} do \textit{byte} armazenado em \textbf{REG}, para isso deve-se informar a mascara (\textbf{extraMask}) capaz de selecionar os \textit{bits} desejados pela operação lógica \& (\textit{and}).
Essa função pode ser chamada em conjunto com a função \textbf{write(...)}, de modo que seja feita uma verificação do sucesso da escrita no registro.
A seguir a descrição dos parâmetros de entrada da função \textbf{regCheck(...)} e dois exemplos de uso, sem e com mascara de seleção de \textit{bits}.

\begin{itemize}
  \item Parâmetros de entrada:
    \begin{itemize}
        \item \textbf{REG} é o endereço do registro do MCP2515 a ser verificado. 
        Deve ter uma tamanho de \num{1} \textit{byte} e seu valor vai 0x0 (\num{0}) até 0x80 (\num{128}).
  
        \item \textbf{VAL} é o \textit{byte} que se deseja verificar em \textbf{REG}. 
        Deve ter o tamanho de \num{1} \textit{byte}.
  
        \item \textbf{extraMask} é uma mascara de \num{1} \textit{byte}, para selecionar os bits desejados pela operação logica \& (and).
        Por padrão \textbf{extraMask} \(=\) 0xFF, logo por padrão a verificação é feita sobre todos os \textit{bits}.
    \end{itemize}
  
  \item Exemplos de uso:
    \begin{enumerate}
        \item Verificação do registro TEC do MCP2515.
            \begin{minted}{C++} 
            uint8_t check = 0; 
            check = mcp.regCheck(0x1C, 0);
            if(check =! 0){
            Serial.println("TEC > 0");
            }
            \end{minted}
        Neste exemplo é primeiro declaro a variável check para armazenar o retorno da função \textbf{regCheck(...)}.
        Depois é chamada a função \textbf{regCheck(0x1C, 0)}, que verificará se o valor salvo no registro \textbf{0x1C} é \num{0}. 
        O registro \textbf{0x1C} armazena a contagem do erros de transmissão (TEC) do MCP2515. 
        O valor retornado pela função \textbf{regCheck(...)} e salvo na variável check é comparado com \num{0}, se ele for diferente de zero, significa que TEC possui um valor maior que zero, e ao menos um erro de transmissão foi detectado.
        
        \item Verificando se o MCP2515 está em modo de configuração.
            \begin{minted}{C++} 
            uint8_t check = 0; 
            check = mcp.regCheck(0x0F, 0x80, 0xE0);
            if(check == 0){
                //realizar as configuração desejadas aqui.
            }
            \end{minted}
       Dessa vez a função \textbf{regCheck(...)} é usado com mascara, para verificar se o MCP2515 está no modo de configuração.
       O código do modo de operação é armazenado nos três \textit{bits} mais significantes do registro 0x0F, por isso o uso da mascara 0xE0. Se o MCP2515 estiver no modo de configuração o valor do registro 0x0F, após aplicação da mascara 0xE0, deve ser 0x80.
       
       
        \end{enumerate}
\end{itemize}


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% - - - write

\subsection{Função: \textbf{write(...)}}\label{bibMCP_write}

\mint{C++}{mcp.write(uint8_t REG, uint8_t VAL, uint8_t CHECK = 1);}

A função \textbf{write(...)} escreve o \textit{byte} \textbf{VAL} no registro \textbf{REG} e verifica se o valor foi realmente escrito. 
O sucesso pode ser verificado na variável \textbf{errLog}, no caso de falha ela valerá "Erro na escrita", se seu valor for "Reg invalido", o registro \textbf{REG} não pode ser escrito. Obs.: Há alguns registros que só podem ser escritos se o MCP2515 estiver no modo de configuração.
A checagem da escrita é opcional, e por padrão é feita, para que ela não seja feita basta fornecer \num{0} para \textbf{check}. Desabilitar a checagem fará com que a rotina \textbf{write(...)} execute em menos tempo, visto que não será feita a checagem.
A seguir a descrição dos parâmetros de entrada da função \textbf{write(...)}, e dois exemplos de utilização, com e sem verificação de escrita.

\begin{itemize}
  \item Parâmetros de entrada:
    \begin{itemize}
        \item \textbf{REG} é o endereço do registro do MCP2515 no qual pretende-se escrever. 
        Deve ter uma tamanho de \num{1} \textit{byte} e seu valor vai 0x0 (\num{0}) até 0x80 (\num{128}).
  
        \item \textbf{VAL} é o \textit{byte} que se deseja escrever em \textbf{REG}. 
        Deve ter o tamanho de \num{1} \textit{byte}.
  
        \item \textbf{check} é a sinalização de checagem, se fornecido \num{1} para \textbf{check} a função \textbf{write(...)} realizará a checagem, se fornecido \num{0} a função \textbf{write(...)} não realizará a checagem. 
        Por padrão a checagem é feita.
    \end{itemize}
  
  \item Exemplos de uso:
    \begin{enumerate}
        \item Escrevendo no registro 0x36 com verificação automática.
            \begin{minted}{C++} 
            mcp.write(0x36, 0xFE);
            if(errLog=="Erro na escrita"){
                Serial.println(mcp.errLog)
            }
            }
            \end{minted}
        Neste exemplo o \textit{byte} 0xFE é escrito no registro 0x36. 
        Por padrão a função \textbf{write(...)} verifica o sucesso da escrita, o que é feito comparando o valor da variável \textbf{errLog}.
        O registro 0x36 pertence ao \textit{buffer} de saída TXB1 do MCP2515.
        
        \item Escrevendo no registro 0x36 sem verificação.
            \begin{minted}{C++} 
             mcp.write(0x36, 0xFE, 0);
            }
            \end{minted}
        Desta vez a checagem é desabilitada informando o valor \num{0} para o parâmetro \textbf{check}.
        \end{enumerate}
\end{itemize}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% - - - bitModify

\subsection{Função: \textbf{bitModify(...)}}\label{bibMCP_bitModify}

\mint{C++}{mcp.bitModify(uint8_t REG, uint8_t MASK, uint8_t VAL, uint8_t check = 0);}

A função \textbf{bitModify(...)} é usada para alterar valores de bits específicos no registro \textbf{REG} do MCP2515.
Para tal deve-se fornecer um mascara (MASK, \num{1} \textit{byte}) capaz de selecionar os \textit{bits} que se deseja modificar pela operação lógica \& (and). 

\begin{itemize}
  \item Parâmetros de entrada:
    \begin{itemize}
        \item \textbf{REG} é o endereço do registro do MCP2515 do qual pretende-se alterar valores de \textit{bits}. 
        Deve ter uma tamanho de \num{1} \textit{byte} e seu valor vai 0x0 (\num{0}) até 0x80 (\num{128}).
  
        \item \textbf{MASK} é a mascara para selecionar a posições de \textit{bits} que deseja-se modificar.
        O \textit{bits} são selecionados pela operação lógica é a \& (and). Deve ter \num{1} \textit{byte} de tamanho.
        
        \item \textbf{VAL} é o \textit{byte} que contem o valor dos \textit{bits} a serem escritos no registro \textbf{REG}. Deve ter \num{1} \textit{byte} de tamanho.
  
        \item \textbf{check} é a sinalização de checagem, para está função o padrão é não checar e por isso \textbf{check}\(= 0\).
        Caso queira verificar o sucesso da alteração dos \textit{bits} fornece \num{1} para \textbf{check}, a habilitação da checagem prolonga o tempo de execução.
    \end{itemize}
  
  \item Exemplos de uso:
    \begin{enumerate}
        \item Seta o modo de operação \textit{Listen-Only}
            \begin{minted}{C++} 
                mcp.bitModify(0x0F, 0xE0, 0x60);
            \end{minted}
            Neste exemplo o MCP2515 é setado no modo \textit{Listen-Only}, alterando o valor dos três bits mais significantes do registro 0x0F, com a mascara 0xE0, para \num{0} \num{1} \num{1}.
        
        \item Seta o modo de operação \textit{Sleep}
            \begin{minted}{C++} 
                mcp.bitModify(0x0F, 0xE0, 0x20, 1);
            \end{minted}
            Desta vez o MCP2515 é setado para o modo \textit{Sleep}, alterando os três \textit{bits} mais significantes do registro 0x0F para \num{0} \num{1} \num{0}.
            Além disso é feita a verificação fornecendo \num{1} para o parâmetro \textbf{check}.
        
        \end{enumerate}
\end{itemize}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% - - - confRX

\subsection{Função: \textbf{confMode()}}\label{bibMCP_confMode}

\mint{C++}{mcp.confMode();}

A função \textbf{confMode()} atua sobre o registro 0x0F do MCP2515, nele pode configurar o modo de operação do controlador CAN e outras funções, para mais detalhes consulte o registro CANCTRL no \textit{datasheet} \citep{MCP2515}.
A variável \textit{wMode} guarda o valor a ser gravado no registro CANCRL, se este valor já estiver configurado, e a função for chamada, o MCP2515 é setado para o modo de configuração, no qual se pode alterar registros 'protegidos'.
Se o MCP2515 estiver no modo de configuração, a função retorna ao modo \textit{wMode}.

\begin{itemize}
   \item Exemplo de uso:
    \begin{enumerate}
        \item Configurando o MCP2515 para \textit{Listen-Only}.
            \begin{minted}{C++} 
                mcp.wMode = 0x60;    
                mcp.confMode();
            \end{minted}
            Na primeira linha é atribuído o valor 0x60 na variável de controle \textit{wMode}, este valor é escrito no registro 0x0F do MCP2515, setando o controlador para modo \textit{Listen-Only}.
            O valor padrão de \textit{wMode} é 0x00, modo \textit{Normal} do MCP2515.
        \end{enumerate}
\end{itemize}

\subsection{Função: \textbf{confRX()}}\label{bibMCP_confRX}

\mint{C++}{mcp.confRX();}

Esta função manipula os registros 0x60 e 0x70 do MCP2515, estes valores são definidos pelas variáveis de controle RXB0CTRL e RXB1CTRL, por padrão e respectivamente, 0x66 e 0x60.

\begin{itemize}
  \item Exemplo de uso:
    \begin{enumerate}
        \item Habilitação dos filtros de aceitança do \textit{buffer} RXB1.
            \begin{minted}{C++} 
                mcp.RXB1CTRL = 0x00;
                mcp.confRX();
            \end{minted}
            Neste exemplo é setado o valor 0x00 no registro 0x70 (RXB1CTRL) do MCP2515, este valor habilita os filtros e mascaras no \textit{buffer} de saída RXB1 do MCP2515.
            
        \end{enumerate}
\end{itemize}

\subsection{Função: \textbf{confTX()}}\label{bibMCP_confTX}

\mint{C++}{mcp.confTX();}

A função \textbf{confTX()} manipula os registros 0x30, 0x40 e 0x50, neles são configurados os \textit{buffers} de saída do MCP2515, respectivamente, TXB0, TXB1 e TXB2.

\begin{itemize}
  \item Exemplo de uso:
    \begin{enumerate}
        \item  Configurando a prioridade interna dos \textit{buffers} de saída do MCP2515.
            \begin{minted}{C++} 
                mcp.TXB0CTRL = 0x03
                mcp.TXB1CTRL = 0x00
                mcp.TXB2CTRL = 0x01
                mcp.confTX();
            \end{minted}
             
             Neste exemplo o TXB0 é configurado com a mais alta prioridade, enquanto que o TXB1 e configurado no nível mais baixo, e por fim o TXB2 é setado no nível 1 de prioridade.
             Existem quatro níveis de prioridade.
        \end{enumerate}
\end{itemize}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% - - - confINT

\subsection{Função: \textbf{confINT()}}\label{bibMCP_confINT}

\mint{C++}{mcp.confINT();}

A função \textbf{confINT()} configuras os registros 0x2B, 0x0C e 0x0D do MCP2515, através das variáveis de controle e respectivamente, \textit{CANINTE}, \textit{BFPCTRL} e \textit{TXRTSCTRL}. 
Outros valores podem ser consultados no \textit{datasheet} do MCP2515 \citep{MCP2515}.

\begin{itemize}
  \item Exemplo de uso:
    \begin{enumerate}
        \item Configurações padrões.
            \begin{minted}{C++} 
                mcp.CANINTE = 0xBF;
                mcp.BFPCTRL = 0x0F;
                mcp.TXRTSCTRL = 0x00;
                mcp.confINT();
            \end{minted}
             Neste exemplo a função \textit{confINT()} é chamada na ultima linha, após a atribuição das três variáveis de controle correspondentes aos registros manipulados pela a função.
             Mas detalhes sobre as variáveis na subseção \ref{bibMCP2515_varpubs_conf}.
             
        \end{enumerate}
\end{itemize}


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% - - - conf

\subsection{Função: \textbf{confFM()}}\label{bibMCP_confINT}

\mint{C++}{mcp.confFM();}

A função \textbf{confFM()} configura os valores das mascaras e filtros de ID, padrão e extensão, nos \textit{buffers} de entrada do MCP2515.

\begin{itemize}
  \item Exemplo de uso:
    \begin{enumerate}
        \item Configurações de filtros e mascaras.
            \begin{minted}{C++} 
                mcp.MASstd[0] = {0x400};
                mcp.MASstd[1] = {0x400};
                mcp.FILstd[0] = {0x7FF};
                mcp.FILstd[1] = {0x7FF};
                mcp.FILstd[2] = {0x7FF};
                mcp.FILstd[3] = {0x7FF};
                mcp.FILstd[4] = {0x7FF};
                mcp.FILstd[5] = {0x7FF};
                mcp.FILstd[6] = {0x7FF};
                mcp.confFM();
            \end{minted}
            Neste exemplo as mascaras e filtros são setados através da atribuição das variáveis de configuração relacionadas, no caso especifico o MCP2515 só aceitara \textit{frames} padrão, cujo valor do ID padrão seja maior que 0x400.
            Mais detalhes do funcionamento dos filtros e mascaras do controlador CAN podem ser vistas no \textit{datasheet} \citep{MCP2515}.
        \end{enumerate}
\end{itemize}

\subsection{Função: \textbf{confCAN()}}\label{bibMCP_confINT}

\mint{C++}{mcp.confCAN();}

A função \textbf{confCAN()} 

\begin{itemize}
  \item Exemplo de uso:
    \begin{enumerate}
        \item Configurações padrões.
            \begin{minted}{C++}
                mcp.crystalCLK = 8; // M Hz
                mcp.bitF = 125; // k bit/s
                mcp.confCAN();
            \end{minted}
            Na primeira linha é atribuído o valor de \num{8} para a variável de configuração \textit{crystalCLK}, deve ser informado nesta variável o valor do \textit{clock} fornecido ao controlador CAN MCP2515.
            Também é atribuído, na segunda linha, o valor da variável de configuração \textit{bitF}, que contem a taxa de transferência de \textit{bits} utilizada no barramento CAN.
            Na ultima linha é chamada a função \textit{confCAN()}, que realiza a configuração dos registros 0x2A, 0x29 e 0x28, os valores gravados podem ser consultados respectivamente, após a função \textit{confCAN()}, nas variáveis de configuração CNF1, CNF2 e CNF3.
        \end{enumerate}
\end{itemize}

\subsection{Função: \textbf{status(...)}}\label{bibMCP_status}

\mint{C++}{mcp.status(uint8_t *status);}

A função \textbf{status(...)} realiza a leitura do \textit{byte} de \textit{status} do MCP2515 e salva no endereço indicado por  \textbf{*status}, que deve ter o tamanho de um \textit{byte}.

\begin{itemize}
  \item Parâmetros de entrada:
    \begin{itemize}
        \item \textbf{*status} é o ponteiro que indica a posição a ser salvo o \textit{byte} lido pela função \textbf{status(...)}. Aconselha-se usar uma lista de uma posição, como no exemplo.
  
    \end{itemize}
  
  \item Exemplo de uso:
    \begin{enumerate}
        \item Leitura e impressão serial do \textit{status} do MCP2515.
            \begin{minted}{C++} 
               uint8_t status[1];
               mcp.status(status);
               Serial.println(status[0]);
            \end{minted}
            
        \end{enumerate}
\end{itemize}

\subsection{Função: \textbf{errCont()}}\label{bibMCP_errCont}

\mint{C++}{mcp.errCont();}

A função \textbf{errCont()} realiza a leitura dos registro relacionados a contagem de erro, TEC (0x1C) e REC (0x1D), e verifica a ocorrência de \textit{overflow} nos \textit{buffers} de entrada, lendo o registro de sinalização de erros (0x2D).
As leituras são atualizadas nas variáveis, \textit{TEC}, \textit{REC}, \textit{RX0OVR}, \textit{RX1OVR}, \textit{MERRF}, \textit{WAKIE}, \textit{multInt} e \textit{errMode}.
Uma descrição destas variáveis está disponível na subseção \ref{bibMCP2515_varpubs}.

\begin{itemize}
  \item Exemplo de uso:
    \begin{enumerate}
        \item Contagem de erros.
            \begin{minted}{C++} 
               mcp.errCont();
               Serial.println(mcp.TEC);
               Serial.println(mcp.errMode);
            \end{minted}
            Neste exemplo é consultado o valor do contador de erros de transmissão do MCP2515 (TEC), e o modo do confinamento por erro.
            A execução da função \textit{errCont()}, atualiza as variáveis dos contadores de erros.
        \end{enumerate}
\end{itemize}

\subsection{Função: \textbf{writeID(...)}}\label{bibMCP_writeID}

\begin{minted}{C++} 
writeID(uint16_t sid, uint8_t id_ef = 0, uint32_t eid = 0, 
        uint8_t txb = 0, uint8_t timeOut = 10, uint8_t check = 0);
\end{minted}

A função \textbf{writeID(...)} realiza a escrita do identificador da mensagem \textbf{ID} no \textit{buffer} de saída indicado por \textbf{txb}.
No caso de haver mensagem pendente de transmissão, a função não irá sobrepor o ID da mensagem pendente, ela tentará repetidamente até conseguir escrever o ID ou até esgotar o tempo \textbf{timeOut} fornecido em \si{\milli \second}.

\begin{itemize}
  \item Parâmetros de entrada:
    \begin{itemize}
        \item \textbf{sid} é uma variável de dois \textit{bytes}, nela é informado o valor do ID padrão.
        O maior valor possível é 0x3FF.

        \item \textbf{id\_ef} é a \textit{flag} de extensão de ID, e deve ser informado \num{0} para ID padrão e \num{1} para ID com extensão.
        Por padrão essa variável vale \num{0}.
    
        \item \textbf{eid} é uma \textit{word} de \num{32} \textit{bits} que contem a extensão identificação (ID) da mensagem a ser enviada.
        Por padrão essa variável vale \num{0}.
        O maior valor possível é 0x7FFFF.
        
        \item \textbf{txb} indica o \textit{buffer} no qual pretende-se escrever o ID, se fornecido \num{0} o ID é escrito no TXB0, se \num{1} é escrito no TXB1, se \num{2} é escrito no TXB2, se for fornecido \num{3} o mesmo ID é escrito nos três \textit{buffers} de saída.
        Por padrão essa variável vale \num{0}.
        
        \item \textbf{timeOut} é o tempo (em \si{\milli \second}) limite para que a função consiga escrever no ID.
        Se o \textit{buffer} escolhido permanecer ocupado durante todo o \textbf{timeOut} a função retornará sem escrever o ID. É esperado por padrão \SI{10}{\milli \second}
        
        \item \textbf{check} é a sinalização de checagem de escrita, quando fornecido \num{1} a checagem é feita, se fornecido \num{0} ela não é feita. Por padrão a checagem é feita.
        
    \end{itemize}
  
  \item Exemplos de uso:
    \begin{enumerate}
        \item Setando um ID padrão e uma extensão de ID no \textit{buffer} de saída TXB0.
            \begin{minted}{C++} 
               uint16_t IDstd = 0x700;
               uint32_t IDext = 0x30000; 
               mcp.writeID(IDstd, 1, IDext);
            \end{minted}
            Neste exemplo o ID padrão 0x700 e a extensão de ID 0x30000 são escritos no \textit{buffer} de saída TXB0, com os valores padrões de \textit{time out} e checagem.
            
        \item Escrevendo o ID padrão 10 em um \textit{buffer} de saída. \textit{time out} desabilitado.
            \begin{minted}{C++} 
               mcp.writeID(10, 0, 0, 1, 0);
            \end{minted}
            Desta vez, o ID padrão escrito no \textit{buffer} de saída TXB1 o valor 10, sem extensão de ID e sem \textit{time out}.
            
        \end{enumerate}
\end{itemize}


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% - - - loadTX

\subsection{Função: \textbf{loadTX(...)}}\label{bibMCP_loadTX}

\mint{C++}{mcp.loadTX(uint8_t *data, uint8_t n = 8,  uint8_t abc = 1);}

A função \textbf{loadTX(...)} realiza a escrita sequencial dos \textbf{n} \textit{bytes} armazenados na lista indicada por \textbf{*data}, nos \textbf{n} registros sequenciais dos \textit{buffers} de saída do MCP2515, começando pelo registro indicado pelo código \textbf{abc} (de \num{0} à \num{5}).
Se fornecido \num{0} para \textbf{abc} a escrita começa do registro 0x31, se fornecido \num{1} começa do 0x36, se se fornecido \num{2} começa do 0x41, se se fornecido \num{3} começa do 0x46, se \textbf{abc} = \num{4} a começa do 0x51 e se \textbf{abc} = \num{5} a começa do 0x56.


\begin{itemize}
  \item Parâmetros de entrada:
    \begin{itemize}
        \item \textbf{*data} é o endereço da lista que armazena os \textbf{n} \textit{bytes} a serem escritos nos \textit{buffers}. 
        
        \item \textbf{n} é o número de \textit{bytes} a serem escritos.
        
        \item \textbf{abc} é o código que marca o registro de partida da escrita sequencial.
  
    \end{itemize}
  
  \item Exemplo de uso:
    \begin{enumerate}
        \item Escrita sequencial com a função \textbf{loadTX(...)}.
            \begin{minted}{C++} 
                uint8_t data = {0, 2, 1, 3, 4, 5, 6, 7};
                mcp.loadTX(data, 8, 1);
            \end{minted}
            Neste exemplo a sequencia de \num{8} \textit{bytes} armazenada na lista \textbf{data} é sequencialmente escrita nos registros que armazenam os dados da mensagem a ser enviada pelo \textit{buffer} TXB0 (do 0x36 até o 0xD). 
            
        \end{enumerate}
\end{itemize}

\subsection{Função: \textbf{send(...)}}\label{bibMCP_send}

\mint{C++}{mcp.send(uint8_t txBuff = 0x01);}

A função \textbf{send(...)} realiza o pedido de transmissão da mensagem armazenada no \textit{buffer} ou \textit{buffers} indicados por \textbf{txBuff}.
Se \textbf{txBuff} = \num{1}, será pedido o envio da mensagem salva no \textit{buffer} TXB0, se \textbf{txBuff} = \num{2} o pedido será para o TXB1, se \textbf{txBuff} = \num{3} o pedido será para os \textit{buffers} TXB0 e TXB1, se \textbf{txBuff} = \num{4} o pedido será para o TXB2, se \textbf{txBuff} = \num{5} o pedido será para os \textit{buffers} TXB0 e TXB2, se \textbf{txBuff} = \num{6} o pedido será para os \textit{buffers} TXB1 e TXB2, e se \textbf{txBuff} = \num{7} o pedido será para os todos os \textit{buffers} de saída.
Quando for solicitado o envio de mais de uma mensagem, será primeiro enviado a armazenada no \textit{buffer} mais prioritário. 

\begin{itemize}
  \item Parâmetros de entrada:
    \begin{itemize}
        \item \textbf{txBuff} é o código de pedido de transmissão, seu valor vai de \num{1} a \num{7}.
  
    \end{itemize}
  
  \item Exemplo de uso:
    \begin{enumerate}
        \item Solicitação de envio da mensagem salva no \textit{buffer} de transmissão TXB2.
            \begin{minted}{C++} 
               mcp.send(0x04);
            \end{minted}
        \end{enumerate}
\end{itemize}

\subsection{Função: \textbf{writeFrame(...)}}\label{bibMCP_writeFrame}

\begin{minted}{C++} 
writeFrame(CANframe frameToSend, uint8_t txb_ = 0, 
           uint8_t timeOut = 10, uint8_t check = 0)
\end{minted}

A função \textbf{writeFrame(...)} realiza a escrita de um \textit{frame}, indicado por \textit{frameToSend}, no \textit{buffer} informado por \textbf{txb\_} (\num{0}, \num{1} ou \num{2}).
A função \textbf{writeFrame(...)} tentará escrever repetidamente até conseguir, ou até que acaba o tempo limite de execução indicado em \textbf{timeOut}.

\begin{itemize}
  \item Parâmetros de entrada:
    \begin{itemize}
        \item \textbf{frameToSend} é uma variável do tipo \textit{CANframe} que armazena a mensagem a ser enviada.
        Essa variável contem todas informações do \textit{frame}, ver subseção \ref{bibMCP2515_frames} para mais detalhes.
        
        \item \textbf{txb\_} indica o \textit{buffer} de saída no qual pretende escrever os dados. Para TXB0 deve ser fornecido \num{0}, para TXB1 \num{1} e para TXB2 \num{2}.
        
        \item \textbf{timeOut} é o tempo máximo de execução da função, a \textbf{writeFrame(...)} tentará escrever no \textit{buffer} escolhido até que este esteja desocupada e possa receber a nova mensagem, ou até que acabe o tempo de execução máximo (\textit{time out}) fornecido.
        \textbf{timeOut} é expresso em \si{\milli \second} é vale, por padrão, \SI{10}{\milli \second}.
        
        \item \textbf{check} é a sinalização de checagem de escrita, se for fornecido \num{1} ela será feita, se for fornecido \num{0} não.
        A checagem custa tempo de execução, que não é incrementado no \textbf{timeOut}, e por padrão ela é feita.
  
    \end{itemize}
  
  \item Exemplo de uso:
    \begin{enumerate}
        \item Envio de um \textit{frame} com os \textit{bytes} de uma leitura analógica.
            \begin{minted}{C++} 
              uint16_t ID_std = random(0x7FF);
              uint32_t ID_ext = random(0x3FFFF);
              
              UNION_t Data;
              Data.INT = analogRead(A0);
              
              CANframe frm(ID_std, sizeof(Data), Data.bytes);
              
              mcp.writeFrame(frm);
            \end{minted}
            
        \item Envio de um \textit{frame} com os \textit{bytes} de um variável do tipo \textit{float} pelo \textit{buffer} de saída TXB2. 
            \begin{minted}{C++} 
              UNION_t Data;
              Data.FLT = analogRead(A0)*(5.0 / 1023);
              
              frm.reload(sizeof(Data.FLT), Data.bytes);
              
              mcp.writeFrame(frm, 2);
            \end{minted}
            Neste exemplo o \textit{frame} (\textit{frm}) foi apenas atualizado com um novo dado, dessa forma os valores de ID, não são alterados.
              
        \end{enumerate}
\end{itemize}

\subsection{Função: \textbf{abort(...)}}\label{bibMCP_send}

\mint{C++}{mcp.abort(uint8_t abortCode = 7);}

A função \textbf{abort(...)} 

\begin{itemize}
  \item Parâmetros de entrada:
    \begin{itemize}
        \item \textbf{abortCode} é o código de cancelamento de envio, seu valor pode ser \num{1}, \num{2}, \num{4} ou \num{7}, e por padrão é \num{7} que aborta todos os envios pendentes nos \textit{buffer} de saída.
  
    \end{itemize}
  
  \item Exemplo de uso:
    \begin{enumerate}
        \item Cancelamento do envio da mensagem salva no \textit{buffer} de saída TXB2.
            \begin{minted}{C++} 
               mcp.abort(4);
            \end{minted}
        
        \item Cancelamento do envio de todas mensagem salvas nos \textit{buffers} de saída.
            \begin{minted}{C++} 
               mcp.abort();
            \end{minted}
        \end{enumerate}
\end{itemize}

\subsection{Função: \textbf{readID(...)}}\label{bibMCP_readID}

\mint{C++}{mcp.readID(uint8_t *id, uint8_t rxb = 0);}

A função \textbf{readID(...)} faz a leitura do ID padrão, e caso haja da extensão, de uma mensagem armazenada no \textit{buffer} de entrada \textbf{rxb}.
Os \num{5} \textit{bytes} do ID lidos, dois para o ID padrão e três para extensão, são salvos \textit{byte} a \textit{byte} na lista indicada pelo endereço \textbf{*id}.
A função \textbf{readID(...)} é capaz de ler tanto o ID padrão quanto o estendido, e a princípio não é sabido o formato do ID, por isso deve-se sempre reservar uma lista de \num{5} \textit{bytes}.

\begin{itemize}
  \item Parâmetros de entrada:
    \begin{itemize}
        \item \textbf{*id} é o endereço da lista onde serão armazenados os \textit{bytes} do ID. 
        Deve ser um lista de \num{5} \textit{bytes}.
        
        \item \textbf{rxb} indica o \textit{buffer} a ser lido, se fornecido \num{0} é lido o RXB0, se fornecido \num{1} é lido RXB1.
        
    \end{itemize}
  
  \item Exemplo de uso:
    \begin{enumerate}
        \item Leitura, reconstrução e impressão do ID padrão e da extensão salvas no RXB1.
            \begin{minted}{C++} 
                uint8_t ID[5];
                mcp.readID(ID, 1);
                
                IDunion_t IDstd, IDext;
                
                IDstd.bytes[0] = ID[0];
                IDstd.bytes[1] = ID[1];
                
                IDext.bytes[0] = ID[2];
                IDext.bytes[1] = ID[3];
                IDext.bytes[2] = ID[4];
                IDext.bytes[3] = 0;
                
                Serial.print(IDstd);
                Serial.print(' ');
                Serial.println(IDext);
            \end{minted}
            
        \end{enumerate}
\end{itemize}

\subsection{Função: \textbf{readFrame()}}\label{bibMCP_readDATA}
\mint{C++}{mcp.readFrame()}

A função \textbf{readFrame(...)} realiza a leitura dos \textit{buffers} de entrada RXB0 e RXB1, e caso haja mensagem valida a salva nos \textit{frames} auto declarados \textit{frameRXB0} e \textit{frameRXB1}, respectivamente.
Os frames são declarados automaticamente na criação de um objeto MCP2515.
Antes da leitura a função verifica se ha mensagem valida em um dos \textit{buffers} de entrada, e caso haja realiza a leitura da mensagem, se não houver a função \textbf{readFrame(...)} atualiza o valor \textit{type} do \textit{frame} respectivo para "No frame".
Pode acontecer de haverem mensagens validas nos dois \textit{buffers} e neste caso a função \textbf{readFrame(...)} atualizará ambos \textit{frames}.


\begin{itemize}
   \item Exemplo de uso:
    \begin{enumerate}
        \item Leitura e impressão de mensagens no \textit{buffer} de entrada RXB0 do MCP2515.
            \begin{minted}{C++} 
               
               mcp.readFrame();
               
               if(frameRXB0.type != "No frame"){
                Serial.println("Frame recebido pelo RXB0");
                Serial.print(frameRXB0.id_std);
                Serial.print(' ');
                Serial.print(frameRXB0.id_ext);
                Serial.print(' ');
                Serial.print(frameRXB0.dlc);
                
                for(int i=0; i < frameRXB0.dlc; i++){
                    Serial.print(' ');
                    Serial.print(frameRXB0.data[i]);
                }
                Serial.println();
               }
            \end{minted}
            O processo feito para impressão do RXB0, a partir do \textit{if(...)}, pode ser feito para o RXB1, trocando o \textit{frameRXB0} por \textit{frameRXB1}. 
        \end{enumerate}
\end{itemize}
\newpage
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\subsection{Função: \textbf{digaOi(...)}}\label{bibMCP_checkDATA}
\mint{C++}{mcp.digaOi(char *oi);}

A função \textbf{digaOi(...)} pode imprimir diversos parâmetros internos da biblioteca, inclusive aqueles relacionados a configurações do MCP2515.
Se o parâmetro de entrada não for informado a função \textit{digaOi(...)}, imprimira todas as \num{40} variáveis descritas na seção \ref{bibMCP2515_varpubs}.

\begin{itemize}
  \item Parâmetros de entrada:
    \begin{itemize}
        \item \textbf{oi} é o \textit{char} que informa quais variáveis deseja-se imprimir, se informado "status", será impresso somente as variáveis relacionadas ao estado do MCP2515 e seus \textit{buffers} de saída e entrada.
        Se informado "eros" ou "errors", serão impressos valores de erros.
        Se informado "SPI", serão impressos os parâmetros do SPI.
        Se informado "CAN", serão impressos os parâmetros da configuração da CAN.
        Se informado "controle", serão impressos os parâmetros de controle do MCP2515.
        Se informado "filtros", serão impressos os filtros setados no MCP2515.
        Se informado "filtros", serão impressos as mascaras setados no MCP2515.
    \end{itemize}

  \item Exemplo de uso da função \textbf{digaOi(...)} sem informar parâmetro de entrada.
    \begin{minted}{C++} 
        Serial.begin(9600);
        mcp.digaOi();
    \end{minted}
    Neste exemplo a função \textbf{digaOI(...)} imprimira todas as variáveis.

  \item Exemplo de uso da função \textbf{digaOi(...)} imprimindo somente os parâmetros relacionados a erros.
    \begin{minted}{C++} 
        Serial.begin(9600);
        mcp.digaOi("error");
    \end{minted}
   
\end{itemize}

%##########################################

\begin{landscape}

\section{Códigos e exemplos}

\subsection{MCP2515.h}\label{MCP2515_h}
\inputminted{C++}{codigos/MCP2515_1.h}
\newpage

\subsection{MCP2515.cpp}\label{MCP2515_cpp}
\inputminted{C++}{codigos/MCP2515_1.cpp}
\newpage

\subsection{CANRX.ino}\label{CANreceiver_ino}
\inputminted{C++}{codigos/CANRX.ino}
\newpage

\subsection{CANTX.ino}\label{CANTX_ino}
\inputminted{C++}{codigos/CANTX.ino}
\newpage



\subsection{CAN Mon}\label{CANmon_ino}

\subsubsection{CANMon.h}
\inputminted{C++}{codigos/CANMon.h}
\newpage

\subsubsection{CANMon.ino}
\inputminted{C++}{codigos/CANMon.ino}
\newpage


\subsection{CAN Save}\label{CANsave_ino}

\subsubsection{CANSave.h}
\inputminted{C++}{codigos/CANSave.h}
\newpage

\subsubsection{CANSave.ino}
\inputminted{C++}{codigos/CANSave.ino}
\newpage


\subsection{CAN Sensor}\label{CANSensor_ino}

\subsubsection{CANSensor.h}
\inputminted{C++}{codigos/CANSensor.h}
\newpage

\subsubsection{CANSensor.ino}
\inputminted{C++}{codigos/CANSensor.ino}

\end{landscape}

