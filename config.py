
PitcherName = 'Pitcher'
ReceiverName = 'Receiver'
FilterName = 'Filter'
DataAnalyticsName = 'Data Analytics'
FlagName = 'Flag'

# PitcherAddr = ('127.0.0.5', 5555)
ReceiverAddr = ('127.0.0.4', 4444)
FilterAddr = ('127.0.0.3', 3333)
DataAnalyticsAddr = ('127.0.0.2', 2222)
FlagAddr = ('127.0.0.1', 1111)

#   constants for Filter and Data Analytics communications
NEW_RULE = 'new rule'
REMOVE_RULE = 'remove rule'
NO_CHANGES_IN_RULES = 'no changes in rules'
BLOCK_WORDS_THAT_START_WITH = 'block words that start with'

PitcherPrints = False
ReceiverPrints = False
FilterPrints = False
DataAnalyticsPrints = False
FlagPrints = False

PitcherPrints = True
ReceiverPrints = True
FilterPrints = True
DataAnalyticsPrints = True
FlagPrints = True

PitcherPrintTag = '\tPitcher' + (' ' + '-' * 5 + ' ') * 5 + 'Pitcher: '
ReceiverPrintTag = '\tReceiver' + (' ' + '-' * 4 + ' ') * 4 + 'Receiver: '
FilterPrintTag = '\tFilter' + (' ' + '-' * 3 + ' ') * 3 + 'Filter: '
DataAnalyticsPrintTag = '\tData Analytics' + (' ' + '-' * 2 + ' ') * 2 + 'Data Analytics: '
FlagPrintTag = '\tFlag - Flag: '
