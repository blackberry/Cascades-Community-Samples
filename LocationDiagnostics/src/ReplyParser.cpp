/*
 * ReplyParser.cpp
 *
 *  Created on: Jul 18, 2012
 *      Author: shaque
 */

#include "ReplyParser.h"

ReplyParser::ReplyParser(const QVariant & replyData) :
		_replyData(replyData), _satIndex(0) {
}
;

bool ReplyParser::latitude(double *lat) {
	return parseDouble(lat, _replyData, "latitude");
}

bool ReplyParser::longitude(double *lon) {
	return parseDouble(lon, _replyData, "longitude");
}

bool ReplyParser::altitude(double *alt) {
	return parseDouble(alt, _replyData, "altitude");
}

bool ReplyParser::horizontalAccuracy(double *acc) {
	return parseDouble(acc, _replyData, "accuracy");
}

bool ReplyParser::verticalAccuracy(double *acc) {
	return parseDouble(acc, _replyData, "altitudeAccuracy");
}

bool ReplyParser::heading(double *heading) {
	return parseDouble(heading, _replyData, "heading");
}

bool ReplyParser::speed(double *speed) {
	return parseDouble(speed, _replyData, "speed");
}

bool ReplyParser::satellitesSize(int *size) {
	QVariantList satellites;
	bool sizeFound = parseList(&satellites, _replyData, "satellites");
	if (sizeFound == false) {
		return false;
	}

	*size = satellites.size();
	return true;
}


bool ReplyParser::satelliteId(double *id) {
	QVariantList satellites;
	if (parseList(&satellites, _replyData, "satellites") == false) {
		return false;
	}
	QVariant sat = satellites.at(_satIndex);
	return parseDouble(id, sat, "id");
}

bool ReplyParser::satelliteCarrierToNoiseRatio(double *cno) {
	QVariantList satellites;
	if (parseList(&satellites, _replyData, "satellites") == false) {
		return false;
	}
	QVariant sat = satellites.at(_satIndex);
	return parseDouble(cno, sat, "cno");
}

bool ReplyParser::parseDouble(double *out, const QVariant & replyData,
		const QString & key) {
	// replyData is a QVariantMap holding all of the reply parameters
	QVariantMap positionData = replyData.toMap();

	QVariant val = positionData.value(key);
	if (val.isValid() && val.canConvert<double>()) {
		*out = val.toDouble();
		return true;
	}

	return false;
}

bool ReplyParser::parseBool(bool *out, const QVariant & replyData,
		const QString & key) {
	// replyData is a QVariantMap holding all of the reply parameters
	QVariantMap positionData = replyData.toMap();

	QVariant val = positionData.value(key);
	if (val.isValid() && val.canConvert<bool>()) {
		*out = val.toBool();
		return true;
	}

	return false;
}

bool ReplyParser::parseString(QString *out, const QVariant & replyData,
		const QString & key) {
	// replyData is a QVariantMap holding all of the reply parameters
	QVariantMap positionData = replyData.toMap();

	QVariant val = positionData.value(key);
	if (val.isValid() && val.canConvert<QString>()) {
		*out = val.toString();
		return true;
	}

	return false;
}

bool ReplyParser::parseList(QVariantList *out, const QVariant & replyData,
		const QString & key) {
	// replyData is a QVariantMap holding all of the reply parameters
	QVariantMap positionData = replyData.toMap();

	QVariant val = positionData.value(key);
	if (val.isValid() && val.canConvert<QVariantList>()) {
		*out = val.toList();
		return true;
	}

	return false;
}



