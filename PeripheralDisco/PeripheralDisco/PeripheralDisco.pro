APP_NAME = PeripheralDisco

CONFIG += qt warn_on cascades10

include(config.pri)

LIBS += -lperipheral_discovery -lbb -lbbsystem
