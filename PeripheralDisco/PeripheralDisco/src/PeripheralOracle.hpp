/*
 * PeripheralOracle.hpp
 *
 *  Created on: 2013-08-30
 *      Author: robwilliams
 */

#ifndef PERIPHERALORACLE_HPP_
#define PERIPHERALORACLE_HPP_

#include <bps/bps.h>
#include <sys/siginfo.h>
#include <QObject>
#include <QMap>
#include <peripheral_discovery.h>

#include "PeripheralDetail.hpp"

class PeripheralOracle : public QObject {
	Q_OBJECT
public:
	PeripheralOracle(QObject * owner);
	virtual ~PeripheralOracle();

	Q_INVOKABLE void refresh();
	Q_INVOKABLE bool registerInterest(pd_class_t klass);

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
