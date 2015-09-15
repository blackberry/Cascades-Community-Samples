APP_NAME = AnnualLeave

ANNUAL_LEAVE_COMMON = AnnualLeaveCommon
ANNUAL_LEAVE_MODEL = AnnualLeaveModel

CONFIG += qt warn_on cascades10

INCLUDEPATH += ../../$${ANNUAL_LEAVE_COMMON}/public
INCLUDEPATH += ../../$${ANNUAL_LEAVE_MODEL}/public

include(config.pri)

LIBS += -lbb -lbbsystem -lhuapi -lbbdata -lbbpim

device {
    CONFIG(debug, debug|release) {
    	LIBS += -L../../$${ANNUAL_LEAVE_COMMON}/arm/so.le-v7-g -l$${ANNUAL_LEAVE_COMMON}
    	LIBS += -L../../$${ANNUAL_LEAVE_MODEL}/arm/so.le-v7-g -l$${ANNUAL_LEAVE_MODEL}
    }

    CONFIG(release, debug|release) {
    	LIBS += -L../../$${ANNUAL_LEAVE_COMMON}/arm/so.le-v7 -l$${ANNUAL_LEAVE_COMMON}
    	LIBS += -L../../$${ANNUAL_LEAVE_MODEL}/arm/so.le-v7 -l$${ANNUAL_LEAVE_MODEL}
    }

    CONFIG(debug, debug|profile) {
    	LIBS += -L../../$${ANNUAL_LEAVE_COMMON}/arm-p/so.le-v7-g -l$${ANNUAL_LEAVE_COMMON}
    	LIBS += -L../../$${ANNUAL_LEAVE_MODEL}/arm-p/so.le-v7-g -l$${ANNUAL_LEAVE_MODEL}
    }
}

simulator {
    CONFIG(debug, debug|release) {
    	LIBS += -L../../$${ANNUAL_LEAVE_COMMON}/x86/so-g -l$${ANNUAL_LEAVE_COMMON}
    	LIBS += -L../../$${ANNUAL_LEAVE_MODEL}/x86/so-g -l$${ANNUAL_LEAVE_MODEL}
    }
}
