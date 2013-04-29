/* Copyright (c) 2013 Research In Motion Limited.
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
#include "Utilities.hpp"
#include "stdint.h"
#include <QChar>
#include <QString>
#include <QDateTime>
#include <QDebug>
#include <bb/system/SystemDialog>
#include <sys/time.h>
#include <bb/platform/PlatformInfo>

using namespace bb::platform;

const QString Utilities::HEX_CHARS = QString("0123456789ABCDEF");
const  PlatformInfo* m_platformInfo = new bb::platform::PlatformInfo();

uint8_t Utilities::hexToInt(QChar c1, QChar c2) {
	c1 = c1.toUpper();
	c2 = c2.toUpper();
	uint8_t result = 0;
	result = HEX_CHARS.indexOf(c1) * 16;
	result = result + HEX_CHARS.indexOf(c2);
	return result;
}

void Utilities::hexToIntArray(QString hex_string, uint8_t* output) {
	const size_t numdigits = hex_string.length() / 2;
	for (size_t i = 0; i != numdigits; ++i) {
		output[i] = hexToInt(hex_string.at(2 * i), hex_string.at(2 * i + 1));
	}
}

QString Utilities::intToHex(int decimal) {
	QString hexadecimal;
	hexadecimal.setNum(decimal, 16);
	return hexadecimal;
}

void Utilities::alert(QString msg) {
	bb::system::SystemDialog *dial;
	dial = new bb::system::SystemDialog("OK", NULL);
	dial->setTitle("Alert");
	dial->setBody(msg);
	dial->setDismissAutomatically(true);
	dial->show();
}

uint Utilities::getSysTimeSeconds() {
	QDateTime now = QDateTime::currentDateTime();
	return now.toTime_t();
}

QString Utilities::getOSVersion() {
	if(m_platformInfo)
		return m_platformInfo->osVersion();
	else
		return QString::null;
}
