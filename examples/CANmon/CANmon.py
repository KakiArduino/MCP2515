import multiprocessing as mult
import serial
import time
import numpy as np
import sys


print("----- CANmon v1 ---------")
print("Programa de monitoramento")
print("para uma mini rede de sensores CAN\n")
print("John W. B. de Araujo - PPGFIS IMEF FURG")
print("Dr. Fabrico Ferrari - PPGFIS IMEF FURG")
print("Dr. Edson M. Kakuno - Fisica Unipampa Bage")
print("Esse programa, bem como os demais")
print("componentes da rede proposta, foi")
print("desenvolvida com auxilio financeiro") 
print("parcial da CAPES, gorv. do Brasil")
print("mais detalhes:\ngithub.com/KakiArduino/CAN-Bus-MCP2515\n\n")
print(time.asctime(time.localtime()))
print

print("Digite a porta serial")
print("Ex. no Linux: /dev/ttyUSBXX") 
print("Ex. no Windows: COMXX") 
print("XX = numero inteiro")
port = raw_input()
#port = '/dev/ttyUSB1'
print

baud = 9600

mon = serial.Serial(port, baud, timeout=0)

print("Digiti um nome de arquivo para\narmazenamento dos dados com a extensao")
arqN = raw_input()

arq = open(arqN, 'a')

arq.write("CANmon v1\n")
arq.write("programa de monitoramento\n")
arq.write("para uma mini rede de sensores CAN\n\n")
arq.write("John W. B. de Araujo - PPGFIS IMEF FURG\n")
arq.write("Dr. Fabrico Ferrari - PPGFIS IMEF FURG\n")
arq.write("Dr. Edson M. Kakuno - Fisica Unipampa Bage\n\n")
arq.write("Esse programa, bem como os demais\n")
arq.write("componentes da rede proposta, foi\n")
arq.write("desenvolvida com auxilio financeiro\n") 
arq.write("parcial da CAPES, gorv. do Brasil\n")
arq.write("mais detalhes acesse: ")
arq.write("github.com/KakiArduino/CAN-Bus-MCP2515\n\n")
arq.write(time.asctime(time.localtime()))
arq.write('\n')
arq.write("ID (n Bytes): data\n")
arq.close()


buff = mult.Array('i', 10000)
conter = mult.Array('i', 1)

lock = mult.Lock() 	


def printBuff(arrayShared, LOCK, cont):
	printBuff = []
	prt = []
	while(1):
		
		LOCK.acquire()
		printBuff[:] = arrayShared[:]
		for i in range(0, len(arrayShared)):
			arrayShared[i] = 0
		LOCK.release()
		
		if(printBuff[4] > 0):
			print('')
			print("Numero de tentativas de leitura {:d}".format(cont[0]))
			print("ID (n Bytes): data")
			
			LOCK.acquire()
			cont[0] = 0
			LOCK.release()
			
		begin = 0
		end = 0
		while(True):
			if (printBuff[begin+4] > 0):
				end = begin + printBuff[begin+4] + 5
				
				ID = printBuff[begin] 
				ID = (ID << 8) + printBuff[begin+1]
				ID = (ID << 8) + printBuff[begin+2]
				ID = (ID << 8) + printBuff[begin+3]
				l = printBuff[begin+4]
				
				
				dR = np.uint64(0x0)
								
				if(l == 8):
					dR = np.sum([printBuff[begin+5+0] << 8*7, 
					    		 printBuff[begin+5+1] << 8*6,
						    	 printBuff[begin+5+2] << 8*5,
								 printBuff[begin+5+3] << 8*4,
								 printBuff[begin+5+4] << 8*3,
							     printBuff[begin+5+5] << 8*2,
							     printBuff[begin+5+6] << 8*1,
							     printBuff[begin+5+7] << 8*0], dtype = np.uint64)				
				else:
					for i in range(0,l):
						dR = np.sum([dR , (printBuff[begin+5+i] << 8*(l-i-1))], dtype = np.uint64)
				
				
				print("{:d} ({:d}): {}".format(ID, l, dR))
				arq = open(arqN, 'a')
				arq.write("{:d} ({:d}): {}\n".format(ID, l, dR))
				arq.close()
				
				begin = end
			
			if(printBuff[begin+4] == 0):
				printBuff = []
				break


def getBuff(device, arrayShared, LOCK, cont):
    readBuff = []
    cont[0] = 0
    while 1:
		if(device.inWaiting() > 4):
			while(device.inWaiting() > 0):
				readBuff.append(ord(device.read()))

		if(len(readBuff) > 4):
			LOCK.acquire()
			arrayShared[:len(readBuff)] = readBuff[:]  
			readBuff = []
			LOCK.release()
		
		LOCK.acquire()
		cont[0] = cont[0] + 1
		LOCK.release()
			
    


GETdata = mult.Process(target=getBuff, args=(mon, buff, lock, conter))
GETdata.start()

PRINTdata = mult.Process(target=printBuff, args=(buff, lock, conter))
PRINTdata.start()


