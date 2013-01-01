TEMPLATE = app
TARGET = pjsipdemo

CONFIG += qt warn_on debug_and_release cascades

INCLUDEPATH += ../src
SOURCES += ../src/*.cpp
HEADERS += ../src/*.hpp ../src/*.h 

DEFINES += PJ_AUTOCONF

INCLUDEPATH += ${QNX_TARGET}/usr/include/pjsip/include ${QNX_TARGET}/usr/include/pjlib/include ${QNX_TARGET}/usr/include/pjlib-util/include ${QNX_TARGET}/usr/include/pjnath/include ${QNX_TARGET}/usr/include/pjmedia/include

LIBS += -lOpenAL -lalut -laudio_manager -lsocket -lasound -lbbsystem -L${QNX_TARGET}/target/qnx6/${CPUVARDIR}/lib

lupdate_inclusion {
    SOURCES += ../assets/*.qml
}

device {
	CONFIG(release, debug|release) {
		LIBS += -lpjsua-arm-unknown-nto-qnx8.0.0eabi -lpjsip-ua-arm-unknown-nto-qnx8.0.0eabi -lpjsip-simple-arm-unknown-nto-qnx8.0.0eabi -lpjsip-arm-unknown-nto-qnx8.0.0eabi -lpjmedia-codec-arm-unknown-nto-qnx8.0.0eabi
		LIBS += -lpjmedia-videodev-arm-unknown-nto-qnx8.0.0eabi -lpjmedia-arm-unknown-nto-qnx8.0.0eabi -lpjmedia-audiodev-arm-unknown-nto-qnx8.0.0eabi -lspeex-arm-unknown-nto-qnx8.0.0eabi -lopencore-amrnb
		LIBS += -lgsmcodec-arm-unknown-nto-qnx8.0.0eabi -lilbccodec-arm-unknown-nto-qnx8.0.0eabi -lg7221codec-arm-unknown-nto-qnx8.0.0eabi -lpjnath-arm-unknown-nto-qnx8.0.0eabi -lpjlib-util-arm-unknown-nto-qnx8.0.0eabi
		LIBS += -lresample-arm-unknown-nto-qnx8.0.0eabi -lmilenage-arm-unknown-nto-qnx8.0.0eabi -lsrtp-arm-unknown-nto-qnx8.0.0eabi -lportaudio-arm-unknown-nto-qnx8.0.0eabi -lpj-arm-unknown-nto-qnx8.0.0eabi -lpjsdp-arm-unknown-nto-qnx8.0.0eabi
		DESTDIR = o.le-v7
	}
	CONFIG(debug, debug|release) {
		LIBS += -lpjsua-arm-unknown-nto-qnx8.0.0eabi -lpjsip-ua-arm-unknown-nto-qnx8.0.0eabi -lpjsip-simple-arm-unknown-nto-qnx8.0.0eabi -lpjsip-arm-unknown-nto-qnx8.0.0eabi -lpjmedia-codec-arm-unknown-nto-qnx8.0.0eabi
		LIBS += -lpjmedia-videodev-arm-unknown-nto-qnx8.0.0eabi -lpjmedia-arm-unknown-nto-qnx8.0.0eabi -lpjmedia-audiodev-arm-unknown-nto-qnx8.0.0eabi -lspeex-arm-unknown-nto-qnx8.0.0eabi -lopencore-amrnb
		LIBS += -lgsmcodec-arm-unknown-nto-qnx8.0.0eabi -lilbccodec-arm-unknown-nto-qnx8.0.0eabi -lg7221codec-arm-unknown-nto-qnx8.0.0eabi -lpjnath-arm-unknown-nto-qnx8.0.0eabi -lpjlib-util-arm-unknown-nto-qnx8.0.0eabi
		LIBS += -lresample-arm-unknown-nto-qnx8.0.0eabi -lmilenage-arm-unknown-nto-qnx8.0.0eabi -lsrtp-arm-unknown-nto-qnx8.0.0eabi -lportaudio-arm-unknown-nto-qnx8.0.0eabi -lpj-arm-unknown-nto-qnx8.0.0eabi -lpjsdp-arm-unknown-nto-qnx8.0.0eabi		
		DESTDIR = o.le-v7-g
	}
}

simulator {
	CONFIG(release, debug|release) {
		DESTDIR = o
	}
	CONFIG(debug, debug|release) {
		LIBS += -lpjsua-i486-pc-nto-qnx8.0.0 -lpjsip-ua-i486-pc-nto-qnx8.0.0 -lpjsip-simple-i486-pc-nto-qnx8.0.0 -lpjsip-i486-pc-nto-qnx8.0.0 -lpjmedia-codec-i486-pc-nto-qnx8.0.0
		LIBS += -lpjmedia-videodev-i486-pc-nto-qnx8.0.0 -lpjmedia-i486-pc-nto-qnx8.0.0 -lpjmedia-audiodev-i486-pc-nto-qnx8.0.0 -lspeex-i486-pc-nto-qnx8.0.0 -lopencore-amrnb
		LIBS += -lgsmcodec-i486-pc-nto-qnx8.0.0 -lilbccodec-i486-pc-nto-qnx8.0.0 -lg7221codec-i486-pc-nto-qnx8.0.0 -lpjnath-i486-pc-nto-qnx8.0.0 -lpjlib-util-i486-pc-nto-qnx8.0.0
		LIBS += -lresample-i486-pc-nto-qnx8.0.0 -lmilenage-i486-pc-nto-qnx8.0.0 -lsrtp-i486-pc-nto-qnx8.0.0 -lportaudio-i486-pc-nto-qnx8.0.0 -lpj-i486-pc-nto-qnx8.0.0 -lpjsdp-i486-pc-nto-qnx8.0.0
		DESTDIR = o-g
	}
}

OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR = $${DESTDIR}/.moc
RCC_DIR = $${DESTDIR}/.rcc
UI_DIR = $${DESTDIR}/.ui

suredelete.target = sureclean
suredelete.commands = $(DEL_FILE) $${MOC_DIR}/*; $(DEL_FILE) $${RCC_DIR}/*; $(DEL_FILE) $${UI_DIR}/*
suredelete.depends = distclean

QMAKE_EXTRA_TARGETS += suredelete

TRANSLATIONS = \
    $${TARGET}_en_GB.ts \
    $${TARGET}_fr.ts \
    $${TARGET}_it.ts \    
    $${TARGET}_de.ts \
    $${TARGET}_es.ts \
    $${TARGET}.ts

