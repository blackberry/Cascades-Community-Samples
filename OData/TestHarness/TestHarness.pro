APP_NAME = TestHarness

CONFIG += qt warn_on cascades10

include(config.pri)

device {
    CONFIG(debug, debug|release) {
        # Device-Debug custom configuration
		INCLUDEPATH += ../../OData/Device-Debug/public
		LIBS += -Bstatic -L../../OData/Device-Debug/ -lOData -Bdynamic
    }

    CONFIG(release, debug|release) {
        # Device-Release custom configuration
		INCLUDEPATH += ../../OData/Device-Release/public
		LIBS += -Bstatic -L../../OData/Device-Release/ -lOData -Bdynamic
    }
}

simulator {
    CONFIG(debug, debug|release) {
        # Simulator-Debug custom configuration
		INCLUDEPATH += ../../OData/Simulator-Debug/public
		LIBS += -Bstatic -L../../OData/Simulator-Debug/ -lOData -Bdynamic
    }
}

LIBS += -lbbdata