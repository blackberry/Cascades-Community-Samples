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

#include "applicationui.hpp"
#include "CameraRollManager.h"
#include "CameraSettingsStore.h"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/multimedia/Camera>
#include <bb/cascades/multimedia/CameraSettings>;

using namespace bb::cascades;
using namespace bb::system;
using namespace bb::cascades::multimedia;
using namespace bb::community::camera;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
		QObject(app) {

	qDebug() << "+++++++++ ApplicationUI Started" << endl;

	//Handle things if we are invoked as a card
	_cardManager = new CardManager(this);

	// Register our camera types in QML
	qmlRegisterType<CameraRollManager>("bb.community.camera", 1, 0,
			"CameraRollManager");
	qmlRegisterType<CameraSettingsStore>("bb.community.camera", 1, 0,
			"CameraSettingsStore");
	//qRegisterMetaType<bb::cascades::multimedia::CameraSettings*>();

	// create scene document from main.qml asset
	// set parent to created document to ensure it exists for the whole application lifetime
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	qDebug() << "+++++++++ Creating root object and starting app" << endl;
	// create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();
	// set created root object as a scene

	qDebug() << "+++++++++ Finding camera" << endl;
	Camera* camera = root->findChild<Camera*>("camera");
	qDebug() << "+++++++++ Camera: " << camera << endl;
	_cardManager->setCamera(camera);
	app->setScene(root);
}

