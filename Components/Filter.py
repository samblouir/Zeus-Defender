from Libraries.functions import *

myName = FilterName
rules = []


def p(msg='', *args, sep=None, end='\n'):
    if FilterPrints:
        print(FilterPrintTag, msg, *args, sep=sep, end=end)


def Filter_begin():
    p('Started!')
    
    while True:
        source_name, received_payload = listen(FilterAddr)
        p('Received message from ', source_name, ', contents: ', received_payload, sep='')
        doWork(source_name, received_payload)


def filter_and_forward(data):
    p('filter_and_forward called with ', data)
    for rule in rules:
        p('Filtering with ', rule, '...')
        for step in rule:
            for each in data:
                if each[0] == step:
                    p('\tFiltering \'', each, '\'', sep='')
                    data.remove(each)
    
    p('Sending remaining data to the Flag...')
    send_to_Flag([myName, *data])


def doWork(source_name, contents):
    if source_name == ReceiverName:
        send_to_DataAnalytics([FilterName, *contents])
        return
    
    if source_name == DataAnalyticsName:
        if contents[0] == NEW_RULE:
            rules_to_add = contents[1:]
            rules.append(rules_to_add)
            p('Added new rules:', *rules_to_add)
            p('Current rules:', *rules)
            return
        elif contents[0] == REMOVE_RULE:
            rules_to_remove = contents[1:]
            rules.remove(rules_to_remove)
            p('Removed rules:', *rules_to_remove)
            p('Current rules:' *rules)
            return
        elif contents[0] == NO_CHANGES_IN_RULES:
            p('No changes in rules!')
            p('Current rules:' *rules)
            return
        
        filter_and_forward(contents)
