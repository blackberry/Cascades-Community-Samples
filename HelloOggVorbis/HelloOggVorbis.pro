TEMPLATE = app
TARGET = hellooggvorbis

CONFIG += qt warn_on debug_and_release cascades
LIBS   += -lOpenAL -lalut -L../external-lib/oggvorbis/lib/qnx/arm/ -L../external-lib/oggvorbis/lib/qnx/x86/ -lvorbis

INCLUDEPATH += ../src ../external-lib/oggvorbis/include
SOURCES += ../src/*.cpp 
HEADERS += ../src/*.h

device {
	CONFIG(release, debug|release) {		
		DESTDIR = o.le-v7		
	} 
	CONFIG(debug, debug|release) {
		DESTDIR = o.le-v7-g
	}
}

simulator {
	CONFIG(release, debug|release) {
		DESTDIR = o
	} 
	CONFIG(debug, debug|release) {
		DESTDIR = o-g
	}
}

OBJECTS_DIR = $${DESTDIR}/.obj
MOC_DIR = $${DESTDIR}/.moc
RCC_DIR = $${DESTDIR}/.rcc
UI_DIR = $${DESTDIR}/.ui

