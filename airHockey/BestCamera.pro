APP_NAME = BestCamera

CONFIG += qt warn_on cascades10 mobility
MOBILITY += sensors

LIBS += -lcamapi -lscreen -lbtapi -lbbsystem
LIBS += -lnfc -lnfc_bps -lbb -lQtNfcSubset

QMAKE_CXXFLAGS_RELEASE += -O3

QMAKE_CXXFLAGS_DEBUG += -O3 -mtune=cortex-a9 -mfloat-abi=softfp -mfpu=neon

include(config.pri)
