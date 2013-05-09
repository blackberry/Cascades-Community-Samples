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

#include "PasswordSample.hpp"
#include "Utilities.hpp"
#include "DataManager.hpp"
#include "Timer.hpp"
#include "SecurityManager.hpp"
#include "sbreturn.h"
#include "husha2.h"
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/Container>
#include <bb/cascades/SceneCover>

using namespace bb::cascades;

PasswordSample::PasswordSample(bb::cascades::Application *app) :
		QObject(app) {

	qmlRegisterType<Timer>("CustomTimer", 1, 0, "Timer");

	// create an instance of SecurityManager
	SecurityManager* sec_mgr = SecurityManager::getInstance();
	int rc = sec_mgr->initSecurity();
	if (rc != SB_SUCCESS) {
		qDebug() << "XXXX initSecurity failed: rc=" << rc;
	}

	// connect to aboutToQuit signal so we can clean up on exiting
	connect(app, SIGNAL(aboutToQuit()), this, SLOT(onAboutToQuit()));

	// set the organisation and app name. Used by QSettings

	bb::cascades::Application::setOrganizationName("RIM");
	bb::cascades::Application::setApplicationName("PasswordSample");

	// create scene document from main.qml asset
	// set parent to created document to ensure it exists for the whole application lifetime
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	// create an instance of DataManager
	DataManager* data_mgr = DataManager::getInstance();
	// and add to context so it's available from QML
	qml->setContextProperty("data_mgr", data_mgr);
	qml->setContextProperty("sec_mgr", sec_mgr);

	// set up the application's cover
	QmlDocument *qmlCover = QmlDocument::create("asset:///cover.qml").parent(this);
	qmlCover->setContextProperty("sec_mgr", sec_mgr);
	Container *rootContainer = qmlCover->createRootObject<Container>();
	SceneCover *cover = SceneCover::create().content(rootContainer);
	Application::instance()->setCover(cover);

	QObject *obj = rootContainer->findChild<QObject*>((const QString) "cover");
	QObject::connect(sec_mgr, SIGNAL(loggedInChanged(const QVariant &)), obj, SLOT(loggedInChanged(const QVariant &)));
	sec_mgr->setLoggedIn(false);

	// create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();
	// set created root object as a scene
	app->setScene(root);
}

void PasswordSample::onAboutToQuit() {
	qDebug() << "XXXX onAboutToQuit";
	SecurityManager* sec_mgr = SecurityManager::getInstance();
	sec_mgr->endSecurity();
}
