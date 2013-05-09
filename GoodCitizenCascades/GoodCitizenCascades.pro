#
# Copyright (c) 2011, 2012 Research In Motion Limited.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

TEMPLATE = app
TARGET = GoodCitizenCascades

CONFIG += qt warn_on debug_and_release cascades10
LIBS   += -lpps -lscreen -lEGL -lGLESv1_CM -lfreetype -lpng  -lbb

INCLUDEPATH += ../src
SOURCES += ../src/*.cpp 
HEADERS += ../src/*.hpp ../src/*.h 

device {
	CONFIG(release, debug|release) {		
		DESTDIR = o.le-v7		
		TEMPLATE=lib
		QMAKE_CXXFLAGS += -fvisibility=hidden
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

