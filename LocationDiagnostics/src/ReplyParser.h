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

class ReplyParser: public QObject {
	Q_OBJECT

public:
	ReplyParser(const QVariant & replyData);
	bool latitude( double *lat );
	bool longitude( double *lon );
	bool altitude( double *alt );
	bool horizontalAccuracy( double *acc );
	bool verticalAccuracy( double *acc );
	bool heading( double *heading );
	bool speed( double *speed );
	bool satellitesSize( int *size );
	bool setSatelliteIndex( int index );
	bool satelliteId( double *id );
	bool satelliteCarrierToNoiseRatio( double *cno );

private:
	const QVariant & _replyData;
	int _satIndex;
    bool parseDouble( double *out, const QVariant & replyData, const QString & key );
    bool parseBool( bool *out, const QVariant & replyData, const QString & key );
    bool parseString( QString *out, const QVariant & replyData, const QString & key );
    bool parseList( QVariantList *out, const QVariant & replyData, const QString & key );


};

#endif /* REPLYPARSER_H_ */
