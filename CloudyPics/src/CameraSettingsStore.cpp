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
 * CameraSettingsStore.cpp
 *
 *  Created on: Jul 31, 2013
 *      Author: pbernhardt
 */

#include "CameraSettingsStore.h"
#include <bb/cascades/multimedia/CameraMode>
#include <bb/cascades/multimedia/CameraFlashMode>
#include <bb/cascades/multimedia/CameraFocusMode>
#include <bb/cascades/multimedia/CameraSceneMode>
#include <bb/cascades/multimedia/CameraShootingMode>

using namespace bb::cascades;
using namespace bb::cascades::multimedia;

namespace bb {
namespace community {
namespace camera {

// These line up with the CameraSettingsType enum as the settings key
const QString CameraSettingName[] = { "cameraMode", "cameraRollPath",
		"captureResolution", "flashMode", "focusMode", "focusRegion",
		"zoomLevel", "sceneMode", "shootingMode" };

CameraSettingsStore::CameraSettingsStore(QObject *parent) :
		QObject(parent) {
	// Prepare our settings object
	_qSettings = new QSettings("Cascades Community Samples", "Cloudy Pics",
			this);
}

CameraSettingsStore::~CameraSettingsStore() {
	_qSettings->deleteLater();
}

void CameraSettingsStore::saveSetting(CameraSettingsType setting,
		QVariant value) {
	qDebug() << "+++++++ Saving " << CameraSettingName[setting] << endl;
	_qSettings->setValue(CameraSettingName[setting], value);
}

QVariant CameraSettingsStore::loadSetting(CameraSettingsType setting) {
	return _qSettings->value(CameraSettingName[setting], 0);
}

void CameraSettingsStore::populateSetting(CameraSettingsType setting,
		CameraSettings *settings) {

	if (_qSettings->contains(CameraSettingName[setting])) {
		QVariant value = _qSettings->value(CameraSettingName[setting]);
		qDebug() << "+++++++ Populating " << CameraSettingName[setting] << " of settings object with " << value << endl;
		QString key = CameraSettingName[setting];

		//Handle each type of setting
		switch (setting) {
		case CameraMode:
			settings->setCameraMode(CameraMode::Type(value.toInt()));
			break;
		case CameraRollPath:
			settings->setCameraRollPath(value.toString());
			break;

		case CaptureResolution:
			settings->setCaptureResolution(value.toSize());
			break;

		case FlashMode:
			settings->setFlashMode(CameraFlashMode::Type(value.toInt()));
			break;

		case FocusMode:
			settings->setFocusMode(CameraFocusMode::Type(value.toInt()));
			break;

		case FocusRegion:
			settings->setFocusRegion(value.toRect());
			break;

		case ZoomLevel:
			settings->setZoomLevel(value.toUInt());
			break;

		case SceneMode:
			settings->setSceneMode(CameraSceneMode::Type(value.toInt()));
			break;

		case ShootingMode:
			settings->setShootingMode(CameraShootingMode::Type(value.toInt()));
			break;

		default:
			break;
		}
	}
}

void CameraSettingsStore::deleteSetting(CameraSettingsType setting) {
	_qSettings->remove(CameraSettingName[setting]);
}

void CameraSettingsStore::saveSettings(QObject* settings) {
	CameraSettings *cameraSettings = qobject_cast<CameraSettings *>(settings);
	saveSettings(cameraSettings);
}

void CameraSettingsStore::saveSettings(bb::cascades::multimedia::CameraSettings* settings) {
	qDebug() << settings << endl;
	//CameraMode
	_qSettings->setValue(CameraSettingName[CameraMode], settings->cameraMode());

	//CameraRollPath
	_qSettings->setValue(CameraSettingName[CameraRollPath],
			settings->cameraRollPath());

	//CaptureResolution
	_qSettings->setValue(CameraSettingName[CaptureResolution],
			settings->captureResolution());

	//FlashMode
	_qSettings->setValue(CameraSettingName[FlashMode], settings->flashMode());

	//FocusMode
	_qSettings->setValue(CameraSettingName[FocusMode], settings->focusMode());

	//FocusRegion,
	_qSettings->setValue(CameraSettingName[FocusRegion],
			settings->focusRegion());

	//ZoomLevel
	_qSettings->setValue(CameraSettingName[ZoomLevel], settings->zoomLevel());

	//SceneMode
	_qSettings->setValue(CameraSettingName[SceneMode], settings->sceneMode());

	//ShootingMode
	_qSettings->setValue(CameraSettingName[ShootingMode],
			settings->shootingMode());

	qDebug() << "+++++++ All settings saved" << endl;
}

void CameraSettingsStore::populateSettings(QObject* settings) {
	CameraSettings *cameraSettings = qobject_cast<CameraSettings *>(settings);
	populateSettings(cameraSettings);
}

void CameraSettingsStore::populateSettings(CameraSettings *settings) {

	qDebug() << "+++++++ Populating settings object" << endl;
	for (int i = 0; i < CameraSettingsTypeCount; i++) {
		populateSetting(CameraSettingsType(i), settings);
	}

}

void CameraSettingsStore::clearSettings() {
	_qSettings->clear();
}

void CameraSettingsStore::restoreAndApplySettings(Camera *camera) {
	qDebug() << "+++++++ Restoring settings" << endl;

	CameraSettings* settings = new CameraSettings(this);
	camera->getSettings(settings);

	qDebug() << "+++++++ Current settings retrieved" << endl;

	populateSettings(settings);

	camera->applySettings(settings);
	settings->deleteLater();
}

bool CameraSettingsStore::isEmpty() {
	return _qSettings->allKeys().length() == 0;
}

} /* namespace camera */
} /* namespace community */
} /* namespace bb */
