TEMPLATE = lib

CONFIG += qt warn_on cascades_library
CONFIG += hardening

include(config.pri)

LIBS += -lbbdata
LIBS += -lbb -lbbsystem -lbbplatform -lbps -lbbutility

device {
    CONFIG(debug, debug|release) {
        # Device-Debug custom configuration
    }

    CONFIG(release, debug|release) {
        # Device-Release custom configuration
    }

    CONFIG(debug, debug|profile) {
        # Device-Profile custom configuration
    }
}

simulator {
    CONFIG(debug, debug|release) {
        # Simulator-Debug custom configuration
    }
}
