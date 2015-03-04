TEMPLATE = lib

ANNUAL_LEAVE_COMMON = AnnualLeaveCommon

CONFIG += qt warn_on cascades_library
CONFIG += hardening

include(config.pri)

INCLUDEPATH += ../../$${ANNUAL_LEAVE_COMMON}/public
INCLUDEPATH += ../public

LIBS += -lbbdata
LIBS += -lbb -lbbsystem -lbbplatform -lbps -lbbpim

device {
    CONFIG(debug, debug|release) {
        # Device-Debug custom configuration
    	LIBS += -L../../$${ANNUAL_LEAVE_COMMON}/arm/so.le-v7-g -l$${ANNUAL_LEAVE_COMMON}
    }

    CONFIG(release, debug|release) {
        # Device-Release custom configuration
    	LIBS += -L../../$${ANNUAL_LEAVE_COMMON}/arm/so.le-v7 -l$${ANNUAL_LEAVE_COMMON}
    }

    CONFIG(debug, debug|profile) {
    	LIBS += -L../../$${ANNUAL_LEAVE_COMMON}/arm-p/so.le-v7-g -l$${ANNUAL_LEAVE_COMMON}
    }
}

simulator {
    CONFIG(debug, debug|release) {
        # Simulator-Debug custom configuration
    	LIBS += -L../../$${ANNUAL_LEAVE_COMMON}/x86/so-g -l$${ANNUAL_LEAVE_COMMON}
    }
}
