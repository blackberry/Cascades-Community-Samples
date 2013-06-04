/*
 * CameraRollManager.cpp
 *
 *  Created on: Apr 5, 2013
 *      Author: pbernhardt
 */

#include "CameraRollManager.h"

#include <bb/system/SystemListDialog>
#include <bb/system/SystemUiResult.hpp>
#include <bb/data/JsonDataAccess>

#include <QDir>
#include <QFile>

namespace bb {
namespace community {
namespace camera {

using namespace bb::system;
using namespace bb::data;

CameraRollManager::CameraRollManager(QObject *parent) {
	// Get settings data (last used path, last used email)
	// Load camera path list from JSON
	// Wait for camera attachment
	_cameraSettings = new CameraSettings(this);
}

CameraRollManager::~CameraRollManager() {
	// TODO Auto-generated destructor stub
}

void CameraRollManager::setCamera(Camera *camera) {
	//Clear old signals?

	_camera = camera;

	qDebug() << "+++++++ Camera set " << endl;
	_email = _settings.value("email").toString();

	//Connect to signals
	//QObject::connect(_camera, onPhotoSaved(const QString, quint64 length), this, sendPhoto(const QString, quint64 length))

	if (_cameraRollListDialog == NULL) {
		createCameraDialog();
	}

	//Set camera roll path to path in settings data, if available


}

Camera* CameraRollManager::getCamera() {
	return _camera;
}

void CameraRollManager::createCameraDialog() {
	_cameraRollListDialog = new SystemListDialog("Save", "Cancel");

	JsonDataAccess jda;

	QString jsonPath = getAppDirectory() + "/app/native/assets/cameraroll.json";
	_cameraRollList = jda.load(jsonPath).toList();

	_cameraRollListDialog->setTitle("Choose camera roll path");
	if (jda.hasError()) {
		qDebug() << "+++++++ JSON Error: " << jda.error();
	}

	qDebug() << "+++++++ JSON Path: " << jsonPath << endl;
	for (int i = 0; i < _cameraRollList.length(); i++) {
		QVariantMap cameraRollItem = _cameraRollList[i].toMap();
		qDebug() << "+++++++ JSON Data: " << cameraRollItem["description"]
				<< endl;
		/*
		 if (QFile::exists(
		 cameraRollItem["required path"].toString().replace("~/",
		 getAppDirectory() + "/"))) {*/
		//listDialog->appendHeader(cameraRollItem["name"].toString());
		_cameraRollListDialog->appendSeparator(
				cameraRollItem["name"].toString());
		bool pathExists = QFile::exists(
				cameraRollItem["required path"].toString().replace("~/",
						getAppDirectory() + "/"));
		_cameraRollListDialog->appendItem(
				cameraRollItem["description"].toString(),
				pathExists);
		/*		} else {
		 qDebug() << "+++++++ Cannot find: " <<cameraRollItem["required path"].toString().replace("~/",
		 getAppDirectory() + "/") << endl;
		 }*/

	}
}

void CameraRollManager::promptCameraRollPath() {
	// Create dialog

	// Populate with radio buttons from path list

	// Set selected button to current camera roll path

	// Create email text area and validate

	// Display dialog

	// Set path and email from dialog results

	// Save path and email to settings

	// Create a SystemListDialog with these characteristics:
	// The "confirmLabel" (OK button) is set to "My favorite".
	// The "cancelLabel" (CANCEL button) is set to "I don't watch movies."
	// This dialog box doesn't have a custom button.

	SystemUiResult::Type result = _cameraRollListDialog->exec();
	if (result == SystemUiResult::ConfirmButtonSelection) {

		qDebug() << "+++++++ ID selected: "
				<< _cameraRollListDialog->selectedIndices()[0] / 2 << endl;
		QVariantMap cameraRollItem =
				_cameraRollList[_cameraRollListDialog->selectedIndices()[0] / 2].toMap();
		QString cameraPath = cameraRollItem["path"].toString().replace("~/",
				getAppDirectory() + "/");

		//if (!QDir::opendir(cameraPath)))
		QDir dir;
		if (!QFile::exists(cameraPath)) {
			dir.mkpath(cameraPath);
		}
		qDebug() << "+++++++ Setting camera roll to: " << cameraPath << endl;

		qDebug() << "+++++++ Camera: " << _camera << endl;
		_camera->getSettings(_cameraSettings);
		qDebug() << "+++++++ Settings got" << endl;
		_cameraSettings->setCameraRollPath(cameraPath);
		qDebug() << "+++++++ Setting path" << endl;
		_camera->applySettings(_cameraSettings);
		qDebug() << "+++++++ Settings applied" << endl;

	}
	//_cameraRollListDialog->deleteLater();

}

QString CameraRollManager::getAppDirectory() {

	return QDir::currentPath();
}

} /* namespace camera */
} /* namespace community */
} /* namespace bb */
