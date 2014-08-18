APP_NAME = hostcardemulation

CONFIG += qt warn_on cascades10

include(config.pri)

LIBS += -lbbsystem -lbb -lnfc -lnfc_bps 

#LIBS += -lbbplatform
#LIBS += -lbtapi -lbbcascades -lQtNetwork -lbbdata
#LIBS += -lbps -lQtNfcSubset -lbbsystem -lbb