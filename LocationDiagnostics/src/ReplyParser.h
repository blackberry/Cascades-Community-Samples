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

/**
 * A utility class to parse the raw reply from the lower level Location Manager of the OS.
 */
class ReplyParser: public QObject {
Q_OBJECT

public:
	ReplyParser(const QVariant & replyData);
	bool latitude(double *lat);
	bool longitude(double *lon);
	bool altitude(double *alt);
	bool horizontalAccuracy(double *acc);
	bool verticalAccuracy(double *acc);
	bool heading(double *heading);
	bool speed(double *speed);
	bool ttff(double *ttff);
	bool gpsWeek(double *gpsWeek);
	bool gpsTow(double *gpsTow);
	bool utc(double *utc);
	bool hdop(double *hdop);
	bool propagated(bool *propagated);

	bool satellitesSize(int *size);

	bool satelliteId(double *id);
	bool satelliteCarrierToNoiseRatio(double *cno);
	bool satelliteEphemeris(bool *ephemeris);
	bool satelliteAzimuth(double *azimuth);
	bool satelliteElevation(double *elevation);
	bool satelliteTracked(bool *tracked);
	bool satelliteUsed(bool *used);

private:
	const QVariant & _replyData;
	int _satIndex;
	bool parseDouble(double *out, const QVariant & replyData, const QString & key);
	bool parseBool(bool *out, const QVariant & replyData, const QString & key);
	bool parseString(QString *out, const QVariant & replyData, const QString & key);
	bool parseList(QVariantList *out, const QVariant & replyData, const QString & key);

};

#endif /* REPLYPARSER_H_ */
