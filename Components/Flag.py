from Libraries.functions import *


def p(msg='', *args, sep=None, end='\n'):
    if FlagPrints:
        print(FlagPrintTag, msg, *args, sep=sep, end=end)

    
def Flag_begin():
    p('Started!')

    while True:
        source_name, received_payload = listen(FlagAddr)
        p('Received message from ', source_name, ', contents: ', received_payload, sep='')
