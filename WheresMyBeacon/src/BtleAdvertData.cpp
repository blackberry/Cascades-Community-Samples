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
	, _hasIBeaconData(false)
    , _hasAltBeaconData(false)
    , _beaconMfgCompanyCode(0)
    , _beaconId()
    , _beaconReserved(0)
{
}

BtleAdvertData::~BtleAdvertData()
{
}

/*
	Sample data (iBeacon)

	Estimote data = 02 01 05 1a ff 4c 00 02 15 b9 40 7f 30 f5 f8 46 6e af f9 25 55 6b 57 fe 6d fa 46 b8 1a c2
	Sensor Tag               1A FF 4C 00 02 15 FA 5F 55 D9 BC 63 40 2E A2 54 09 1B 8F E8 C9 91 00 01 00 01 C5
	02 (AD Structure length)
	    01  (AD Type - flags)
	        05  (AD Data)
	1a  (AD Structure length)
	    ff  (AD Type - Manufacturer Specific Data)
	        4c 00 ( Apple Specific data  - little endian of 0x004c)
	        02 (iBeacon data follows)
	            15  ( 21 (dec) bytes)
	                b9 40 7f 30 f5 f8 46 6e af f9 25 55 6b 57 fe 6d ( BeconRegion UUID - fixed field 16 bytes)
	                fa 46  (Major Number - fixed field 16 bit integer - (big endian 64070)
	                b8 1a  (Minor Number - fixed field 16 bit integer - (big endian 47130)
	                c2 (Calibrated Strength -- power in dBm at 1m from device)


    AltBeacon (RIM MANF code) data = 1b ff 3c 00 be ac 02 15 b9 40 7f 30 f5 f8 46 6e af f9 25 55 6b 57 fe 6d fa 46 c2 00

    1b  (AD Structure length)
        ff  (AD Type - Manufacturer Specific Data)
            3c 00 ( RIM Specific data - little endian of 0x003c)
            be ac (Alt Beacon data follows big endian of 0xbeac)
               02 15 b9 40 7f 30 f5 f8 46 6e af f9 25 55 6b 57 fe 6d fa 46 ( Beacon Id 20 bytes )
               c2 (Reference -- Calibrated Strength -- power in dBm at 1m from device)
               00 (Reserved)
*/

bool BtleAdvertData::parse(const QByteArray &advertData)
{

    qDebug() << "BBBB parse [" << advertData.toHex() << "]";

	if (advertData.length() < 27) { // quick sanity check 27 is minimal length of iBeacon advertisment
		_hasIBeaconData = false;
		return true;
	}
	int i = 0;
	do {
		uint8_t entryLen = advertData.at(i);
		uint8_t entryType = advertData.at(i+1);

		if (entryType == 0xff) {
			int j = i+2;
			if (((uint8_t)advertData.at(j) == 0x4c) && ((uint8_t)advertData.at(j+1) == 0x00) && ((uint8_t)advertData.at(j+2) == 0x02)&& ((uint8_t)advertData.at(j+3) == 0x15)) {
				_hasIBeaconData = true;
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

			} else if (((uint8_t)advertData.at(j+2) == 0xbe)&& ((uint8_t)advertData.at(j+3) == 0xac)) {
                _hasAltBeaconData = true;
                _beaconMfgCompanyCode  = (uint16_t)((uint8_t)advertData.at(j) & 0xff);
                _beaconMfgCompanyCode += (uint16_t)(((uint8_t)advertData.at(j+1) & 0xff) << 8);
                _beaconId.clear();
                for (int k=0; k<20; k++) {
                    _beaconId.append(advertData.at(k+j+4));
                }
                j += 24;
                _calibratedStrength = 0;
                _calibratedStrength = (int8_t) (advertData.at(j) & 0xff);
                j += 1;
                _beaconReserved = (int8_t) (advertData.at(j) & 0xff);

                return true;
			}
		}
		i += (entryLen+1);
	} while (i < advertData.length());

	_hasIBeaconData = false;
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

	if (_beaconUuid == estimote) {
		return "Default Estimote UUID";
	}

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
	return hasIBeaconData() || hasAltBeaconData();
}

bool BtleAdvertData::hasIBeaconData()
{
    return _hasIBeaconData;
}

void BtleAdvertData::clear()
{
	_advertData.clear();

	_beaconUuid.clear();
	_beaconMajor = 0;
	_beaconMinor = 0;
	_calibratedStrength = 0;
	_hasIBeaconData = false;

    _hasAltBeaconData = false;
    _beaconMfgCompanyCode = 0;
    _beaconId.clear();
    _beaconReserved = 0;
}

QByteArray & BtleAdvertData::beaconId()
{
    return _beaconId;
}

bool BtleAdvertData::hasAltBeaconData()
{
    return _hasAltBeaconData;
}

int BtleAdvertData::companyCode()
{
    return (int)_beaconMfgCompanyCode;
}

int BtleAdvertData::altBeaconReserved()
{
    return (int)_beaconReserved;
}

QString BtleAdvertData::beaconIdAsString()
{
    QString id("");
    for (int i = 0; i < _beaconId.length(); i++) {
        id.append(QString("%1").arg((uint8_t)_beaconId.at(i), 0, 16));
    }
    return id;
}

QString BtleAdvertData::companyCodeAsString()
{
    QString company = "";

    switch (companyCode()) {
        case 60:
            company = "BlackBerry Ltd.";
            break;
        case 76:
            company = "Apple Inc.";
            break;
        default:
            break;
    }

    return company;
}

