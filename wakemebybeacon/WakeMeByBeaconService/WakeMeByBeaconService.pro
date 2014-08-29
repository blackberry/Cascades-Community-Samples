APP_NAME = WakeMeByBeaconService

CONFIG += qt warn_on

include(config.pri)

LIBS += -lbb -lbbsystem -lbbplatform
LIBS += -lbtapi -lbbcascades -lQtNetwork -lbbdata
