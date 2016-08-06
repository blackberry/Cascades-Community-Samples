/*
 * BB10FBManager.cpp
 *
 *  Created on: Aug 11, 2012
 *      Author: alanwong
 */

#include "BB10FBManager.hpp"

BB10FBManager::BB10FBManager(QString appId, CustomDialog* authDialog) {
	_authDialog = authDialog;
	_appId = appId;

	//SignalMapper to map requests<->responses
	requestMapper = new QSignalMapper(this);
	connect(requestMapper, SIGNAL(mapped(const QString &)), this, SLOT(responseReceived(const QString &)));

	_isLoggedIn = false;

	//set up webview for authentication
	initAuthWebview();
}

BB10FBManager::~BB10FBManager() {
}

//TODO: Add exit/back button in auth dialog
void BB10FBManager::initAuthWebview() {
	//set up qnam and reusable request
	manager = new QNetworkAccessManager(this);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

	//TODO: Use non-hardcoded values
	_webview = WebView::create();
	_authContainer = Container::create();
	_authDialog->setContent(_authContainer);

	_authContainer->setPreferredHeight(1280);
	_authContainer->setPreferredWidth(768);
	_authContainer->setBackground(Color::fromRGBA(0.0, 0.0, 0.0, 0.5));
	_authContainer->setLayout(DockLayout::create());

	_webview->setPreferredHeight(700);
	_webview->setPreferredWidth(700);
	_webview->setLayoutProperties(DockLayoutProperties::create().horizontal(HorizontalAlignment::Center).vertical(VerticalAlignment::Center));

	_authContainer->add(_webview);
	QObject::connect(_webview, SIGNAL(urlChanged()), this, SLOT(slotWebViewURLChanged()));
}

void BB10FBManager::login() {
	if (_appId == "") {
		emit onErrorEvent(ERROR_LOGIN, "AppID Empty");
	}
	else {
		if(!_isLoggedIn) {
			_authDialog->setVisible(true);
			_webview->setUrl(QUrl("https://www.facebook.com/dialog/oauth?client_id="+_appId+"&redirect_uri=https://www.facebook.com/connect/login_success.html&scope=publish_stream&display=touch&response_type=token"));
		}
	}
}

void BB10FBManager::logout() {
	_webview->setUrl(QUrl("https://www.facebook.com/logout.php?next=https://www.facebook.com/connect/login_success.html&access_token="+getAccessToken()));
}

void BB10FBManager::slotWebViewURLChanged() {
	//For list of FB Auth URLs and redirects, see authURLs.txt
	QString newURL = _webview->url().toString();
	qDebug() << "\n" << newURL;

	if (!_isLoggedIn && newURL.contains("login_success") && newURL.contains("access_token=")) {  //successful login
		int startIndex = newURL.indexOf("access_token=") + 13;
		int endIndex = newURL.indexOf("&expires");
		setAccessToken(newURL.mid(startIndex, endIndex - startIndex));
		_authDialog->setVisible(false);
		_isLoggedIn = true;
		emit onLoginSuccess();
	}
	else if (!_isLoggedIn && newURL.contains("user_denied") && !newURL.contains("login.php")) { //app install denied.  (not a facebook login req url)
		_authDialog->setVisible(false);
		emit onErrorEvent(ERROR_LOGIN, "User denied installing the app");
	}
	else if (!_isLoggedIn && newURL.contains("home.php")) {	//clicked on install app, leave app on permission dialog
		_authDialog->setVisible(false);
		emit onErrorEvent(ERROR_LOGIN, "User denied permissions");
	}
	else if (_isLoggedIn && newURL.contains("login_success") && !newURL.contains("access_token=")) { //same redirect url, but lack of access token (LOGOUT detection)
		_authDialog->setVisible(false);
		_isLoggedIn = false;
		emit onLogoutSuccess();
	}
}

void BB10FBManager::getUserProfile() {
	request.setUrl(QUrl("https://graph.facebook.com/me?access_token="+getAccessToken()));
	reply = manager->get(request);
	requestMapper->setMapping(reply, "CMD_USER_PROFILE"); //map type of request (identifier) in SignalMapper
	connect(reply, SIGNAL(finished()), requestMapper, SLOT(map()));
}

void BB10FBManager::getUserFriends() {
	request.setUrl(QUrl("https://graph.facebook.com/me/friends?fields=id,name,first_name,last_name,link,username&access_token="+ getAccessToken()));
	reply = manager->get(request);
	requestMapper->setMapping(reply, "CMD_USER_FRIENDS");
	connect(reply, SIGNAL(finished()), requestMapper, SLOT(map()));
}

void BB10FBManager::publishPost(QString id, QString message) {
	if (id == "" || message == "") {
		emit onErrorEvent(ERROR_PUBLISH_POST, "ID or Message missing");
	}
	else {
		request.setUrl(QUrl("https://graph.facebook.com/" + id + "/feed?access_token="+ getAccessToken()));
		QByteArray data;
		QUrl params;

		params.addQueryItem("message", message);
		data = params.encodedQuery();
		reply = manager->post(request, data);
		requestMapper->setMapping(reply, "CMD_PUBLISH_POST");
		connect(reply, SIGNAL(finished()), requestMapper, SLOT(map()));
	}
}

void BB10FBManager::publishPostWithImage(QString id, QString message, QString picture) {
	if (id == "" || message == "" || picture == "") {
		emit onErrorEvent(ERROR_PUBLISH_POST, "ID, Message or Picture missing");
	}
	else {
		request.setUrl(QUrl("https://graph.facebook.com/" + id + "/feed?access_token="+ getAccessToken()));
		QByteArray data;
		QUrl params;

		params.addQueryItem("message", message);
		params.addQueryItem("picture", picture);
		data = params.encodedQuery();

		reply = manager->post(request, data);
		requestMapper->setMapping(reply, "CMD_PUBLISH_POST");
		connect(reply, SIGNAL(finished()), requestMapper, SLOT(map()));
	}
}

void BB10FBManager::publishPostWithLink(QString id, QString message, QString link, QString name = "", QString caption = "", QString description = "") {
	if (id == "" || message == "" || link == "") {
		emit onErrorEvent(ERROR_PUBLISH_POST, "ID, Message or Link missing");
	}
	else {
		request.setUrl(QUrl("https://graph.facebook.com/" + id + "/feed?access_token="+ getAccessToken()));
		QByteArray data;
		QUrl params;

		params.addQueryItem("message", message);
		params.addQueryItem("link", link);
		params.addQueryItem("name", name);
		params.addQueryItem("caption", caption);
		params.addQueryItem("description", description);
		data = params.encodedQuery();

		reply = manager->post(request, data);
		requestMapper->setMapping(reply, "CMD_PUBLISH_POST");
		connect(reply, SIGNAL(finished()), requestMapper, SLOT(map()));
	}
}

void BB10FBManager::publishPostWithVideo(QString id, QString message, QString source) {
	if (id == "" || message == "" || source == "") {
		emit onErrorEvent(ERROR_PUBLISH_POST, "ID, Message or Video missing");
	}
	else {
		request.setUrl(QUrl("https://graph.facebook.com/" + id + "/feed?access_token="+ getAccessToken()));
		QByteArray data;
		QUrl params;

		params.addQueryItem("message", message);
		params.addQueryItem("source", source);
		data = params.encodedQuery();

		reply = manager->post(request, data);
		requestMapper->setMapping(reply, "CMD_PUBLISH_POST");
		connect(reply, SIGNAL(finished()), requestMapper, SLOT(map()));
	}
}

User* BB10FBManager::parseUserProfile(QString data) {

	JsonDataAccess jda;
	QVariantMap qmap = jda.loadFromBuffer(data).value<QVariantMap>();
	User * user = new User(qmap["id"].toString(), qmap["name"].toString(), qmap["first_name"].toString(), qmap["last_name"].toString(), qmap["link"].toString(), qmap["username"].toString());

	return user;
}

QList<User>* BB10FBManager::parseUserFriends(QString data) {

	JsonDataAccess jda;
	QVariantMap qmap = jda.loadFromBuffer(data).value<QVariantMap>();
	QVariantList raw_list = qmap["data"].value<QVariantList>();

	QList<User>* qlist = new QList<User>();
	QVariantList::iterator i;
	for (i = raw_list.begin(); i != raw_list.end(); ++i) {
		qlist->append(User(i->value<QVariantMap>()["id"].toString(), i->value<QVariantMap>()["name"].toString(), i->value<QVariantMap>()["first_name"].toString(),
				i->value<QVariantMap>()["last_name"].toString(), i->value<QVariantMap>()["link"].toString(), i->value<QVariantMap>()["username"].toString()));
	}

	return qlist;
}

int BB10FBManager::parsePublishPost(QString data) {
	return data.contains("\"id\":") ? 1 : 0;
}

void BB10FBManager::responseReceived(const QString &requestType) {
	reply = (QNetworkReply*)requestMapper->mapping(requestType);
	QString message = reply->readAll();
	reply->deleteLater();
	qDebug() << "\nResponse Received! " << "\nType: " << requestType << "\nMessage: " << message;

	if (requestType == "CMD_USER_PROFILE") {
		emit onUserProfileRetrieved(parseUserProfile(message));
	}
	else if (requestType == "CMD_USER_FRIENDS") {
		emit onUserFriendsRetrieved(parseUserFriends(message));
	}
	else if (requestType == "CMD_PUBLISH_POST") {
		if (parsePublishPost(message)) {
			emit onPublishPostSuccess();
		}
		else {
			emit onErrorEvent(ERROR_PUBLISH_POST, "Bad response.");
		}
	}
}
