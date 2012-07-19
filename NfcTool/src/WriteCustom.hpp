/* Copyright (c) 2012 Research In Motion Limited.
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
#ifndef WriteCustom_h
#define WriteCustom_h

#include <QtCore/QObject>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Control>
#include <bb/cascades/Container>
#include <bb/cascades/Page>

#include "EventLog.hpp"

using namespace bb::cascades;

class WriteCustom: public QObject {

	Q_OBJECT
	Q_PROPERTY(QString appVersion READ getAppVersion WRITE setAppVersion NOTIFY detectAppVersionChanged)
	Q_PROPERTY(QString _domain READ getDomain WRITE setDomain NOTIFY detectDomainChanged)
	Q_PROPERTY(QString _type READ getType WRITE setType NOTIFY detectTypeChanged)
	Q_PROPERTY(QString _content READ getContent WRITE setContent NOTIFY detectContentChanged)

public:
	WriteCustom();
	virtual ~WriteCustom();

	QString getAppVersion() const;
	void setAppVersion(QString appVersion);
	QString getDomain() const;
	void setDomain(QString domain);
	QString getType() const;
	void setType(QString type);
	QString getContent() const;
	void setContent(QString content);

private:
	QmlDocument* _qml;
	Page* _root;
	EventLog* _eventLog;
	QString _appVersion;
	QString _domain;
	QString _type;
	QString _content;

	void findAndConnectControls();
	void createModules();
	void connectNavigationSignals();

public slots:
	void show();
	void startWriteProcess();
	void onDomainChanged(QString type);
	void onTypeChanged(QString type);
	void onContentChanged(QString type);
	void backFromEventLog();

private slots:

signals:

	void back();
	void detectAppVersionChanged();
	void detectDomainChanged();
	void detectTypeChanged();
	void detectContentChanged();
	void showEventLog();

};

#endif // ifndef WriteCustom_h
