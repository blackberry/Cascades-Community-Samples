APP_NAME = AnnualLeaveService

ANNUAL_LEAVE_COMMON_PATH = AnnualLeaveCommon
ANNUAL_LEAVE_COMMON_NAME = AnnualLeaveCommon
ANNUAL_LEAVE_ADAPTER_PATH = AnnualLeaveAdapterEmail
ANNUAL_LEAVE_ADAPTER_NAME = AnnualLeaveAdapter

CONFIG += qt warn_on

INCLUDEPATH += ../../$${ANNUAL_LEAVE_COMMON_PATH}/public
INCLUDEPATH += ../../$${ANNUAL_LEAVE_ADAPTER_PATH}/public
INCLUDEPATH += ../public

include(config.pri)

LIBS += -lbb -lbbsystem -lbbplatform -lbps
LIBS += -lQtDeclarative -lQtScript -lQtSvg -lQtSql -lQtXmlPatterns -lQtNetwork -lQtXml

device {
    CONFIG(debug, debug|release) {
    	LIBS += -L../../$${ANNUAL_LEAVE_COMMON_PATH}/arm/so.le-v7-g -l$${ANNUAL_LEAVE_COMMON_NAME}
    	LIBS += -L../../$${ANNUAL_LEAVE_ADAPTER_PATH}/arm/so.le-v7-g -l$${ANNUAL_LEAVE_ADAPTER_NAME}
    }

    CONFIG(release, debug|release) {
    	LIBS += -L../../$${ANNUAL_LEAVE_COMMON_PATH}/arm/so.le-v7 -l$${ANNUAL_LEAVE_COMMON_NAME}
    	LIBS += -L../../$${ANNUAL_LEAVE_ADAPTER_PATH}/arm/so.le-v7 -l$${ANNUAL_LEAVE_ADAPTER_NAME}
    }

    CONFIG(debug, debug|profile) {
    	LIBS += -L../../$${ANNUAL_LEAVE_COMMON_PATH}/arm-p/so.le-v7-g -l$${ANNUAL_LEAVE_COMMON_NAME}
    	LIBS += -L../../$${ANNUAL_LEAVE_ADAPTER_PATH}/arm-p/so.le-v7-g -l$${ANNUAL_LEAVE_ADAPTER_NAME}
    }
}

simulator {
    CONFIG(debug, debug|release) {
    	LIBS += -L../../$${ANNUAL_LEAVE_COMMON_PATH}/x86/so-g -l$${ANNUAL_LEAVE_COMMON_NAME}
    	LIBS += -L../../$${ANNUAL_LEAVE_ADAPTER_PATH}/x86/so-g -l$${ANNUAL_LEAVE_ADAPTER_NAME}
    }
}
