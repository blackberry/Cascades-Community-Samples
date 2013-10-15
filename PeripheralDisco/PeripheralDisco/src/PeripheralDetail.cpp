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
#include "PeripheralDetail.hpp"
#include <QDebug>
#include <QStringList>
#include <errno.h>

PeripheralDetail::PeripheralDetail() {
}

PeripheralDetail::PeripheralDetail(const PeripheralDetail & p) {
	details = p.details;
}

PeripheralDetail::PeripheralDetail(pd_peripheral_t * peripheral) {
	pd_peripheral_properties_t *properties;
	pd_peripheral_property_t *property;
	pd_property_type_t type;
	const char *name;
	const char *strvalue;
	int intval;

	if ((properties = pd_alloc_property_list()) == NULL) {
		qDebug() << "Couldn't allocate properties";
		return;
	}

	if (pd_get_peripheral_properties(peripheral, properties) == EOK) {
		while (pd_get_next_property(properties, &property) == EOK) {
			type = pd_get_property_type(property);
			switch (type) {
			case PD_STRING_TYPE:
				pd_get_property_as_string(property, &name, &strvalue);
				details[name] = strvalue;
				break;
			case PD_INTEGER_TYPE:
				pd_get_property_as_integer(property, &name, &intval);
				details[name] = intval;
				break;
			}
		}
		qDebug() << details;
	} else {
		printf("Couldn't get properties\n");
	}

	pd_free_property_list(&properties);
}

PeripheralDetail::~PeripheralDetail() {
// TODO Auto-generated destructor stub
}

QString PeripheralDetail::toHTML() {
	return toSomething(true);
}

QString PeripheralDetail::toString() {
	return toSomething(false);
}

QString PeripheralDetail::toSomething(bool isHTML) {
	QString prefix("");
	QString suffix("");

	QString lineFormat = isHTML ? "<div><b>%1:</b>%2</div>" : "%1: %2";

	QStringList keys(details.keys());
	keys.sort();

	QString toReturn(prefix);
	for (int i = 0; i < keys.length(); ++i) {
		QString line(lineFormat);
		line = line.arg(keys[i]);
		line = line.arg(details[keys[i]].toString());
		toReturn += line;
	}
	toReturn += suffix;

	qDebug() << "Result of toSomething" << toReturn;
	return toReturn;
}

bool PeripheralDetail::isUSBSerial() {
	if (details.contains("bus") && details.contains("class")
			&& details.contains("device_class")) {
		int bus = details["bus"].toInt();
		int klass = details["class"].toInt();
		int deviceClass = details["device_class"].toInt();

		if (bus == PD_BUS_USB_HOST_MODE && klass == PD_CLASS_SERIAL
				&& deviceClass == 2) {
			return true;
		}
	}
	return false;
}

QVariant PeripheralDetail::getValue(QString name) {
	return details[name];
}
