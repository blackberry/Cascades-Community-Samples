/*
 * SSmaatoSDKInfo.h
 *
 *  Created on: Nov 14, 2013
 *      Author: jial
 */

#ifndef SSMAATOSDKINFO_H_
#define SSMAATOSDKINFO_H_

#include <QObject>
#include <QMetaType>

/*!
 * @brief SSmaatoSDKInfo class provides access to metadata about the SDK.
 *
 * As the BlackBerry static library package does not contain any manifest information, all data provided by this class are hardcoded values. The Developer is responsible to update the values if they change between releases.
 *
 * @since SmaatoSDK 1.0.0
 */
class SSmaatoSDKInfo: public QObject {
	Q_OBJECT
	Q_PROPERTY(QString sdkVersion READ sdkVersion)
	Q_PROPERTY(QString sdkName READ sdkName)
public:
	/*!
	 * Constructs a SSmaatoSDKInfo;
	 */
	SSmaatoSDKInfo(QObject* parent = 0);

	/*!
	 * Destroys the SSmaatoSDKInfo.
	 */
	virtual ~SSmaatoSDKInfo();

	/*!
	 * Returns the sdk version in format major.minor.micro.
	 *
	 * e.g. 1.0.0
	 *
	 * \return The sdk version.
	 */
	QString sdkVersion();

	/*!
	 * Returns the sdk name.
	 *
	 * e.g. 1.0.0
	 *
	 * \return The sdk name.
	 */
	QString sdkName();

};
Q_DECLARE_METATYPE(SSmaatoSDKInfo*)
#endif /* SSMAATOSDKINFO_H_ */
