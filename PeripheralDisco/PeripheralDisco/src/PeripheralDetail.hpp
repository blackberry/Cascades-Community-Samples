/*
 * PeripheralDetail.hpp
 *
 *  Created on: 2013-08-30
 *      Author: robwilliams
 */

#ifndef PERIPHERALDETAIL_HPP_
#define PERIPHERALDETAIL_HPP_

#include <QObject>
#include <QMap>
#include <QVariant>
#include <peripheral_discovery.h>

class PeripheralDetail {
public:
	PeripheralDetail(pd_peripheral_t *);
	PeripheralDetail(const PeripheralDetail &);
	PeripheralDetail();
	virtual ~PeripheralDetail();

	QString toHTML();
	QString toString();

	bool isUSBSerial();

	QVariant getValue(QString name);

private:
	QString toSomething(bool isHTML);
	QMap<QString, QVariant> details;
};

#endif /* PERIPHERALDETAIL_HPP_ */
