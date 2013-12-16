APP_NAME = ID3TestApp

CONFIG += qt warn_on cascades10

INCLUDEPATH += ../include

LIBS += -L../lib -lid3 -lz -linput_utilities -linput_client -lcpp

include(config.pri)

QMAKE_CXXFLAGS += -fPIC

device {
    CONFIG(debug, debug|release) {
        # Device-Debug custom configuration
        LIB_SOURCE = ..\\lib_versions\\arm-qnx\\libid3.a
        LIB_TARGET = ..\\lib
        QMAKE_PRE_LINK += $$quote(cmd /c copy /y $${LIB_SOURCE} $${LIB_TARGET})$$escape_expand(\\n\\t)
    }
    CONFIG(release, debug|release) {
        # Device-Release custom configuration
        LIB_SOURCE = ..\\lib_versions\\arm-qnx\\libid3.a
        LIB_TARGET = ..\\lib
        QMAKE_PRE_LINK += $$quote(cmd /c copy /y $${LIB_SOURCE} $${LIB_TARGET})$$escape_expand(\\n\\t)
     }
}

simulator {
    CONFIG(debug, debug|release) {
        # Simulator-Debug custom configuration
        LIB_SOURCE = ..\\lib_versions\\x86-qnx\\libid3.a
        LIB_TARGET = ..\\lib
        QMAKE_PRE_LINK += $$quote(cmd /c copy /y $${LIB_SOURCE} $${LIB_TARGET})$$escape_expand(\\n\\t)
    }
}
