import socket
from multiprocessing import Process

from Components.Pitcher import Pitcher_begin
from Components.Receiver import Receiver_begin
from Components.Filter import Filter_begin
from Components.DataAnalytics import DataAnalytics_begin
from Components.Flag import Flag_begin
from Libraries.functions import setSocketType

if __name__ == '__main__':
	
	try:
		hasattr(socket, socket.AF_UNIX)
		setSocketType(UDS_instead_of_IPV4=True)
	except:
		print('UDS failed to work. Using IPv4...')
		setSocketType(UDS_instead_of_IPV4=False)
	
	Pitcher = Process(target=Pitcher_begin).start()
	Receiver = Process(target=Receiver_begin).start()
	Filter = Process(target=Filter_begin).start()
	DataAnalytics = Process(target=DataAnalytics_begin).start()
	Flag = Process(target=Flag_begin).start()
	print('All processes started')
