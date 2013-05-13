APP_NAME = FlurryAnalyticsSample

CONFIG += qt warn_on cascades10
LIBS += -lscreen -lcrypto -lcurl -lpackageinfo -lbbdevice -lQtLocationSubset -lbb -lbbsystem

include(config.pri)

device {
    CONFIG(debug, debug|release) {
        # Device-Debug custom configuration
    }

    CONFIG(release, debug|release) {
        # Device-Release custom configuration
    }
}

simulator {
    CONFIG(debug, debug|release) {
        # Simulator-Debug custom configuration
    }
}
