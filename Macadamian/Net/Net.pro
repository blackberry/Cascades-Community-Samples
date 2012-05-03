TEMPLATE = app
TARGET = Net

CONFIG += qt warn_on debug_and_release cascades
LIBS += -lpps -lscreen -lEGL -lGLESv1_CM -lfreetype -lpng -lcamapi

INCLUDEPATH += ../src
SOURCES += ../src/*.cpp  ../src/openGL/*.cpp
HEADERS += ../src/*.hpp ../src/*.h  ../src/openGL/*.hpp ../src/openGL/*.h

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

