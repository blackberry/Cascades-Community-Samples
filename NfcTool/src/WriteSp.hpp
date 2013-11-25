/* Copyright (c) 2013 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef WriteSp_h
#define WriteSp_h

#include <QtCore/QObject>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Control>
#include <bb/cascades/Container>
#include <bb/cascades/Page>

#include "EventLog.hpp"

using namespace bb::cascades;

class WriteSp: public QObject {

	Q_OBJECT
	Q_PROPERTY(QString appVersion READ appVersion WRITE setAppVersion NOTIFY detectAppVersionChanged)
	Q_PROPERTY(QString _uri READ getUri WRITE setUri NOTIFY detectUriChanged)
	Q_PROPERTY(QString _text READ getText WRITE setText NOTIFY detectTextChanged)

public:
	WriteSp();
	virtual ~WriteSp();

	QString appVersion() const;
	void setAppVersion(QString appVersion);
	QString getUri() const;
	void setUri(QString uri);
	QString getText() const;
	void setText(QString text);

private:
	QmlDocument* _qml;
	Page* _root;
	EventLog* _eventLog;
	QString _appVersion;
	QString _uri;
	QString _text;

	void findAndConnectControls();
	void createModules();
	void connectNavigationSignals();

public slots:
	void show();
	void startWriteProcess();
	void onUriChanged(QString uri);
	void onTextChanged(QString text);
	void backFromEventLog();

private slots:

signals:
	void back();
	void detectAppVersionChanged();
	void detectUriChanged();
	void detectTextChanged();
	void showEventLog();

};

#endif // ifndef WriteSp_h
