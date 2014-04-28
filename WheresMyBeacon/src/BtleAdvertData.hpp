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

#ifndef BTLEADVERTDATA_HPP_
#define BTLEADVERTDATA_HPP_

#include <QObject>
#include <QDebug>
#include <stdint.h>

class BtleAdvertData: public QObject {

	Q_OBJECT

public:
	BtleAdvertData(QObject *obj = 0);
	virtual ~BtleAdvertData();

	bool parse(const QByteArray &advertData);
	bool hasBeaconData();
	QByteArray & beaconUuid();
	QString beaconUuidAsString();
	int beaconMajor();
	int beaconMinor();
	int calibratedStrength();
	void clear();

private:
	QByteArray _advertData;
	QByteArray _beaconUuid;
	uint16_t _beaconMajor;
	uint16_t _beaconMinor;
	int8_t _calibratedStrength;
	bool _hasBeaconData;
};

#endif /* BTLEADVERTDATA_HPP_ */
