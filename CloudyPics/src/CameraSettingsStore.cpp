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

void CameraSettingsStore::saveSetting(CameraSettingType::Type setting,
		QVariant value) {
	qDebug() << "+++++++ Saving " << CameraSettingName[setting] << endl;
	_qSettings->setValue(CameraSettingName[setting], value);
}

QVariant CameraSettingsStore::loadSetting(CameraSettingType::Type setting) {
	return _qSettings->value(CameraSettingName[setting], 0);
}

void CameraSettingsStore::populateSetting(CameraSettingType::Type setting,
		CameraSettings *settings) {
	qDebug() << "+++++++ Populating " << CameraSettingName[setting] << " of settings object" << endl;
	if (_qSettings->contains(CameraSettingName[setting])) {
		QVariant value = _qSettings->value(CameraSettingName[setting]);
		QString key = CameraSettingName[setting];

		switch (setting) {
		case CameraSettingType::CameraMode:
			settings->setCameraMode(CameraMode::Type(value.toInt()));
			break;
		case CameraSettingType::CameraRollPath:
			settings->setCameraRollPath(value.toString());
			break;

		case CameraSettingType::CaptureResolution:
			settings->setCaptureResolution(value.toSize());
			break;

		case CameraSettingType::FlashMode:
			settings->setFlashMode(CameraFlashMode::Type(value.toInt()));
			break;

		case CameraSettingType::FocusMode:
			settings->setFocusMode(CameraFocusMode::Type(value.toInt()));
			break;

		case CameraSettingType::FocusRegion:
			settings->setFocusRegion(value.toRect());
			break;

		case CameraSettingType::ZoomLevel:
			settings->setZoomLevel(value.toUInt());
			break;

		case CameraSettingType::SceneMode:
			settings->setSceneMode(CameraSceneMode::Type(value.toInt()));
			break;

		case CameraSettingType::ShootingMode:
			settings->setShootingMode(CameraShootingMode::Type(value.toInt()));
			break;

		default:
			break;
		}
	}
}

void CameraSettingsStore::deleteSetting(CameraSettingType::Type setting) {
	_qSettings->remove(CameraSettingName[setting]);
}

void CameraSettingsStore::saveSettings(QObject* settings) {
	CameraSettings *cameraSettings = qobject_cast<CameraSettings *>(settings);
	saveSettings(cameraSettings);
}

void CameraSettingsStore::saveSettings(bb::cascades::multimedia::CameraSettings* settings) {

	//CameraMode
	_qSettings->setValue(CameraSettingName[CameraSettingType::CameraMode], settings->cameraMode());

	//CameraRollPath
	_qSettings->setValue(CameraSettingName[CameraSettingType::CameraRollPath],
			settings->cameraRollPath());

	//CaptureResolution
	_qSettings->setValue(CameraSettingName[CameraSettingType::CaptureResolution],
			settings->captureResolution());

	//FlashMode
	_qSettings->setValue(CameraSettingName[CameraSettingType::FlashMode], settings->flashMode());

	//FocusMode
	_qSettings->setValue(CameraSettingName[CameraSettingType::FocusMode], settings->focusMode());

	//FocusRegion,
	_qSettings->setValue(CameraSettingName[CameraSettingType::FocusRegion],
			settings->focusRegion());

	//ZoomLevel
	_qSettings->setValue(CameraSettingName[CameraSettingType::ZoomLevel], settings->zoomLevel());

	//SceneMode
	_qSettings->setValue(CameraSettingName[CameraSettingType::SceneMode], settings->sceneMode());

	//ShootingMode
	_qSettings->setValue(CameraSettingName[CameraSettingType::ShootingMode],
			settings->shootingMode());
}

void CameraSettingsStore::populateSettings(CameraSettings *settings) {

	qDebug() << "+++++++ Populating settings object" << endl;
	for (int i = 0; i < CameraSettingType::CameraSettingTypeCount; i++) {
		populateSetting(CameraSettingType::Type(i), settings);
	}

}

void CameraSettingsStore::clearSettings(CameraSettings *settings) {
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
