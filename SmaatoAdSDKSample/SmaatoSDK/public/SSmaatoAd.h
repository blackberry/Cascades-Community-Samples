/*
 * SSmaatoAd.h
 *
 *  Created on: Sep 26, 2013
 *      Author: jial
 */

#ifndef SSMAATOAD_H_
#define SSMAATOAD_H_

#include <QObject>
#include <QMetaType>
#include <QString>
#include <QUrl>
#include <bb/cascades/Image>

using bb::cascades::Image;

/*!
 * @brief SSmaatoAd class contains the ad's data for display.
 *
 * This user should not initialize an instance of it directly. SSmaatoAd objects should be obtained through SSmaatoAPI.
 *
 * @since SmaatoSDK 1.0.0
 */
class SSmaatoAd : public QObject{
	Q_OBJECT
	Q_ENUMS(AdType)
	Q_ENUMS(AdActionType)
	Q_ENUMS(AdActionAccount)

	/*! Whether the SSmaatoAd object is null. */
	Q_PROPERTY(bool isNull READ isNull)

	/*! Whether the SSmaatoAd object is valid. */
	Q_PROPERTY(bool isValid READ isValid)

	/*! The type of the ad.*/
	Q_PROPERTY(AdType type READ getType)

	/*! The action url of the ad. */
	Q_PROPERTY(QString actionUrl READ getActionUrl)

public:
	/*!
	 * The enum defines the types of ads.
	 */
	enum AdType {
		/*! The type of any ads that the sdk can not handle */
		AdTypeUnknown,
		/*! The type of image ads. */
		AdTypeImg = 1,
	};

	/*!
	 * The enum defines the types of the ad's actions.
	 */
	enum AdActionType {
		/*! The type of link actions. */
		AdActionTypeLink
	};

	/*!
	 * The enum defineds the account states, where the accounting takes place.
	 */
	enum AdActionAccount {
		/*! The accounting takes place on server. */
		AdActionAccountServer
	};

	/*!
	 * Constructs a SSmaatoAd.
	 */
	SSmaatoAd(QObject* parent = 0);

	/*!
	 * Destroys the SSmaatoAd.
	 */
	virtual ~SSmaatoAd();

	/*!
	 * Checks if the SSmaatoAd object is properly initialized.
	 *
	 * \return Returns true if the SSmaatoAd object is not properly initialized; otherwise, returns false.
	 */
	bool isNull() const;

	/*!
	 * Checks if SSmaatoAD object contains valid data.
	 *
	 * \return Returns true if the object contains valid data. Otherwise, returns false.
	 */
	virtual bool isValid();

	/*!
	 * Gets the type of the ad.
	 *
	 * \return The type of the ad.
	 */
	virtual AdType getType();

	/*!
	 * Gets the type of the ad's click action type.
	 *
	 * \return The ad's click action type.
	 */
	virtual AdActionType getActionType();

	/*!
	 * Gets the url of the ad's click action.
	 *
	 * \return The action url.
	 */
	virtual QString getActionUrl();

	virtual void showAd();
	virtual Image getAdImage();

protected:

	AdType mAdType;
	AdActionType mActionType;
	QString mActionLink;
	QString mAdText;
	QString mRichMedia;
	QByteArray mAdImgData;
	bool mIsNull;
};
Q_DECLARE_METATYPE(SSmaatoAd*)
#endif /* SSMAATOAD_H_ */
