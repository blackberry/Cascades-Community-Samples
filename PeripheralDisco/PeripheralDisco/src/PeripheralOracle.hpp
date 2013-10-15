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
#ifndef PERIPHERALORACLE_HPP_
#define PERIPHERALORACLE_HPP_

#include <bps/bps.h>
#include <sys/siginfo.h>
#include <QObject>
#include <QMap>
#include <QSet>
#include <peripheral_discovery.h>

#include "PeripheralDetail.hpp"

class PeripheralOracle : public QObject {
	Q_OBJECT
public:
	PeripheralOracle(QObject * owner);
	virtual ~PeripheralOracle();

	Q_INVOKABLE void refresh();
	Q_INVOKABLE QSet<pd_class_t> registerInterest(const QSet<pd_class_t> &);
	Q_INVOKABLE QSet<pd_bus_t> getSupportedBusses();

   int signalHandler();

   QMap<int,PeripheralDetail> peripherals();

signals:
	void peripheralConnected(int peripheralId,PeripheralDetail details);
	void peripheralDisconnected(int peripheralId,PeripheralDetail details);

private:
	Q_INVOKABLE void toEmit(bool isConnect,int peripheralId,PeripheralDetail details);

	struct sigevent sigEvent;
	QMap<int,PeripheralDetail> details;
};

#endif /* PERIPHERALORACLE_HPP_ */
