from Libraries.functions import *

myName = ReceiverName

def p(msg='', *args, sep=None, end='\n'):
    if ReceiverPrints:
        print(ReceiverPrintTag, msg, *args, sep=sep, end=end)


def Receiver_begin():
    p('Started!')
    
    while True:
        source_name, received_payload = listen(ReceiverAddr)
        p('Received message from ', source_name, ', contents: ', received_payload, sep='')
        send_to_Filter([myName, *received_payload])
