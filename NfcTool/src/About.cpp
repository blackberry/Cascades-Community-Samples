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
#include <bb/cascades/Button>
#include <bb/cascades/TextArea>

#include "About.hpp"
#include "Settings.hpp"
#include "StateManager.hpp"
#include "Navigator.hpp"

#include <qdebug>

using namespace bb::cascades;

About::About() : _appVersion(QString(Settings::AppVersion)) {
	qDebug() << "constructing About...";
	_qml = QmlDocument::create("about.qml");
	_qml->setContextProperty("_about", this);
	_root = _qml->createRootNode<AbstractPane>();
}

About::~About() {}

void About::show() {
	qDebug() << "About: show()";
	qDebug() << "finding NavigationPane object from cache";
	Navigator* nav = Navigator::getInstance();
	NavigationPane* navpane = nav->getNavigationPane();
	// refresh _root
	_root = _qml->createRootNode<AbstractPane>();
	navpane->push(_root);
}

QString About::appVersion() const {
	return _appVersion;
}

void About::setAppVersion(QString appVersion) {
	if (_appVersion.compare(appVersion) == 0)
		return;

	_appVersion = appVersion;

	emit detectAppVersionChanged();
}
