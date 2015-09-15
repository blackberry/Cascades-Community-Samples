APP_NAME = TranslateHLService

CONFIG += qt warn_on

include(config.pri)

LIBS += -lbb -lbbsystem -lbbplatform

TRANSLATIONS = \
    $${TARGET}_fr.ts \
    $${TARGET}_en.ts \
    $${TARGET}_es.ts \
    $${TARGET}_it.ts \
    $${TARGET}.ts