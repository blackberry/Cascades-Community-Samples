/*
 * SSmaatoBridge.h
 *
 *  Created on: Oct 31, 2013
 *      Author: jial
 */

#ifndef SSMAATOBRIDGE_H_
#define SSMAATOBRIDGE_H_

#include <QObject>
#include <QMetaType>
#include <QVariantMap>
#include <QByteArray>
#include <bb/cascades/WebNavigationRequest>
#include <QUrl>
#include <QList>
#include <bb/system/SystemDialog>
#include <bb/cascades/WebView>
#include <QSize>
#include <QRect>

class SHttpRequest;
class SHttpResponse;

using namespace bb::cascades;
using namespace bb::system;

/*!
 * @brief SSmaatoBridge class handles the javascript messages and urls sent by the richmedia ads through WebView.
 *
 * This class understand the Ormma javascript library and the Smaato's javascript library for richmedia ads.
 *
 * A SSmaatoBridge object can be reused between richmedia ads. But, reset() must be called to clear any recorded data for the current ad.
 *
 * ## Permissions
 * ### Optional permissions for the sdk to be fully functioning
 * - access_internet
 * - access_pimdomain_calendars
 * - access_shared
 *
 * @since SmaatoSDK 1.0.0
 */
class SSmaatoBridge: public QObject {
	Q_OBJECT
public:
	/*!
	 * The state of the Ormma richmedia ad.
	 */
	enum OrmmaState {
		/*! The Ormma richmedia ad is in the default state */
		OrmmaStateDefault,
		/*! The Ormma richmedia ad is in the expanded state */
		OrmmaStateExpanded,
	};

	/*
	 * Constructs a SSmaatoBridge object.
	 */
	SSmaatoBridge(QObject* parent = 0);

	/*
	 * Destroys the SSmaatoBridge object.
	 */
	virtual ~SSmaatoBridge();

	/*!
	 * Handles the javascript bridge message sent by the richmedia ad. With some of the messages, the SSmaatoBridge needs to call the javascript functions through the WebView to make the ad fully functioning.
	 *
	 * The messages handled by the SSmaatoBridge are
	 * - foundORMMAAd: Stores a boolean value localy.
	 * - expand: Emits expand signal.
	 * - legacyExpand: Emits expand signal if the ad is not an Ormma Richmedia ad.
	 * - close: Emits close signal
	 * - playVideo: Launches external applicatino to play the video at the given url.
	 * - setResizeProperties: Stores the given width and height.
	 * - resize: Emits resize(width, height) signal with the width and height values from setResizeProperties.
	 * - createCalendarEvent: Launches device calendar application with prefilled event details.
	 * - storePicture: Downloads and Stores the picture at given url into "smaato" folder in the device album.
	 *
	 * \param message The message sent by the richmedia ad. The message contains a title and optionally the message data.
	 * \param webView The originating webview
	 */
	Q_INVOKABLE void handleMessage(const QVariantMap& message, WebView* webView);

	/*!
	 * This function process the give url and perform different actions accordingly.
	 *
	 * It launches external applications if the url is with the schemes below.
	 * - http
	 * - https
	 * - mailto
	 * - sms
	 * - tel
	 * - appworld
	 *
	 * \return Returns true if the url is handled by this function. Otherwise, returns false.
	 */
	Q_INVOKABLE bool handleUrl(const QUrl& url);

	/*!
	 * Checks if the ad is an Ormma richmedia ad.
	 *
	 * \return Returns true if the ad is an Ormma richmedia ad. Otherwise, returns false.
	 */
	Q_INVOKABLE bool isOrmma();

	/*!
	 * Resets the SSmaatoBridge object to clear any data recorded for the current ad.
	 *
	 * Reset() must be called before the SSmaatoBridge object is used for a different ad.
	 */
	Q_INVOKABLE void reset();

	/*!
	 * Notifies the richmedia ad about the size changing. It should be called when the ad view is expanded and resized back to normal.
	 *
	 * This function calls the window.ormmaview.fireChangeEvent(state) in Smaato javascript library.
	 *
	 * e.g. window.ormmaview.fireChangeEvent({state:'default',size:{width:768,height:1280}});
	 *
	 * \param state The state, which the richmedia ad should be in.
	 * \param width The width of the adview.
	 * \param height The height of the adview.
	 * \param webView The WebView showing the richmedia ad.
	 */
	Q_INVOKABLE void notifySizedChanged(OrmmaState state, int width, int height, WebView* webView);

	/*!
	 * Notifies the richmedia ad that the ad view has finished loading. It sets the richmedia ad's state to default and passes some information to it, including
	 * - state: default
	 * - size: the ad view size
	 * - maxSize
	 * - screenSize
	 * - defaultPosition
	 *
	 * \param size The size of the ad view.
	 * \param maxSize The max size that the ad can use.
	 * \param screenSize The device screen size.
	 * \param defaultPosition the position of the ad banner.
	 * \param webView The WebView showing the richmedia ad.
	 */
	Q_INVOKABLE void notifyBannerLoaded(QSize size, QSize maxSize, QSize screenSize, QRect defaultPosition, WebView* webView);

private slots:
	void onStorePictureDialogFinished(bb::system::SystemUiResult::Type value);
	void onPictureRequestFinished(SHttpRequest* request, SHttpResponse* response);


signals:
	/*!
	 * Emitted when
	 * - The expand message is received.
	 * - The legacyExpand message is received and the ad is not a Ormma Richmedia ad.
	 */
	void expand();

	/*!
	 * Emitted when close message is received.
	 */
	void close();

	/*!
	 * Emitted when resize message is received.
	 *
	 * \param width The width that the ad view should be resized to.
	 * \param height The hiehgt that the ad view should be resized to.
	 */
	void resize(int width, int height);

	/*!
	 * Emitted when foundORMMAAd message is received.
	 */
	void foundOrmma();

private:
	void invokeTarget(const QUrl& url, const QString& target = QString(), const QString& mimeType = QString(),  const QString& action = QString(), const QByteArray& data = QByteArray());
	void invokeTarget(const QString& url, const QString& target = QString(), const QString& mimeType = QString(),  const QString& action = QString(), const QByteArray& data = QByteArray());
	void createCalendarEvent(const QVariantMap& propertiesmap);
	void storePicture(const QString url);

	int mWidth;
	int mHeight;

	QSharedPointer<SystemDialog> mStorePictureDialog;
	QString mStorePictureUrl;
	QList<SHttpRequest*> mPictureRequests;
	bool mIsOrmma;

};
Q_DECLARE_METATYPE(SSmaatoBridge*)

#endif /* SSMAATOBRIDGE_H_ */
