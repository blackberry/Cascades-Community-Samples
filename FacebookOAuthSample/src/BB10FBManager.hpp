/*
 * BB10FBManager.hpp
 *
 *  Created on: Aug 11, 2012
 *      Author: alanwong
 */

#ifndef BB10FBMANAGER_HPP_
#define BB10FBMANAGER_HPP_

#include <QObject>
#include <QtNetwork/qnetworkreply.h>
#include <QtNetwork/qnetworkaccessmanager.h>
#include <bb/data/JsonDataAccess>
#include <bb/cascades/controls/webview.h>
#include <bb/cascades/controls/customdialog.h>
#include <bb/cascades/controls/container.h>
#include <bb/cascades/layouts/docklayout.h>
#include <bb/cascades/layouts/docklayoutproperties.h>
#include <bb/cascades/resources/color.h>

#include "User.hpp"

using namespace bb::cascades;
using namespace bb::data;

#define ERROR_LOGIN 1
#define ERROR_PUBLISH_POST 2

class BB10FBManager : public QObject
{
	Q_OBJECT
public:
	BB10FBManager(QString, CustomDialog*);
	virtual ~BB10FBManager();

    void login();
    void logout();
    void getUserProfile();
    void publishPost(QString, QString);
    void publishPostWithImage(QString, QString, QString);
    void publishPostWithLink(QString, QString, QString, QString, QString, QString);
    void publishPostWithVideo(QString , QString, QString);
    void getUserFriends();

    QString getAccessToken() const {
		return _access_token;
	}

	void setAccessToken(QString accessToken) {
		_access_token = accessToken;
	}

public slots:
	void slotWebViewURLChanged();
	void responseReceived(const QString&);

signals:
	void onUserProfileRetrieved(User* );
	void onUserFriendsRetrieved(QList<User>*);
	void onPublishPostSuccess();
	void onLoginSuccess();
	void onLogoutSuccess();
	void onErrorEvent(int, QString);

private:
	User* parseUserProfile(QString data);
	QList<User>* parseUserFriends(QString data);
	int parsePublishPost(QString data);
	void initAuthWebview();

	QNetworkAccessManager* manager;
	QNetworkReply* reply;
	QNetworkRequest request;
	QString _access_token;
	QString _appId;
	CustomDialog* _authDialog;
	Container* _authContainer;
	WebView* _webview;
	bool _isLoggedIn;
	QSignalMapper* requestMapper;
};

#endif /* BB10FBMANAGER_HPP_ */
