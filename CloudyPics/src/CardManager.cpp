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