/*
 * RawLocationParser.cpp
 *
 *  Created on: Jul 18, 2012
 *      Author: shaque
 */

#include "RawLocationParser.h"

RawLocationParser::RawLocationParser(const QVariant & replyData) :
		_replyData(replyData) {

}

double RawLocationParser::latitude() {
	return parseDouble(_replyData, "latitude");
}

double RawLocationParser::longitude() {
	return parseDouble(_replyData, "longitude");
}

double RawLocationParser::altitude() {
	return parseDouble(_replyData, "altitude");
}

double RawLocationParser::horizontalAccuracy() {
	return parseDouble(_replyData, "accuracy");
}

double RawLocationParser::verticalAccuracy() {
	return parseDouble(_replyData, "altitudeAccuracy");
}

double RawLocationParser::heading() {
	return parseDouble(_replyData, "heading");
}

double RawLocationParser::speed() {
	return parseDouble(_replyData, "speed");
}

double RawLocationParser::ttff() {
	return parseDouble(_replyData, "ttff");
}

double RawLocationParser::gpsWeek() {
	return parseDouble(_replyData, "gpsWeek");
}

double RawLocationParser::gpsTow() {
	return parseDouble(_replyData, "gpsTow");
}

double RawLocationParser::utc() {
	return parseDouble(_replyData, "utc");
}

double RawLocationParser::hdop() {
	return parseDouble(_replyData, "hdop");
}

double RawLocationParser::vdop() {
	return parseDouble(_replyData, "vdop");
}

double RawLocationParser::pdop() {
	return parseDouble(_replyData, "pdop");
}

bool RawLocationParser::propagated() {
	return parseBool(_replyData, "propagated");
}

QString RawLocationParser::positionMethod() {
	QString fixType = parseString(_replyData, "fix_type");
	QString provider = parseString(_replyData, "provider");
	return fixType + " [" + provider + "]";
}

QString RawLocationParser::error() {
	QString err = parseString(_replyData, "err");
	QString errstr = parseString(_replyData, "errstr");
	return err + ": " + errstr;
}

int RawLocationParser::numberOfSatellites() {
	QVariantList satellites;
	bool sizeFound = parseList(&satellites, _replyData, "satellites");
	if (sizeFound == false) {
		return 0;
	}

	return satellites.size();
}

double RawLocationParser::satelliteId(int satIndex) {
	QVariantList satellites;
	if (parseList(&satellites, _replyData, "satellites") == false) {
		return qQNaN();
	}
	QVariant sat = satellites.at(satIndex);
	return parseDouble(sat, "id");
}

double RawLocationParser::satelliteCarrierToNoiseRatio(int satIndex) {
	QVariantList satellites;
	if (parseList(&satellites, _replyData, "satellites") == false) {
		return qQNaN();
	}
	QVariant sat = satellites.at(satIndex);
	return parseDouble(sat, "cno");
}

bool RawLocationParser::satelliteEphemerisAvailable(int satIndex) {
	QVariantList satellites;
	if (parseList(&satellites, _replyData, "satellites") == false) {
		return false;
	}
	QVariant sat = satellites.at(satIndex);
	return parseBool(sat, "ephemeris");
}

double RawLocationParser::satelliteAzimuth(int satIndex) {
	QVariantList satellites;
	if (parseList(&satellites, _replyData, "satellites") == false) {
		return qQNaN();
	}
	QVariant sat = satellites.at(satIndex);
	return parseDouble(sat, "azimuth");
}

double RawLocationParser::satelliteElevation(int satIndex) {
	QVariantList satellites;
	if (parseList(&satellites, _replyData, "satellites") == false) {
		return qQNaN();
	}
	QVariant sat = satellites.at(satIndex);
	return parseDouble(sat, "elevation");
}

bool RawLocationParser::satelliteTracked(int satIndex) {
	QVariantList satellites;
	if (parseList(&satellites, _replyData, "satellites") == false) {
		return false;
	}
	QVariant sat = satellites.at(satIndex);
	return parseBool(sat, "tracked");
}

bool RawLocationParser::satelliteUsed(int satIndex) {
	QVariantList satellites;
	if (parseList(&satellites, _replyData, "satellites") == false) {
		return false;
	}
	QVariant sat = satellites.at(satIndex);
	return parseBool(sat, "used");
}

bool RawLocationParser::satelliteAlmanac(int satIndex) {
	QVariantList satellites;
	if (parseList(&satellites, _replyData, "satellites") == false) {
		return false;
	}
	QVariant sat = satellites.at(satIndex);
	return parseBool(sat, "almanac");
}

double RawLocationParser::parseDouble(const QVariant & replyData, const QString & key) {
	// replyData is a QVariantMap holding all of the reply parameters
	QVariantMap positionData = replyData.toMap();

	QVariant val = positionData.value(key);
	if (val.isValid() && val.canConvert<double>()) {
		return val.toDouble();
	}

	return qQNaN();
}

bool RawLocationParser::parseBool(const QVariant & replyData, const QString & key) {
	// replyData is a QVariantMap holding all of the reply parameters
	QVariantMap positionData = replyData.toMap();

	QVariant val = positionData.value(key);
	if (val.isValid() && val.canConvert<bool>()) {
		return val.toBool();
	}

	return false;
}

QString RawLocationParser::parseString(const QVariant & replyData, const QString & key) {
	// replyData is a QVariantMap holding all of the reply parameters
	QVariantMap positionData = replyData.toMap();

	QVariant val = positionData.value(key);
	if (val.isValid() && val.canConvert<QString>()) {
		return val.toString();
	}

	return "";
}

bool RawLocationParser::parseList(QVariantList *out, const QVariant & replyData, const QString & key) {
	// replyData is a QVariantMap holding all of the reply parameters
	QVariantMap positionData = replyData.toMap();

	QVariant val = positionData.value(key);
	if (val.isValid() && val.canConvert<QVariantList>()) {
		*out = val.toList();
		return true;
	}

	return false;
}

