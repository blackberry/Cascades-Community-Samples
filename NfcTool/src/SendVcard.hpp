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
#ifndef SendVcard_h
#define SendVcard_h

#include <QtCore/QObject>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Control>
#include <bb/cascades/Container>
#include <bb/cascades/Page>

#include "EventLog.hpp"

using namespace bb::cascades;

class SendVcard: public QObject {

	Q_OBJECT
	Q_PROPERTY(QString _first_name READ getFirstName WRITE setFirstName NOTIFY detectFirstNameChanged)
	Q_PROPERTY(QString _last_name READ getLastName WRITE setLastName NOTIFY detectLastNameChanged)
	Q_PROPERTY(QString _address READ getAddress WRITE setAddress NOTIFY detectAddressChanged)
	Q_PROPERTY(QString _email READ getEmail WRITE setEmail NOTIFY detectEmailChanged)
	Q_PROPERTY(QString _mobile READ getMobile WRITE setMobile NOTIFY detectMobileChanged)

public:
	SendVcard();
	virtual ~SendVcard();

	QString getFirstName() const;
	void setFirstName(QString name);
	QString getLastName() const;
	void setLastName(QString name);
	QString getAddress() const;
	void setAddress(QString address);
	QString getEmail() const;
	void setEmail(QString email);
	QString getMobile() const;
	void setMobile(QString mobile);

private:
	QmlDocument* _qml;
	Page* _root;
	EventLog* _eventLog;
	QString _first_name;
	QString _last_name;
	QString _address;
	QString _email;
	QString _mobile;

	void findAndConnectControls();
	void createModules();
	void connectNavigationSignals();

public slots:
	void show();
	void startSendVcardProcess();
	void onFirstNameChanged(QString first_name);
	void onLastNameChanged(QString first_name);
	void onAddressChanged(QString address);
	void onEmailChanged(QString email);
	void onMobileChanged(QString mobile);
	void backFromEventLog();

private slots:

signals:

	void back();
	void detectFirstNameChanged();
	void detectLastNameChanged();
	void detectAddressChanged();
	void detectEmailChanged();
	void detectMobileChanged();
	void showEventLog();

};

#endif // ifndef SendVcard_h
