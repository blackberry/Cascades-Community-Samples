/*
 * SSmaatoAPI.h
 *
 *  Created on: Sep 26, 2013
 *      Author: jial
 */

#ifndef SSMAATOAPI_H_
#define SSMAATOAPI_H_

#include <QObject>
#include <QMetaType>
#include <QString>
#include <QHash>
#include <QVariant>
#include <bb/device/HardwareInfo>
#include <bb/device/DisplayInfo>
#include <bb/platform/PlatformInfo>
#include <bb/device/CellularNetworkInfo>
#include <bb/system/LocaleHandler>

#include "SSmaatoAd.h"

using bb::device::HardwareInfo;
using bb::device::DisplayInfo;
using bb::platform::PlatformInfo;
using bb::device::CellularNetworkInfo;
using bb::system::LocaleHandler;

class SHttpRequest;
class SHttpResponse;
class SLocationHandler;
class SAdResponse;
class SReverseGeocoder;

/*!
 * @brief SSmaatoAPI class handles the parameters for ad request and retrieves ads from server.
 *
 * The user must provide a valid Publisher Id and a valid Ad Space Id to get ads from server.
 * If the user does not explicitly set the other parameters, their default values are used or the parameter will not be sent with the request if it doesn't have default value.
 *
 * An instance of the SSmaatoAPI handles one ad request at a time. Calling fetchAd() function before the current request is finished does nothing.
 * It only keeps one ad in memory and replaces it when a new ad is retrieved from server.
 *
 * ## Permissions
 * ### Mandatory permissions
 * - read_device_identifying_information
 *
 * ### Optional permissions for the sdk to be fully functioning
 * - access_location_services
 *
 * ## Examples
 * Register SSmaatoAPI in the application class.
  @code

 	 qmlRegisterType<SSmaatoAPI>("smaatosdk", 1, 0, "SSmaatoAPI");

  @endcode
 *
 * Create an SSmmatoAPI and start a ad request
   @code

   SSmaatoAPI* api = new SSmaatoAPI("AdSpaceId", "PublisherId");

   QObject::connect(api, SIGNAL(adFetchFinished(QVariant)), this, SLOT(onAdFetchFinished(QVariant)));
   QObject::connect(api, SIGNAL(adFetchFailed(SSmaatoAPI::AdFetchError, QString, QString)), this, SLOT(onAdFetchFailed(SSmaatoAPI::AdFetchError, QString, QString)));

   api->fetchAd();

   @endcode
 *
 * @since SmaatoSDK 1.0.0
 */
class SSmaatoAPI : public QObject{
	Q_OBJECT
	Q_ENUMS(AdFormat)
	Q_ENUMS(AdDimension)
	Q_ENUMS(Gender)
	Q_ENUMS(AdFetchError)

	/*!
	 * The publisher id.
	 */
	Q_PROPERTY(QString publisherId READ getPublisherId WRITE setPublisherId)

	/*!
	 * The ad space id.
	 */
	Q_PROPERTY(QString adSpaceId READ getAdSpaceId WRITE setAdSpaceId)

	/*!
	 * The format of the ad to be requested.
	 */
	Q_PROPERTY(AdFormat format READ getFormat WRITE setFormat)

	/*!
	 * Format strict is used to force server to return only required format.
	 */
	Q_PROPERTY(bool formatStrict READ isFormatStrict)

	/*!
	 * The coppa of the ad to be requested.
	 *
	 * The possible values are 0 and 1.
	 *
	 * The default value is 0.
	 */
	Q_PROPERTY(int coppa READ getCoppa WRITE setCoppa)

	/*!
	 * The width of the ad to be requested.
	 *
	 * The default value is 0, incidcating no height is set.
	 */
	Q_PROPERTY(int width READ getWidth WRITE setWidth)

	/*!
	 * The height of the ad to be requested.
	 *
	 * The default value is 0, incidcating no height is set.
	 */
	Q_PROPERTY(int height READ getHeight WRITE setHeight)

	/*!
	 * The dimension of the ad to be requested.
	 *
	 * The default value is AdDimensionMMA.
	 */
	Q_PROPERTY(AdDimension dimension READ getDimension WRITE setDimension)

	/*!
	 * The user's age.
	 *
	 * The default value is 0, incidcating no age is set.
	 */
	Q_PROPERTY(int age READ getAge WRITE setAge)

	/*!
	 * The user's gender.
	 *
	 * The default value is UnknownGender, incidcating no gender is set.
	 */
	Q_PROPERTY(Gender gender READ getGender WRITE setGender)

	/*!
	 * The content tags.
	 *
	 * Tags are comma separated case insensitive free text, describing the content.
	 */
	Q_PROPERTY(QString tags READ getTags WRITE setTags)

	/*!
	 * The query string.
	 *
	 * A comma separated search term entered by the user. (e.g. coffee,tea)
	 */
	Q_PROPERTY(QString queryString READ getQueryString WRITE setQueryString)

	/*!
	 * The post code of the user's location.
	 */
	Q_PROPERTY(QString zip READ getZip WRITE setZip)

public:
	/*!
	 * The desired format of the ads to be returned.
	 */
	enum AdFormat {
		/*! The format for image ads. */
		AdFormatImg = 1,
	};

	/*!
	 * The desired dimension of ads to be returned.
	 */
	enum AdDimension {
		/*! Any MMA banners. This value should be used as the default. */
		AdDimensionMMA,
		/*! IAB Medium Rectangle (1:1.2 banners). */
		AdDimensionMEDRECT,
		/*! Any full screen banners. */
		AdDimensionFullScreen,
	};

	/*!
	 * Valid gender values.
	 */
	enum Gender {
		/*! Gender is unknown. */
		UnknownGender,
		/*! Male */
		Male,
		/*! Female */
		Female
	};

	/*!
	 * The result type of the ad fetch.
	 */
	enum AdFetchError {
		/*! The application doesn't have sufficient permission to access device information, which is required for fetching ads. */
		AdFetchErrorNoSufficientPermission,
		/*! The parameters are not set properly. */
		AdFetchErrorNoSufficientParameters,
		/*! The ad request is failed because of network problem. */
		AdFetchErrorNetworkError,
		/*! The server response does not contain any data. */
		AdFetchErrorNoDataInServerResponse,
		/*! The server response contains invalid data. */
		AdFetchErrorInvalidAdData,
		/*! The ad request is failed becaused of a server error. */
		AdFetchErrorServerError,
		/*! The server response indicates that no ad is available. */
		AdFetchErrorNoAdAvailable,
		/*! the server response contains a ad, which is of an unknown ad type. */
		AdFetchErrorUnknownAdType
	};

	/*!
	 * Constructs a SSmaatoAPI object.
	 */
	SSmaatoAPI(const QString& adSpaceId = QString(), const QString& publisherId = QString(), QObject* parent = 0);

	/*!
	 * Destroys the SSmaatoAPI object.
	 */
	virtual ~SSmaatoAPI();

	/*!
	 * Sets the Ad Space Id.
	 *
	 * The adSpaceId is mandatory for getting ads from server.
	 *
	 *  \param adSpaceId The Ad Space Id.
	 */
	void setAdSpaceId(const QString& adSpaceId);

	/*!
	 *  Gets the Ad Space Id.
	 *
	 *  \return returns the Ad Space Id.
	 */
	QString getAdSpaceId() const;

	/*!
	 * Sets the Publisher Id.
	 *
	 * The publisherId is mandatory for getting ads from server.
	 *
	 * \param publisherId The Publisher Id.
	 */
	void setPublisherId(const QString& publisherId);

	/*!
	 * Gets the Publisher Id.
	 *
	 * \return The Publisher Id.
	 */
	QString getPublisherId() const;

	/*!
	 * Sets the format of the ad to be requested.
	 *
	 * The default value is AdFormatImg.
	 *
	 * \param format AdFormat.
	 */
	void setFormat(AdFormat format);

	/*!
	 * Gets the format of the ad to be requested.
	 *
	 * \return The AdFormat.
	 */
	AdFormat getFormat() const;

	/*!
	 * Gets the value of Format Strict.
	 *
	 * \return The value of Format Strict.
	 */
	bool isFormatStrict() const;

	/*!
	 * Sets the COPPA value. It can be either 0 or 1.
	 *
	 * The default value is 0.
	 *
	 * \param coppa The coppa.
	 */
	void setCoppa(int coppa);

	/*!
	 * Gets the COPPA value;
	 *
	 * \return The COPPA value;
	 */
	int getCoppa() const;

	/*!
	 * Sets the width of the ad to be requested. The ad width is optional for getting ads from server.
	 *
	 * \param width The required width.
	 */
	void setWidth(int width);

	/*!
	 * Gets the width of the ad to be requested.
	 *
	 * return The width of the ad.
	 */
	int getWidth() const;

	/*!
	 * Sets the height of the ad to be requested. The ad height is optional for getting ads from server.
	 *
	 * \param height The required height.
	 */
	void setHeight(int height);

	/*!
	 * Gets the height of the ad to be requested.
	 *
	 * \return The height of the ad.
	 */
	int getHeight() const;

	/*!
	 * Sets the dimension of the ad to be requested.
	 *
	 * The default value is AdDimensionMMA.
	 *
	 * \param dimension The required dimension.
	 */
	void setDimension(AdDimension dimension);

	/*!
	 * Gets the dimension of the ad to be requested.
	 *
	 * \return The dimension value.
	 */
	AdDimension getDimension() const;

	/*!
	 * Sets the user's age. The age is optional for getting ads from server.
	 *
	 * \param age The user's age.
	 */
	void setAge(int age);

	/*!
	 * Gets the user's age.
	 *
	 * \return The user's age.
	 */
	int getAge() const;

	/*!
	 * Sets the user's gender. The gender is optional for getting ads from server.
	 *
	 * The default value is UnknownGender, indicating no gender is set.
	 *
	 * \param gender The user's gender.
	 */
	void setGender(Gender gender);

	/*!
	 * Gets the user's gender.
	 *
	 * \return The user's gender.
	 */
	Gender getGender() const;

	/*!
	 * Sets a case insensitive, comma separated free text describing the content. (e.g. motorsport,news,cars) The tags is optional for getting ads from server.
	 *
	 * \param tags The comma separated tags.
	 */
	void setTags(const QString& tags);

	/*!
	 * Gets the tags.
	 *
	 * \return The tags.
	 */
	QString getTags() const;

	/*!
	 * Sets a comma separated search term. (e.g. coffee,washington) The query string is optional for getting ads from server.
	 *
	 * \param queryString The comma separated search term.
	 */
	void setQueryString(const QString& queryString);

	/*!
	 * Gets the query string.
	 *
	 * \return The query string.
	 */
	QString getQueryString() const;

	/*!
	 * Sets a post code. The zip is optional for getting ads from server.
	 *
	 * \param zip A postcode.
	 */
	void setZip(const QString& zip);

	/*!
	 * Gets the zip code.
	 *
	 * \return The zip code.
	 */
	QString getZip() const;

	/*!
	 * Fetches an ad from the server. This function does nothing if the SSmaatoAPI is currently fetching for an ad.
	 *
	 * \return True if a new fetch is started or the SSmaatoAPI is currently fetching. Otherwise, false.
	 */
	Q_INVOKABLE bool fetchAd();

	/*!
	 * Indicates if the SSmaatoAPI is currently fetching for an ad.
	 *
	 * \return True if the SSmaatoAPI is currently fetching. Otherwise, false.
	 */
	Q_INVOKABLE bool isFetching() const;

	/*!
	 * Get the ad for showing. An ad times out in 5 minutes from the time it is fetched from server.
	 */
	Q_INVOKABLE SSmaatoAd* getAd();

	/*
	 * Indicates if an ad is currently available for showing. An ad times out in 5 minutes from the time it is fetched from server.
	 *
	 * \return True if an ad is available. Otherwise, false.
	 */
	Q_INVOKABLE bool hasAd() const;

signals:
	/*!
	 * The signal is emitted when ad fetching is successfully finished.
	 *
	 * @note The user takes the ownership of the ad and responsible to delete it. Do not delete the ad object in the slot connected to the signal. Use deleteLater().
	 *
	 * \param ad The ad downloaded from server.
	 */
	void adFetchFinished(const QVariant& /* SSmaatoAd* */ ad);

	/*!
	 * Ths signal is emitted when ad fetching is failed.
	 *
	 * \param error An AdFetchError indicating the type of error.
	 * \param errorCode an optional error code return by the server
	 * \param errorMessage an optional error message return by the server.
	 */
	void adFetchFailed(SSmaatoAPI::AdFetchError error,  const QString& errorCode = QString(), const QString& errorMessage = QString());

private slots:
	void requestFinished(SHttpRequest* request, SHttpResponse* response);

private:
	bool hasSufficientData();
	bool hasSufficientPermissions();
	QHash<QString, QString> buildParams();
	QHash<QString, QString> buildHeaders();

	QString mAdSpaceId;
	QString mPublisherId;
	AdFormat mFormat;
	bool mFormatStrict;
	int mCoppa;
	int mWidth;
	int mHeight;
	AdDimension mDimension;
	int mAge;
	Gender mGender;
	QString mTags;
	QString mQueryString;
	QString mZip;

	bool mIsFetching;
	QString mSessionId;
	QByteArray mImgData;

	HardwareInfo mHardwareInfo;
	DisplayInfo mDisplayInfo;
	PlatformInfo mPlatformInfo;
	CellularNetworkInfo mCellularNetworkInfo;
	LocaleHandler mLocaleHandler;

	QSharedPointer<SHttpRequest> mAdRequest;
	QSharedPointer<SHttpRequest> mAdImgRequest;
	QSharedPointer<SLocationHandler> mLocationHandler;
	QSharedPointer<SReverseGeocoder> mReverseGeocoder;
	QSharedPointer<SAdResponse> mAdResponse;

};
Q_DECLARE_METATYPE(SSmaatoAPI*)
#endif /* SSMAATOAPI_H_ */
