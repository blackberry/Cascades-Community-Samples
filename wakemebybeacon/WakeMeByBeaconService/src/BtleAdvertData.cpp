/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
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

#include "BtleAdvertData.hpp"

BtleAdvertData::BtleAdvertData(QObject *obj)
	: QObject(obj)
	, _advertData()
	, _beaconUuid()
	, _beaconMajor(0)
	, _beaconMinor(0)
	, _calibratedStrength(0)
	, _hasBeaconData(false)
{
}

BtleAdvertData::~BtleAdvertData()
{
}

/*
	Sample data

	Estimote data = 02 01 05 1a ff 4c 00 02 15 b9 40 7f 30 f5 f8 46 6e af f9 25 55 6b 57 fe 6d fa 46 b8 1a c2
	Sensor Tag               1A FF 4C 00 02 15 FA 5F 55 D9 BC 63 40 2E A2 54 09 1B 8F E8 C9 91 00 01 00 01 C5
	02 (AD Structure length)
	    01  (AD Type - flags)
	        05  (AD Data)
	1a  (AD Structure length)
	    ff  (AD Type - Manufacturer Specific Data)
	        4c 00 ( Apple Specific data )
	        02 (iBeacon data follows)
	            15  ( 21 (dec) bytes)
	                b9 40 7f 30 f5 f8 46 6e af f9 25 55 6b 57 fe 6d ( BeconRegion UUID - fixed field 16 bytes)
	                fa 46  (Major Number - fixed field 16 bit integer - (endian ? 64070)
	                b8 1a  (Minor Number - fixed field 16 bit integer - (endian ? 47130)
	                c2 (Calibrated Strength)
*/

bool BtleAdvertData::parse(const QByteArray &advertData)
{

    qDebug() << "SSSS parse [" << advertData.toHex() << "]";

	if (advertData.length() < 27) { // quick sanity check 27 is minimal length of iBeacon advertisment
		_hasBeaconData = false;
		return true;
	}
	int i = 0;
	do {
		uint8_t entryLen = advertData.at(i);
		uint8_t entryType = advertData.at(i+1);

		if (entryType == 0xff) {
			int j = i+2;
			if (((uint8_t)advertData.at(j) == 0x4c) && ((uint8_t)advertData.at(j+1) == 0x00) && ((uint8_t)advertData.at(j+2) == 0x02)&& ((uint8_t)advertData.at(j+3) == 0x15)) {
				_hasBeaconData = true;
				_beaconUuid.clear();
				for (int k=0; k<16; k++) {
					_beaconUuid.append(advertData.at(k+j+4));
				}
				j += 20;
				_beaconMajor = 0;
				_beaconMajor  = (uint16_t)((uint8_t)advertData.at(j+1) & 0xff);
				_beaconMajor += (uint16_t)(((uint8_t)advertData.at(j) & 0xff) << 8);
				j += 2;
				_beaconMinor = 0;
				_beaconMinor  = (uint16_t)((uint8_t)advertData.at(j+1) & 0xff);
				_beaconMinor += (uint16_t)(((uint8_t)advertData.at(j) & 0xff) << 8);
				j += 2;
				_calibratedStrength = 0;
				_calibratedStrength = (int8_t) (advertData.at(j) & 0xff);

				return true;
			}
		}
		i += (entryLen+1);
	} while (i < advertData.length());

	_hasBeaconData = false;
	return true;
}

QByteArray & BtleAdvertData::beaconUuid()
{
	return _beaconUuid;
}

QString BtleAdvertData::beaconUuidAsString()
{
	QString uuid("");

	QByteArray estimote = QByteArray::fromHex("b9407f30f5f8466eaff925556b57fe6d");

	for (int i = 0; i < _beaconUuid.length(); i++) {
	    uuid.append(QString("%1").arg((uint8_t)_beaconUuid.at(i), 0, 16));
	}
	return uuid;
}

int BtleAdvertData::beaconMajor()
{
	return (int)_beaconMajor;
}

int BtleAdvertData::beaconMinor()
{
	return (int)_beaconMinor;
}

int BtleAdvertData::calibratedStrength()
{
	return (int)_calibratedStrength;
}

bool BtleAdvertData::hasBeaconData()
{
	return _hasBeaconData;
}

void BtleAdvertData::clear()
{
	_advertData.clear();
	_beaconUuid.clear();
	_beaconMajor = 0;
	_beaconMinor = 0;
	_calibratedStrength = 0;
	_hasBeaconData = false;
}
