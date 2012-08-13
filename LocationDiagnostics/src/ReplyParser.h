/*
 * ReplyParser.h
 *
 *  Created on: Jul 18, 2012
 *      Author: shaque
 */

#ifndef REPLYPARSER_H_
#define REPLYPARSER_H_

#include <QtCore/QObject>
#include <QtCore/qvariant.h>
#include <qnumeric.h>

/**
 * A utility class to parse the raw reply from the lower level Location Manager of the OS.
 */
class RawLocationParser: public QObject {
Q_OBJECT

public:
	RawLocationParser(const QVariant & replyData);
	double latitude();
	double longitude();
	double altitude();
	double horizontalAccuracy();
	double verticalAccuracy();
	double heading();
	double speed();
	double ttff();
	double gpsWeek();
	double gpsTow();
	double utc();
	double hdop();
	double vdop();
	double pdop();
	bool propagated();
	QString positionMethod();
	QString error();

	int numberOfSatellites();
	double satelliteId(int satIndex);
	double satelliteCarrierToNoiseRatio(int satIndex);
	bool satelliteEphemerisAvailable(int satIndex);
	double satelliteAzimuth(int satIndex);
	double satelliteElevation(int satIndex);
	bool satelliteTracked(int satIndex);
	bool satelliteUsed(int satIndex);
	bool satelliteAlmanac(int satIndex);

private:
	const QVariant & _replyData;
	double parseDouble(const QVariant & replyData, const QString & key);
	bool parseBool(const QVariant & replyData, const QString & key);
	QString parseString(const QVariant & replyData, const QString & key);
	bool parseList(QVariantList *out, const QVariant & replyData, const QString & key);

};

#endif /* REPLYPARSER_H_ */
