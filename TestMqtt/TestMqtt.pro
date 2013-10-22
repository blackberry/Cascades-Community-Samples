APP_NAME = TestMqtt

MQTT_PROJECT=Mqtt

CONFIG += qt warn_on cascades10

INCLUDEPATH += ../../$${MQTT_PROJECT}/public

LIBS += -lsocket
LIBS += -L../assets/lib -lMqtt

include(config.pri)

device {
    CONFIG(debug, debug|release) {
        # Device-Debug custom configuration
        LIB_MQTT_SOURCE = ..\\..\\$${MQTT_PROJECT}\\Device-Debug\\libMqtt.so
        LIB_MQTT_TARGET = ..\\assets\\lib
        QMAKE_PRE_LINK += $$quote(cmd /c copy /y $${LIB_MQTT_SOURCE} $${LIB_MQTT_TARGET})$$escape_expand(\\n\\t)
    }
    CONFIG(release, debug|release) {
        # Device-Release custom configuration
        LIB_MQTT_SOURCE = ..\\..\\$${MQTT_PROJECT}\\Device-Release\\libMqtt.so
        LIB_MQTT_TARGET = ..\\assets\\lib
        QMAKE_PRE_LINK += $$quote(cmd /c copy /y $${LIB_MQTT_SOURCE} $${LIB_MQTT_TARGET})$$escape_expand(\\n\\t)
    }
}

simulator {
    CONFIG(debug, debug|release) {
        # Simulator-Debug custom configuration
        LIB_MQTT_SOURCE = ..\\..\\$${MQTT_PROJECT}\\Simulator-Debug\\libMqtt.so
        LIB_MQTT_TARGET = ..\\assets\\lib
        QMAKE_PRE_LINK += $$quote(cmd /c copy /y $${LIB_MQTT_SOURCE} $${LIB_MQTT_TARGET})$$escape_expand(\\n\\t)
    }
}
