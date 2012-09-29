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
#ifndef EventLog_h
#define EventLog_h

#include <QtCore/QObject>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Control>
#include <bb/cascades/Container>
#include <bb/cascades/Page>
#include <bb/cascades/TouchEvent>
#include <bb/cascades/ListItemListener>
#include <bb/cascades/databinding/datamodel.h>
#include <bb/cascades/controls/listitemprovider.h>

using namespace bb::cascades;

class EventLog: public QObject {

	Q_OBJECT
	Q_PROPERTY(QString message READ getMessage WRITE setMessage NOTIFY detectMessageChanged)
	Q_PROPERTY(QString appVersion READ appVersion WRITE setAppVersion NOTIFY detectAppVersionChanged)

public:
	virtual ~EventLog();
	static EventLog* getInstance();

	QString getMessage() const;
	void setMessage(const QString &message);

	QString appVersion() const;
	void setAppVersion(QString appVersion);

private:
	EventLog();
	static EventLog* _instance;
	QmlDocument *_qml;
	Page *_root;
	QString _message;
	QString _appVersion;

	ListItemProvider* itemprovider;
	DataModel* datamodel;

	void findAndConnectControls();
	void insertListView();

public slots:
	void show();
	void onBackNavigationTriggered();

private slots:

signals:
	void back();
	void hide();
	void detectAppVersionChanged();
	void detectMessageChanged();
};

#endif // ifndef EventLog_h
