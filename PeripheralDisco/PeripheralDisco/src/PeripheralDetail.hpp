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
#ifndef PERIPHERALDETAIL_HPP_
#define PERIPHERALDETAIL_HPP_

#include <QObject>
#include <QMap>
#include <QVariant>
#include <peripheral_discovery.h>

/**
 * Essentially a QMap of properties and values - with some extra functionality to ease management.
 */
class PeripheralDetail {
public:
	/**
	 * Build from the low level data.
	 */
	PeripheralDetail(pd_peripheral_t *);
	/**
	 * Build a copy.
	 */
	PeripheralDetail(const PeripheralDetail &);
	/**
	 * Default constructor.
	 */
	PeripheralDetail();

	virtual ~PeripheralDetail();

	/**
	 * Make a pretty listing in HTML.
	 */
	QString toHTML();
	/**
	 * Make a not-so-pretty listing in text.
	 */
	QString toString();

	/**
	 * Check if this peripheral is a USB serial device.
	 */
	bool isUSBSerial();

	/**
	 * Get a specific value from the details map.
	 */
	QVariant getValue(QString name);

private:
	/**
	 * Convert to HTML, or a String.
	 */
	QString toSomething(bool isHTML);

	/**
	 * The peripheral's details.
	 */
	QMap<QString, QVariant> details;
};

#endif /* PERIPHERALDETAIL_HPP_ */
