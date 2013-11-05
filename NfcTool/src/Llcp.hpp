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
#ifndef Llcp_h
#define Llcp_h

#include <QtCore/QObject>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Control>
#include <bb/cascades/Container>
#include <bb/cascades/Page>
#include "NfcWorker.hpp"

#include "EventLog.hpp"

using namespace bb::cascades;

class Llcp: public QObject {

	Q_OBJECT

public:
	Llcp();
	virtual ~Llcp();

	Q_INVOKABLE QString getText() const;
	void setText(QString text);

private:
	QmlDocument* _qml;
	Page* _root;
	EventLog* _eventLog;
	QString _llcp_text;
	bool _llcp_send;
	NfcWorker* _workerInstance;

	void findAndConnectControls();

public slots:
	void show();
	void startLlcp();
	void stopLlcp();
	void sendOverLlcp(QString text);
	void onBack();
	void logMessageHere(QString message);

private slots:

signals:

	void showEventLog();

};

#endif // ifndef Llcp_h
