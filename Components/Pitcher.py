from Libraries.functions import *

myName = PitcherName

def p(msg='', *args, sep=None, end='\n'):
    if PitcherPrints:
        print(PitcherPrintTag, msg, *args, sep=sep, end=end)
    
def Pitcher_begin():
    p('Started')
    
    msg = ['Good', 'weather', 'today!']
    p('Sending...', msg)
    send_to_Receiver([myName, *msg])
