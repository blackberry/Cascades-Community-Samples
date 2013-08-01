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

const QString CameraSettingName[] = { "cameraMode", "cameraRollPath",
		"captureResolution", "flashMode", "focusMode", "focusRegion",
		"zoomLevel", "sceneMode", "shootingMode" };

CameraSettingsStore::CameraSettingsStore(QObject *parent) :
		QObject(parent) {
	// TODO Auto-generated constructor stub
	_qSettings = new QSettings("Cascades Community Samples", "Cloudy Pics",
			this);
}

CameraSettingsStore::~CameraSettingsStore() {
	// TODO Auto-generated destructor stub
	_qSettings->deleteLater();
}

void CameraSettingsStore::saveSetting(CameraSettingType setting,
		QVariant value) {
	_qSettings->setValue(CameraSettingName[setting], value);
}

QVariant CameraSettingsStore::restoreSetting(CameraSettingType setting) {
	return _qSettings->value(CameraSettingName[setting], 0);
}

void CameraSettingsStore::applySetting(CameraSettingType setting,
		CameraSettings *settings) {
	if (_qSettings->contains(CameraSettingName[setting])) {
		QVariant value = _qSettings->value(CameraSettingName[setting]);
		QString key = CameraSettingName[setting];

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
			settings->setFlashMode(FlashMode::Type(value.toInt()));
			break;

		case FocusMode:
			settings->setFlashMode(FocusMode::Type(value.toInt()));
			break;

		case FocusRegion:
			settings->setFocusRegion(value.toRect());
			break;

		case ZoomLevel:
			settings->setZoomLevel(value.toUInt());
			break;

		case SceneMode:
			settings->setFlashMode(SceneMode::Type(value.toInt()));
			break;

		case ShootingMode:
			settings->setSceneMode(SceneMode::Type(value.toInt()));
			break;

		default:
			break;
		}
	}
}

void CameraSettingsStore::deleteSetting(CameraSettingType setting) {
	_qSettings->remove(CameraSettingName[setting]);
}

void CameraSettingsStore::saveSettings(CameraSettings *settings) {

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
}

void CameraSettingsStore::restoreSettings(CameraSettings *settings) {

	for (int i = 0; i < CameraSettingTypeCount; i++) {
		applySetting(CameraSettingType(i), settings);
	}

}

void CameraSettingsStore::clearSettings(CameraSettings *settings) {
	_qSettings->clear();
}

void CameraSettingsStore::restoreAndApplySettings(Camera *camera) {
	CameraSettings *settings;
	camera->getSettings(settings);

	restoreSettings(settings);

	camera->applySettings(settings);
}

} /* namespace camera */
} /* namespace community */
} /* namespace bb */
