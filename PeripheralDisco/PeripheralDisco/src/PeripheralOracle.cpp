/**
 * Copyright (c) 2013 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "PeripheralOracle.hpp"
#include "PeripheralDetail.hpp"

#include <QDebug>
#include <QThread>

#include <errno.h>

PeripheralOracle::PeripheralOracle(QObject * owner) :
		QObject(owner) {
	bps_initialize();
	pd_initialize(0);

	bps_add_sigevent_handler(&sigEvent, signalHandlerStatic, this);
}

PeripheralOracle::~PeripheralOracle() {
	pd_uninitialize();
	bps_shutdown();
}

QSet<pd_bus_t> PeripheralOracle::getSupportedBusses() {
	QSet<pd_bus_t> toReturn;

	// Check if all the known busses are supported.
	for (int i=0; i<PD_BUS_NUM_SUPPORTED; ++i) {
		qDebug() << "Checking support for bus" << i;
		pd_bus_t bus = (pd_bus_t)i;
		bool result = false;
		if (pd_is_bus_supported(bus,&result)==EOK) {
			if (result) {
				qDebug() << "Supported";
				toReturn << bus;
			} else {
				qDebug() << "Not supported";
			}
		} else {
			qDebug() << "Could not even check if bus is supported...";
		}
	}

	return toReturn;
}

QSet<pd_class_t> PeripheralOracle::registerInterest(const QSet<pd_class_t> & klasses) {
	QSet<pd_class_t> toReturn;

	for (QSet<pd_class_t>::const_iterator i = klasses.begin(); i!=klasses.end(); ++i) {
		pd_class_t klass = *i;
		qDebug() << "Attempting to register" << (int)klass;

		bool result = false;

		if (pd_is_class_supported(klass, &result) == EOK) {
			if (result) {
				if (pd_register_event(klass, &sigEvent) == EOK) {
					qDebug() << "Good";
					toReturn << klass;
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

int PeripheralOracle::signalHandlerStatic(void * data) {
	return ((PeripheralOracle*) data)->signalHandler();
}

int PeripheralOracle::signalHandler() {
	qDebug() << "In signal handler" << (int) QThread::currentThreadId();

	pd_peripheral_t *peripheral = pd_alloc_peripheral();

	if (peripheral!=NULL) {

		int peripheralId;
		pd_event_type_t type;

		// Which thread does this happen in? It's a signal handler - it could be any thread.
		// That's why QMetaObject::invokeMethod is called. Let Qt do the hard work.

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
	} else {
		qDebug() << "Could not allocate peripheral!?!?";
	}
	return 0;
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
