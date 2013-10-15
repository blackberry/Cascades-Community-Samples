/*
 * PeripheralOracle.cpp
 *
 *  Created on: 2013-08-30
 *      Author: robwilliams
 */

#include "PeripheralOracle.hpp"
#include "PeripheralDetail.hpp"

#include <QDebug>
#include <QThread>

#include <errno.h>

static int peripheralOracleSignalHandler(void * data) {
	return ((PeripheralOracle*) data)->signalHandler();
}

PeripheralOracle::PeripheralOracle(QObject * owner) :
		QObject(owner) {
	bps_initialize();
	pd_initialize(0);

	bps_add_sigevent_handler(&sigEvent, peripheralOracleSignalHandler, this);
}

PeripheralOracle::~PeripheralOracle() {
	pd_uninitialize();
	bps_shutdown();
}

bool PeripheralOracle::registerInterest(pd_class_t klass) {
	int min = klass;
	int max = klass;
	if (min == -1) {
		min = 0;
	}
	if (max == -1) {
		max = PD_CLASS_NUM_SUPPORTED - 1;
	}
	++max;

	bool toReturn = false;
	bool result = false;

	for (int i = min; i < max; ++i) {
		qDebug() << "Attempting to register" << i;
		klass = (pd_class_t) i;

		if (pd_is_class_supported(klass, &result) == EOK) {
			if (result) {
				if (pd_register_event(klass, &sigEvent) == EOK) {
					qDebug() << "Good";
					toReturn |= true;
				} else {
					qDebug() << "Could not register";
				}
			} else {
				qDebug() << "Class is not supported";
			}
		} else {
			qDebug() << "Could not even check if class is supported";
		}
	}
	return toReturn;
}

//// Cleans up a bps event for peripheral discovery
//static void pd_bps_completion(bps_event_t *event) {
//	fprintf(stderr, "Cleaning up an event\n");
//	bps_event_payload_t *evtPayload = bps_event_get_payload(event);
//	pd_free_peripheral((pd_peripheral_t**)&(evtPayload->data1));
//	free( (int*)(evtPayload->data2) );
//	bps_event_destroy(event);
//}

int PeripheralOracle::signalHandler() {
	qDebug() << "In signal handler" << (int) QThread::currentThreadId();

	pd_peripheral_t *peripheral = pd_alloc_peripheral();

	int peripheralId;
	pd_event_type_t type;

	if (pd_get_event(&type, &peripheralId, peripheral) == EOK) {
		qDebug() << "Event type,id" << (int) type << peripheralId;
		if (type==    PD_EVENT_INSERTION) {
			qDebug() << "Emitting insertion";
			PeripheralDetail detail(peripheral);
			details[peripheralId] = detail;
			QMetaObject::invokeMethod(this,"toEmit",Qt::QueuedConnection,Q_ARG(bool, true),Q_ARG(int,peripheralId),Q_ARG(PeripheralDetail,detail));
		} else if (type==PD_EVENT_REMOVAL){
			qDebug() << "Emiting removal";
			if (details.contains(peripheralId)) {
				QMetaObject::invokeMethod(this,"toEmit",Qt::QueuedConnection,Q_ARG(bool, false),Q_ARG(int,peripheralId),Q_ARG(PeripheralDetail,details[peripheralId]));
			} else {
				QMetaObject::invokeMethod(this,"toEmit",Qt::QueuedConnection,Q_ARG(bool, false),Q_ARG(int,peripheralId),Q_ARG(PeripheralDetail,PeripheralDetail()));
			}
			details.remove(peripheralId);
		} else {
			qDebug() << "Unknown pd event type" << type;
		}
	} else {
		qDebug() << "pd_get_event failed";
	}

	pd_free_peripheral(&peripheral);
	return 0;
}

void PeripheralOracle::refresh() {
	qDebug() << "refresh";
}

void PeripheralOracle::toEmit(bool isConnect, int peripheralId, PeripheralDetail detail) {
	qDebug() << "toEmit";
	if (isConnect) {
		emit peripheralConnected(peripheralId,detail);
	} else {
		emit peripheralDisconnected(peripheralId,detail);
	}
}

QMap<int,PeripheralDetail> PeripheralOracle::peripherals() {
	return details;
}
