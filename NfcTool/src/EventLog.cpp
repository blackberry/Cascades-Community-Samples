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
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Control>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/Button>
#include <bb/cascades/ListView>
#include <bb/cascades/StackListLayout>

#include "EventLog.hpp"
#include "StateManager.hpp"
#include "alternatinglistdatamodel.hpp"
#include "alternatinglistdatamanager.hpp"
#include "Settings.hpp"
#include "Logger.hpp"
#include "Navigator.hpp"

using namespace bb::cascades;

EventLog* EventLog::_instance;

EventLog* EventLog::getInstance() {
	if (_instance == 0) {
		_instance = new EventLog;
	}
	return _instance;
}

EventLog::EventLog() :
		_appVersion(QString(Settings::AppVersion)) {
	_qml = QmlDocument::create("asset:///eventlog.qml");
	_qml->setContextProperty("_el", this);

	Logger* logger = Logger::getInstance();

	AlternatingListDataManager* alternatingListDataManager =
			new AlternatingListDataManager("asset:///AlternatingListItemTemplate.qml");
	AlternatingListDataModel* alternatingListDataModel = logger->getDataModel();

	itemprovider = alternatingListDataManager;
	datamodel = alternatingListDataModel;

	_qml->setContextProperty("_modelObj", datamodel);
	_qml->documentContext()->setContextProperty("_model",
			QVariant::fromValue(datamodel));
	_qml->documentContext()->setContextProperty("_manager",
			QVariant::fromValue(itemprovider));

	alternatingListDataModel->ascendingSortOrder(false);

	qDebug() << "XXXX Constructing EventLog root Page";
	_root = _qml->createRootObject<Page>();
	findAndConnectControls();
}

EventLog::~EventLog() {
	qDebug() << "XXXX Eventlog destructor";
	_instance = 0;
}

void EventLog::findAndConnectControls() {
	qDebug() << "XXXX EventLog: findAndConnectControls()";

	QObject* obj = _root->findChild<QObject*>((const QString) "theEventLog");
	QObject::connect(obj, SIGNAL(eventLogBackButton()), this,
			SLOT(onBackNavigationTriggered()));

	qDebug() << "XXXX ...done";
}

QString EventLog::getMessage() const {
	return _message;
}

void EventLog::setMessage(const QString &message) {
	_message = message;
}

void EventLog::show() {
	qDebug() << "XXXX EventLog: show()";

	qDebug() << "XXXX finding NavigationPane object from cache";
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();

	Logger::getInstance()->clearLog();

	StateManager* state_mgr = StateManager::getInstance();

	_root = _qml->createRootObject<Page>();
	navpane->push(_root);

	state_mgr->setEventLogShowing(true);

	findAndConnectControls();
}

void EventLog::onBackNavigationTriggered() {
	qDebug() << "XXXX finding NavigationPane object from cache";
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();
	_root = _qml->createRootObject<Page>();
	navpane->pop();

	StateManager* state_mgr = StateManager::getInstance();
	state_mgr->setEventLogShowing(false);

	qDebug() << "XXXX EventLog:emit hide()";
	emit back();
}

QString EventLog::appVersion() const {
	return _appVersion;
}

void EventLog::setAppVersion(QString appVersion) {
	if (_appVersion.compare(appVersion) == 0)
		return;
	_appVersion = appVersion;
	emit detectAppVersionChanged();
}

