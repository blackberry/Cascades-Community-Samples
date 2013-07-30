/*
 * CameraRollManager.cpp
 *
 *  Created on: Apr 5, 2013
 *      Author: pbernhardt
 */

#include "CameraSettingsManager.h"

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

CameraSettingsManager::CameraSettingsManager(QObject *parent) {
	// Get settings data (last used path, last used email)
	// Load camera path list from JSON
	// Wait for camera attachment
	_cameraSettings = new CameraSettings(this);
}

CameraSettingsManager::~CameraSettingsManager() {
	// TODO Auto-generated destructor stub
}

void CameraSettingsManager::setCamera(Camera *camera) {
	//Clear old signals?

	_camera = camera;

	qDebug() << "+++++++ Camera set " << endl;
	_cameraRollPath = _settings.value(CAMERA_ROLL_PATH).toString();
	if (_cameraRollPath != NULL) {
		qDebug() << "+++++++ Saved path: " << _cameraRollPath;
		setCameraRollPath(_cameraRollPath);
	}

	_cameraRollIndex = _settings.value(CAMERA_ROLL_INDEX).toInt();

	//Connect to signals
	//QObject::connect(_camera, onPhotoSaved(const QString, quint64 length), this, sendPhoto(const QString, quint64 length))

	if (_cameraRollListDialog == NULL) {
		createCameraRollDialog();
	}

	//Set camera roll path to path in settings data, if available

}

Camera* CameraSettingsManager::getCamera() {
	return _camera;
}

void CameraSettingsManager::createCameraRollDialog() {
	delete _cameraRollListDialog;
	_cameraRollListDialog = new SystemListDialog("Save", "Cancel");

	if (_cameraRollList.length() == 0) {
		JsonDataAccess jda;

		QString jsonPath = getAppDirectory()
				+ "/app/native/assets/cameraroll.json";
		_cameraRollList = jda.load(jsonPath).toList();

		_cameraRollListDialog->setTitle("Choose camera roll path");
		if (jda.hasError()) {
			qDebug() << "+++++++ JSON Error: " << jda.error();
		}
	}
	qDebug() << "+++++++ Camera roll path: " << _cameraRollPath << endl;

	for (int i = 0; i < _cameraRollList.length(); i++) {
		QVariantMap cameraRollItem = _cameraRollList[i].toMap();
		qDebug() << "+++++++ JSON Data: " << cameraRollItem["path"] << endl;
		/*
		 if (QFile::exists(
		 cameraRollItem["required path"].toString().replace("~/",
		 getAppDirectory() + "/"))) {*/
		//listDialog->appendHeader(cameraRollItem["name"].toString());
		//_cameraRollListDialog->appendSeparator(cameraRollItem["name"].toString());
		bool pathExists = QFile::exists(
				cameraRollItem["required path"].toString().replace("~/",
						getAppDirectory() + "/"));
		_cameraRollListDialog->appendItem(cameraRollItem["name"].toString(),
				pathExists, i == _cameraRollIndex); //_cameraRollPath == cameraRollItem["path"].toString().replace("~/", getAppDirectory()));
		/*		} else {
		 qDebug() << "+++++++ Ca
		 nnot find: " <<cameraRollItem["required path"].toString().replace("~/",
		 getAppDirectory() + "/") << endl;
		 }*/

	}
	qDebug() << "+++++++ _cameraRollListDialog created. " << endl;
}

void CameraSettingsManager::promptCameraRollPath(bool alsoSetCameraRollPath) {
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

	//_cameraRollListDialog->

	SystemUiResult::Type result = _cameraRollListDialog->exec();
	if (result == SystemUiResult::ConfirmButtonSelection) {
		qDebug() << "+++++ Confirmed" << endl;
		qDebug() << "+++++ ID selected: "
				<< _cameraRollListDialog->selectedIndices()[0] << endl;

		QVariantMap cameraRollItem =
				_cameraRollList[_cameraRollListDialog->selectedIndices()[0]].toMap();
		QString cameraPath = cameraRollItem["path"].toString().replace("~/",
				getAppDirectory() + "/");

		//if (!QDir::opendir(cameraPath)))
		QDir dir;
		if (!QFile::exists(cameraPath)) {
			dir.mkpath(cameraPath);
		}

		if (alsoSetCameraRollPath) {
			if (setCameraRollPath(cameraPath)) {
				_cameraRollIndex = _cameraRollListDialog->selectedIndices()[0];
				_settings.setValue(CAMERA_ROLL_INDEX, _cameraRollIndex);
				createCameraRollDialog();
			}
		}
		emit cameraRollPathUpdated(cameraPath);
	} else {
		emit cameraRollPathNotUpdated();
	}
}

bool CameraSettingsManager::setCameraRollPath(QString path) {

	qDebug() << "+++++++ Setting camera roll to: " << path << endl;

	qDebug() << "+++++++ Camera: " << _camera << endl;
	_camera->getSettings(_cameraSettings);
	qDebug() << "+++++++ Settings got" << endl;
	_cameraSettings->setCameraRollPath(path);
	qDebug() << "+++++++ Setting path" << endl;
	if (_camera->applySettings(_cameraSettings)) {
		_cameraRollPath = path;
		_settings.setValue(CAMERA_ROLL_PATH, _cameraRollPath);
		qDebug() << "+++++++ Camera roll set" << endl;
		return true;
	} else {
		qDebug() << "+++++++ Error setting camera roll to " << path << endl;
		emit cameraRollError("Unable to set camera roll to " + path);
		return false;
	}

}

QString CameraSettingsManager::getAppDirectory() {

	return QDir::currentPath();
}

QString CameraSettingsManager::getCameraRollPath() {
	return _cameraRollPath;
}

} /* namespace camera */
} /* namespace community */
} /* namespace bb */
