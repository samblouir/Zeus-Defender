from Libraries.functions import *

myName = DataAnalyticsName

def p(msg='', *args, sep=None, end='\n'):
    if DataAnalyticsPrints:
        print(DataAnalyticsPrintTag, msg, *args, sep=sep, end=end)


def DataAnalytics_begin():
    p('Started!')

    while True:
        source_name, received_payload = listen(DataAnalyticsAddr)
        p('Received message from ', source_name, ', contents: ', received_payload, sep='')
        
        p('Thinking...')
        updates = [NEW_RULE, 't', 'w']
        
        p('Generated new rules, sending to filter...')
        send_to_Filter([myName, *updates])
        
        p('Rule adjustments sent! Sending payload to filter...')
        send_to_Filter([myName, *received_payload])
