import serial
import multiprocessing
from multiprocessing import Process


arduino = serial.Serial('/dev/ttyUSB0', 9600)

buff=[]
for i in range(100):
	buff.append(0)
	
buffIN =  multiprocessing.Array('i', buff)
lock = multiprocessing.Lock() 	

def serialRead(buffIN, lock):
	
	while(1):
		lock.acquire()
		if(arduino.inWaiting() > 4):
			i = 0
			while(arduino.inWaiting() > 0):
				buffIN[i] = ord(arduino.read())
				i = i+1
		
		lock.release()
			
	


def serialPrint(buffIN, lock):
	
	while(True):
		lock.acquire()
		if (buffIN[4] != 0):
			print(buffIN[:buffIN[4]+5])
			buffIN[4] = 0
		lock.release()
			
		
	


if __name__=="__main__":

	
	Read = Process(target=serialRead, args=(buffIN,lock))
	
	Print = Process(target=serialPrint, args=(buffIN,lock))

	Read.start()
	Print.start()

	
	Read.join() 
	Print.join()
