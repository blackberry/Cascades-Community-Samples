#! /usr/bin/env python

from smartcard.CardType import AnyCardType
from smartcard.CardRequest import CardRequest
from smartcard.CardConnection import CardConnection
from smartcard.CardConnectionObserver import ConsoleCardConnectionObserver
from smartcard.Exceptions import CardRequestTimeoutException
from smartcard.util import toHexString
from smartcard.util import toASCIIString

COMMAND_HELLO_POS = [0xa0, 0x37, 0x00, 0x00, 0x01, 0x99, 0x00]

cardtype = AnyCardType()

try:
    print 'insert a card within 10s'
    cardrequest = CardRequest(timeout=10, cardType=cardtype)
    cardservice = cardrequest.waitforcard()
    observer = ConsoleCardConnectionObserver()
    cardservice.connection.addObserver(observer)
    cardservice.connection.connect()

    response, sw1, sw2 = cardservice.connection.transmit(COMMAND_HELLO_POS)
    if sw1 == 0x90 and sw2 == 0x00:
        print toASCIIString(response)
    else:
        print 'bad response'
except CardRequestTimeoutException:
    print 'time-out: no card inserted during last 10s'
except:
    import sys
    print sys.exc_info()[1]
