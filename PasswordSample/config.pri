# Auto-generated by IDE. All changes by user will be lost!
# Created at 13/02/13 15:18

BASEDIR = $$_PRO_FILE_PWD_

INCLUDEPATH +=  \
    $$BASEDIR/src

SOURCES +=  \
    $$BASEDIR/src/DataManager.cpp \
    $$BASEDIR/src/PasswordSample.cpp \
    $$BASEDIR/src/SecurityManager.cpp \
    $$BASEDIR/src/Timer.cpp \
    $$BASEDIR/src/Utilities.cpp \
    $$BASEDIR/src/main.cpp

HEADERS +=  \
    $$BASEDIR/src/DataManager.hpp \
    $$BASEDIR/src/PasswordSample.hpp \
    $$BASEDIR/src/SecurityManager.hpp \
    $$BASEDIR/src/Timer.hpp \
    $$BASEDIR/src/Utilities.hpp

CONFIG += precompile_header
PRECOMPILED_HEADER = $$BASEDIR/precompiled.h

lupdate_inclusion {
    SOURCES += \
        $$BASEDIR/../assets/*.qml
}

TRANSLATIONS = \
    $${TARGET}.ts

