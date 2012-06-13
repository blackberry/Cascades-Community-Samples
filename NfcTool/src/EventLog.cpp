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
#include "alternatinglistdatamodel.hpp"
#include "alternatinglistdatamanager.hpp"
#include "Settings.hpp"
#include "Logger.hpp"
#include "Navigator.hpp"

using namespace bb::cascades;

EventLog::EventLog(const QString message) : _appVersion(QString(Settings::AppVersion)) {
	setMessage(message);
	_qml = QmlDocument::create().load("eventlog.qml");
	_qml->setContextProperty("_el", this);

	Logger* logger = Logger::getInstance();

	AlternatingListDataManager* alternatingListDataManager = new AlternatingListDataManager("AlternatingListItemTemplate.qml");
	AlternatingListDataModel* alternatingListDataModel = logger->getDataModel();

	itemmanager = alternatingListDataManager;
	datamodel = alternatingListDataModel;

	_qml->setContextProperty("_modelObj", datamodel);
	_qml->documentContext()->setContextProperty("_model", QVariant::fromValue(datamodel));
	_qml->documentContext()->setContextProperty("_manager", QVariant::fromValue(itemmanager));

	alternatingListDataModel->ascendingSortOrder(false); // we want display to be descending sort order

	// the following would be replaced by an NFCManager singleton being obtained and a function such as
	// setEventLog(alternatingListDataModel) being called. Then NFCManager would log to the model and the details
	// would appear on the event log screen.

	qDebug() << "Constructing EventLog root AbstractPane";
	_root = _qml->createRootNode<AbstractPane>();
	findAndConnectControls();
}

EventLog::~EventLog() {
}

void EventLog::findAndConnectControls() {
	qDebug() << "EventLog: findAndConnectControls()";

	QObject* obj = _root->findChild<QObject*>((const QString)"theEventLog");
	QObject::connect(obj,     SIGNAL(eventLogBackButton()),
			         this,      SLOT(onBackNavigationTriggered()));

	qDebug() << "...done";
}

QString EventLog::getMessage() const {
	return _message;
}

void EventLog::setMessage(const QString &message) {
	_message = message;
}

void EventLog::show() {
	qDebug() << "EventLog: show()";

	qDebug() << "finding NavigationPane object from cache";
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();
	// refresh _root
	_root = _qml->createRootNode<AbstractPane>();
	navpane->push(_root);

	//find and reconnect all the controls again, because we had to refresh the root pointer
	findAndConnectControls();
}

void EventLog::onBackNavigationTriggered() {
	qDebug() << "finding NavigationPane object from cache";
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();
	// refresh _root
	_root = _qml->createRootNode<AbstractPane>();
	navpane->popAndDelete();

	qDebug() << "EventLog:emit hide()";
	emit back();
}

QString EventLog::appVersion() const {
	return _appVersion;
}

void EventLog::setAppVersion(QString appVersion) {
	if(_appVersion.compare(appVersion) == 0)
		return;
	_appVersion = appVersion;
	emit detectAppVersionChanged();
}

