/*
 * SSmaatoAdView.h
 *
 *  Created on: Oct 21, 2013
 *      Author: jial
 */

#ifndef SSMAATOADVIEW_H_
#define SSMAATOADVIEW_H_

#include <bb/cascades/CustomControl>
#include <bb/cascades/Container>
#include <bb/cascades/Label>
#include <bb/cascades/ImageView>
#include <bb/cascades/WebView>
#include <bb/cascades/Sheet>
#include <bb/cascades/TapEvent>
#include <bb/cascades/WebNavigationRequest>
#include <bb/cascades/WebLoadRequest>
#include <bb/device/DisplayInfo>

#include <QVariantMap>
#include <QObject>
#include <QMetaType>
#include <QVariant>
#include <QString>
#include <QSharedPointer>
#include <QUrl>

#include "SSmaatoAPI.h"
#include "SSmaatoAd.h"
#include "SSmaatoBridge.h"

using namespace bb::cascades;
using bb::device::DisplayInfo;

/*!
 * @brief SSmaatoAdView class displays ads and handles touch events and javascript function callbacks.
 *
 * SSmaatoAdView supports the following ad formats
 * - Img
 *
 * The user must at least provide a Ad Space Id and a Publisher Id in order to get ads from server.
 *
 *
 * The size of the view behaves as follows:
 * - The requested ad size will scale to fit into the size of the SSmaatoAdView maintaining proportions.
 * - If the ad request failed to return any valid ad from server, the SSmaatoAdView hides itself.
 *
 * The SSmaatoAdView does not detect orientation change. The user is responsible for it.
 *
 * Clicking on an Img ad, if an action link is available, the appropriate external application is launched to handle the url.
 *
 * An ad is retrieved automatically if the object is created in QML and the creationCompleted() signal is emitted. If the object is created in c++, the user needs to call updateAd() to get an ad from server.
 *
 * ## Permissions
 * ### Mandatory permissions
 * - access_internet
 *
 * ## Examples
 * Register SSmaatoAdView in the application class.
  @code

 	 qmlRegisterType<SSmaatoAdView>("smaatosdk", 1, 0, "SSmaatoAdView");

  @endcode
 *
 *
 * Create a SSmaatoAdView in qml
  @code
 	SSmaatoAdView {
	   id: adView
	   adSpaceId: "AdSpaceId"
	   publisherId: "PublisherId"
	   viewSize: SSmaatoAdView.AdViewSizeNormal
	   format: 1
	   coppa: 1
	   age: 30
	   gender: SSmaatoAPI.Male
	   tags: "motorsport,travel"
	   queryString: "london,manchester"
	   zip: "Zip"
    }

   @endcode
 *
 * @since SmaatoSDK 1.0.0
 */
class SSmaatoAdView : public bb::cascades::CustomControl {
	Q_OBJECT
	Q_ENUMS(AdViewSize)

	/*!
	 * The ad space id.
	 */
	Q_PROPERTY(QString adSpaceId READ getAdSpaceId WRITE setAdSpaceId)

	/*!
	 * The publisher id
	 */
	Q_PROPERTY(QString publisherId READ getPublisherId WRITE setPublisherId)

	/*!
	 * The format of the ad to be requested.
	 */
	Q_PROPERTY(int format READ getFormat WRITE setFormat)

	/*!
	 * The AdViewSize value that controls the size of the SSmaatoAdView.
	 */
	Q_PROPERTY(SSmaatoAdView::AdViewSize viewSize READ getAdViewSize WRITE setAdViewSize NOTIFY adViewSizeChanged)

	/*!
	 * The coppa of the ad to be requested.
	 *
	 * The possible values are 0 and 1.
	 *
	 * The default value is 0.
	 */
	Q_PROPERTY(int coppa READ getCoppa WRITE setCoppa)

	/*!
	 * The user's age.
	 *
	 * The default value is 0, indicating no age is set.
	 */
	Q_PROPERTY(int age READ getAge WRITE setAge)

	/*!
	 * The user's gender.
	 *
	 * The default value is UnknownGender, indicating no gender is set.
	 */
	Q_PROPERTY(SSmaatoAPI::Gender gender READ getGender WRITE setGender)

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

	/*!
	 * The number of seconds between refreshing ads (default is 0, meaning do not auto refresh)
	 */
	Q_PROPERTY(int autoRefreshPeriod READ getAutoRefreshPeriod WRITE setAutoRefreshPeriod)

public:
	/*!
	 * The predefined ad view sizes.
	 */
	enum AdViewSize {
		/*!
		 * 6.4:1 banner.
		 */
		AdViewSizeNormal,
		/*!
		 * 1.2:1 rectangle.
		 */
		AdViewSizeMedium,
		/*!
		 * Full screen rectangle, whose width is larger than height.
		 */
		AdViewSizeFullScreenPortrait,
		/*!
		 * Full screen rectangle, whose height is larger than width.
		 */
		AdViewSizeFullScreenLandscape

	};

	/*!
	 * Constructs a SSmaatoAdView.
	 */
	SSmaatoAdView();

	/*!
	 * Destroys the SSmaatoAdView.
	 */
	virtual ~SSmaatoAdView();

	/*!
	 * Sets the ad space id.
	 *
	 * The ad space id is mandatory for getting ads from server.
	 *
	 * \param id The ad space id.
	 */
	void setAdSpaceId(const QString& id);

	/*!
	 * Gets the ad space id.
	 *
	 * \return The ad space id.
	 */
	QString getAdSpaceId() const;

	/*!
	 * Sets the publisher id.
	 *
	 * The publisher id is mandatory for getting ads from server.
	 *
	 * \param id The publisher id.
	 */
	void setPublisherId(const QString& id);

	/*!
	 * Gets the publisher id.
	 *
	 * \return The publisher id.
	 */
	QString getPublisherId() const;

	/*!
	 * Sets the ad view size. The SSmaatoAdView is cleared when this function is called. The current ad will be removed.
	 *
	 * The default is AdViewSizeNormal.
	 *
	 * \param size An AdViewSize value.
	 */
	void setAdViewSize(AdViewSize size);

	/*!
	 * Gets the ad view size.
	 *
	 * \return The AdViewSize value.
	 */
	AdViewSize getAdViewSize() const;

	/*!
	 * Sets the format of the ad to be requested.
	 *
	 * The default value is AdFormatImg
	 *
	 * \param format The ad format.
	 */
	void setFormat(int format);

	/*!
	 * Returns the ad format.
	 *
	 * \return The ad format.
	 */
	int getFormat() const;

	/*!
	 * Gets the format strict value.
	 *
	 * \return The format strict value.
	 */
	bool isFormatStrict() const;

	/*!
	 * Sets the coppa. The coppa can be 0 or 1.
	 *
	 * The default is 1.
	 *
	 * \param coppa The coppa value.
	 */
	void setCoppa(int coppa);

	/*!
	 * Gets the coppa value.
	 *
	 * \return The coppa value.
	 */
	int getCoppa() const;

	/*!
	 * Sets the user's age. The age is optional for getting ads from server.
	 *
	 * \param age The user's age.
	 */
	void setAge(int age);

	/*!
	 * Gets the age.
	 *
	 * \return The age.
	 */
	int getAge() const;

	/*!
	 * Sets the user's gender. The gender is optional for getting ads from server.
	 *
	 * The default value is UnknownGender, indicating no gender is set.
	 *
	 * \param gender The user's gender.
	 */
	void setGender(SSmaatoAPI::Gender gender);

	/*!
	 * Gets the gender value.
	 *
	 * \return The Gender value.
	 */
	SSmaatoAPI::Gender getGender() const;

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
	 * Gets the auto refresh period in seconds.
	 *
	 * \return The auto refresh period in seconds.
	 */
	int getAutoRefreshPeriod() const;

	/*!
	 * Sets the auto refresh period in seconds.
	 *
	 * \param autoRefreshPeriod The auto refresh period in seconds.
	*/
	void setAutoRefreshPeriod(const int autoRefreshPeriod);

	/*!
	 * Triggers an ad update. If the SSmaatoAd is currently requesting an ad from server, it will start a new request as soon as the current one is finished.
	 */
	Q_INVOKABLE void updateAd();

	virtual void timerEvent(QTimerEvent * event);

signals:
	/*!
	 * Emitted when an ad update is finished.
	 *
	 * \param success True if the ad update is successful. Otherwise, false.
	 */
	void adUpdated(bool success);

	/*!
	 * triggerAdUpdate must not be handled by the user of the SSmaatoAdView. It is consumed by SSmaatoAdView itself.
	 */
	void triggerAdUpdate();

	void adViewSizeChanged(SSmaatoAdView::AdViewSize adViewSize);


private slots:
	void onAdViewCreationCompleted();

	void onAdFetchFinished(const QVariant& /* SSmaatoAd* */ ad);
	void onAdFetchFailed(SSmaatoAPI::AdFetchError error,  const QString& errorCode = QString(), const QString& errorMessage = QString());

	void onTapped (bb::cascades::TapEvent *event);

	void onAdUpdateTriggered();

	void onWebViewNavigationRequested (bb::cascades::WebNavigationRequest *request);
	void onWebViewMessageReceived(const QVariantMap& message);
	void onWebviewLoadingChanged (bb::cascades::WebLoadRequest *loadRequest);

	void expandRichmediaView();
	void closeRichmediaView();
	void resizeRichmediaView(int width, int height);
	void onFullscreen();
	void onThumbnail();
	void onVisibleChanged(bool);

private:
	enum AdViewMode {
		AdViewModeNormal,
		AdViewModeRichMedia,
		AdViewModeRichmediaExpanded,
		AdViewModeRichmediaResized
	};

	void showImageView(const Image& image);
	void showTextView(const QString& txt);
	void showRichmediaView(const QString& richmedia);
	void hideAdView(bool clearData);
	void clearAdView();
	void clearImageView();
	void clearTextView();
	void clearRichMediaView();
	FontSize::Type findProperFontSize();
	QSize findSizeForAdViewSize(AdViewSize adViewSize);
	SSmaatoAPI::AdDimension findAdDimensionForAdViewSize(AdViewSize adViewSize);
	void resizeAdView(int width, int height);
	void resizeWebView(float width, float height);

	QSize getScreenSize();

	DisplayInfo mDisplayInfo;

	ImageView* mImageView;
	Label* mTextView;
	WebView* mWebView;
	Sheet* mRichmediaSheet;

	Container* mRootContainer;

	QSharedPointer<SSmaatoAPI> mApi;
	QSharedPointer<SSmaatoAd> mAd;
	QSharedPointer<SSmaatoBridge> mBridge;

	AdViewSize mAdViewSize;
	bool mUpdateAgain;
	AdViewMode mAdViewMode;
	int mAutoRefreshPeriod;
	int mRefreshTimerId;

};

#endif /* SSMAATOADVIEW_H_ */
