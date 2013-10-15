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

/**
 * This class is an oracle for the peripheral_discovery functionality.
 */
class PeripheralOracle : public QObject {
	Q_OBJECT
public:
	PeripheralOracle(QObject * owner);
	virtual ~PeripheralOracle();

	/**
	 * Register than you'd like to get information about the given classes.
	 *
	 * @return Classes that registration worked for.
	 */
	Q_INVOKABLE QSet<pd_class_t> registerInterest(const QSet<pd_class_t> &);

	/**
	 * Get the buses that this device supports.
	 */
	Q_INVOKABLE QSet<pd_bus_t> getSupportedBusses();

	/**
	 * Get a QMap of all currently known and connected peripherals.
	 */
	QMap<int,PeripheralDetail> peripherals();

signals:
	/**
	 * This signal fires when a peripheral is connected.
	 */
	void peripheralConnected(int peripheralId,PeripheralDetail details);

	/**
	 * Surprisingly this signal fires when a peripheral is disconnected.
	 */
	void peripheralDisconnected(int peripheralId,PeripheralDetail details);

private:
	/**
	 * OS signal handling can happen in any thread. This invokable is called
	 * so that Qt handles the threading for us.
	 */
	Q_INVOKABLE void toEmit(bool isConnect,int peripheralId,PeripheralDetail details);

	/**
	 * Helper to get from the callback to the instance.
	 */
	static int signalHandlerStatic(void *);

	/**
	 * Handles the OS signal.
	 */
    int signalHandler();

    /**
     * Structure holding how to call back from this function.
     */
	struct sigevent sigEvent;

	/**
	 * Details of the known and connected peripherals.
	 */
	QMap<int,PeripheralDetail> details;
};

#endif /* PERIPHERALORACLE_HPP_ */
