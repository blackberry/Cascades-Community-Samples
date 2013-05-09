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
#include <nfc/nfc_types.h>

const QString Utilities::HEX_CHARS = QString("0123456789ABCDEF");

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

bool Utilities::isSameCharArray(unsigned char* array1, unsigned char* array2, int array_length) {
	for (int i = 0; i < array_length; i++) {
		if (array1[i] != array2[i]) {
			return false;
		}
	}
	return true;
}

QString Utilities::getOperationResultName(int rc) {

	switch (rc) {
	case NFC_RESULT_SUCCESS:
		return QString("NFC_RESULT_SUCCESS");
	case NFC_RESULT_INVALID_PARAMETER:
		return QString("NFC_RESULT_INVALID_PARAMETER");
	case NFC_RESULT_FEATURE_NOT_IMPLEMENTED:
		return QString("NFC_RESULT_FEATURE_NOT_IMPLEMENTED");
	case NFC_RESULT_UNSUPPORTED_API:
		return QString("NFC_RESULT_UNSUPPORTED_API");
	case NFC_RESULT_LOCKED:
		return QString("NFC_RESULT_LOCKED");
	case NFC_RESULT_LIMITATION_EXCEEDED:
		return QString("NFC_RESULT_LIMITATION_EXCEEDED");
	case NFC_RESULT_OPERATION_NOT_SUPPORTED:
		return QString("NFC_RESULT_OPERATION_NOT_SUPPORTED");
	case NFC_RESULT_OPERATION_REJECTED:
		return QString("NFC_RESULT_OPERATION_REJECTED");
	case NFC_RESULT_SERVICE_CONNECTION_ERROR:
		return QString("NFC_RESULT_SERVICE_CONNECTION_ERROR");
	case NFC_RESULT_P2P_REJECTED:
		return QString("NFC_RESULT_P2P_REJECTED");
	case NFC_RESULT_TIMEOUT:
		return QString("NFC_RESULT_TIMEOUT");
	case NFC_RESULT_RESOURCE_BUSY:
		return QString("NFC_RESULT_RESOURCE_BUSY");
	case NFC_RESULT_NOT_ENOUGH_SPACE:
		return QString("NFC_RESULT_NOT_ENOUGH_SPACE");
	case NFC_RESULT_OUT_OF_MEMORY:
		return QString("NFC_RESULT_OUT_OF_MEMORY");
	case NFC_RESULT_FD_ACCESS_ERROR:
		return QString("NFC_RESULT_FD_ACCESS_ERROR");
	case NFC_RESULT_LOCK_FAILED:
		return QString("NFC_RESULT_LOCK_FAILED");
	case NFC_RESULT_WRITE_FAILED:
		return QString("NFC_RESULT_WRITE_FAILED");
	case NFC_RESULT_READ_FAILED:
		return QString("NFC_RESULT_READ_FAILED");
	case NFC_RESULT_NOT_NFC_TAG_BUT_CAPABLE:
		return QString("NFC_RESULT_NOT_NFC_TAG_BUT_CAPABLE");
	case NFC_RESULT_TAG_NOT_SUPPORTED:
		return QString("NFC_RESULT_TAG_NOT_SUPPORTED");
	case NFC_RESULT_BAD_NDEF_FORMAT:
		return QString("NFC_RESULT_BAD_NDEF_FORMAT");
	case NFC_RESULT_SE_INPUT_TOO_SHORT:
		return QString("NFC_RESULT_SE_INPUT_TOO_SHORT");
	case NFC_RESULT_SE_INVALID_HANDLE:
		return QString("NFC_RESULT_SE_INVALID_HANDLE");
	case NFC_RESULT_SE_SERVICE_NOT_READY:
		return QString("NFC_RESULT_SE_SERVICE_NOT_READY");
	case NFC_RESULT_SE_OUT_OF_LOGICAL_CHANNELS:
		return QString("NFC_RESULT_SE_OUT_OF_LOGICAL_CHANNELS");
	case NFC_RESULT_SE_AID_NOT_FOUND:
		return QString("NFC_RESULT_SE_AID_NOT_FOUND");
	case NFC_RESULT_SE_INVALID_APDU:
		return QString("NFC_RESULT_SE_INVALID_APDU");
	case NFC_RESULT_SE_NOT_PRESENT:
		return QString("NFC_RESULT_SE_NOT_PRESENT");
	case NFC_RESULT_SE_REQUEST_REJECTED:
		return QString("NFC_RESULT_SE_REQUEST_REJECTED");
	case NFC_RESULT_UNHANDLED_ERROR:
		return QString("NFC_RESULT_UNHANDLED_ERROR");
	case NFC_RESULT_ERROR:
		return QString("NFC_RESULT_ERROR");
	default:
		return QString("UNKNOWN RESULT CODE");
	};

}

