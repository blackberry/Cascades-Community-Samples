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

/*
 * CardManager.cpp
 *
 *  Created on: Apr 18, 2013
 *      Author: pbernhardt
 */

#include "CardManager.h"

namespace bb {
namespace community {
namespace camera {

CardManager::CardManager(QObject *parent) :
		QObject(parent) {

	// Get ready to be invoked
	_pooled = false;
	qDebug() << "+++++++++ Setting up card manager" << endl;

	_invokeManager = new InvokeManager(this);

	connect(_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
			this, SLOT(onInvoked(const bb::system::InvokeRequest&)));

	connect(_invokeManager,
			SIGNAL(cardPooled(const bb::system::CardDoneMessage&)), this,
			SLOT(onCardPooled(const bb::system::CardDoneMessage&)));

	switch (_invokeManager->startupMode()) {
	case ApplicationStartupMode::LaunchApplication:
		_invoked = false;
		break;
	case ApplicationStartupMode::InvokeApplication:
	case ApplicationStartupMode::InvokeViewer:
	case ApplicationStartupMode::InvokeCard:

		_invoked = true;
		break;
	}

}

CardManager::~CardManager() {
	// TODO Auto-generated destructor stub
}

void CardManager::onInvoked(const bb::system::InvokeRequest&) {
	if(_pooled) {
		// TODO: Consider connecting this into CameraSettingsManager
		_camera->open(CameraUnit::Rear);
		_camera->startViewfinder();
		_pooled = false;
	}
}
void CardManager::onCardPooled(const bb::system::CardDoneMessage&) {
	_pooled=true;
	_camera->stopViewfinder();
	_camera->close();
}

void CardManager::triggerCardDone(const QString& data) {

	// Send a signal containing the image path
	if (_invoked) {
		qDebug() << "+++++++++ Sending done message" << endl;
		QFileInfo fileInfo (data);

		CardDoneMessage message;
		message.setData(fileInfo.canonicalFilePath());
		message.setDataType("text/plain");
		message.setReason("Photo captured");
		_camera->stopViewfinder();
		_camera->close();
		_invokeManager->sendCardDone(message);
	}
}

InvokeManager* CardManager::getInvokeManager() {
	return _invokeManager;
}

void CardManager::setCamera(Camera *camera) {
	_camera = camera;
	connect(camera, SIGNAL(photoSaved (const QString&, quint64)), this,
			SLOT(triggerCardDone(const QString&)));
}

} /* namespace camera */
} /* namespace community */
} /* namespace bb */
