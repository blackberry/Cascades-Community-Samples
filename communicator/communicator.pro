APP_NAME = communicator

CONFIG += qt warn_on cascades10

include(config.pri)
LIBS += -lbb
LIBS += -lbbsystem
LIBS += -lbbdata
LIBS += -lOpenAL -lalut -lasound
LIBS += -lbbpim