APP_NAME = AnnualLeave

ANNUAL_LEAVE_COMMON = AnnualLeaveCommon

CONFIG += qt warn_on cascades10

INCLUDEPATH += ../../$${ANNUAL_LEAVE_COMMON}/public

include(config.pri)

LIBS += -lbb -lbbsystem -lhuapi -lbbdata -lbbpim

device {
    CONFIG(debug, debug|release) {
    	LIBS += -L../../$${ANNUAL_LEAVE_COMMON}/arm/so.le-v7-g -l$${ANNUAL_LEAVE_COMMON}
    }

    CONFIG(release, debug|release) {
    	LIBS += -L../../$${ANNUAL_LEAVE_COMMON}/arm/so.le-v7 -l$${ANNUAL_LEAVE_COMMON}
    }

    CONFIG(debug, debug|profile) {
    	LIBS += -L../../$${ANNUAL_LEAVE_COMMON}/arm-p/so.le-v7-g -l$${ANNUAL_LEAVE_COMMON}
    }
}

simulator {
    CONFIG(debug, debug|release) {
    	LIBS += -L../../$${ANNUAL_LEAVE_COMMON}/x86/so-g -l$${ANNUAL_LEAVE_COMMON}
    }
}
