/* Copyright (c) 2012 Research In Motion Limited.
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
#include <QDebug>

const QString Utilities::HEX_CHARS = QString("0123456789ABCDEF");

uint8_t Utilities::hexToInt(QChar c1, QChar c2) {
	c1 = c1.toUpper();
	c2 = c2.toUpper();
	uint8_t result = 0;
	result = HEX_CHARS.indexOf(c1) * 16;
	result = result + HEX_CHARS.indexOf(c2);
	return result;
}

void Utilities::hexToIntArray(QString hex_string,uint8_t* output) {
	const size_t numdigits = hex_string.length() / 2;
	for (size_t i = 0; i != numdigits; ++i) {
		output[i] = hexToInt(hex_string.at(2 * i), hex_string.at(2 * i + 1));
	}
}

bool Utilities::isValidHex(QString hex_string) {
	const size_t numdigits = hex_string.length();
	if (numdigits == 0) {
		return true;
	}
	if (numdigits % 2 == 1) {
		qDebug() << "XXXX Invalid hex string. Odd number of characters:" << hex_string;
		return false;
	}
	for (size_t i = 0; i != numdigits; ++i) {
		if (HEX_CHARS.indexOf(hex_string.at(i)) == -1) {
			qDebug() << "XXXX Invalid hex character:" << hex_string.at(i);
			return false;
		}
	}
	return true;
}

