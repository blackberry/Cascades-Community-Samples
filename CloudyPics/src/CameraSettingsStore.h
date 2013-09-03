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
 * CameraSettingsStore.h
 *
 *  Created on: Jul 31, 2013
 *      Author: pbernhardt
 */

#ifndef CAMERASETTINGSSTORE_H_
#define CAMERASETTINGSSTORE_H_

#include <qobject.h>
#include <QSettings>
#include <bb/cascades/multimedia/CameraSettings>
#include <bb/cascades/multimedia/Camera>

namespace bb {
namespace community {
namespace camera {

using namespace bb::cascades;
using namespace bb::cascades::multimedia;

class CameraSettingsStore: public QObject {
	Q_OBJECT

	Q_ENUMS(CameraSettingsType)

public:
	// All the settings that can be saved
	enum CameraSettingsType {
		CameraMode,
		CameraRollPath,
		CaptureResolution,
		FlashMode,
		FocusMode,
		FocusRegion,
		ZoomLevel,
		SceneMode,
		ShootingMode,
		CameraSettingsTypeCount // Not a setting, makes it easy to go through all settings
	};

	CameraSettingsStore(QObject *parent=0);
	virtual ~CameraSettingsStore();

	// Save, load, or delete an individual setting from the file system
	Q_INVOKABLE void saveSetting(CameraSettingsType setting, QVariant value);
	Q_INVOKABLE QVariant loadSetting(CameraSettingsType setting);
	Q_INVOKABLE void deleteSetting(CameraSettingsType setting);

	// Loads a setting and applies it to the supplied setting object
	Q_INVOKABLE void populateSetting(CameraSettingsType setting, CameraSettings *settings);

	// Saves all settings to the file system
	//  QObject version is a work around to enable passing a CameraSettings object in QML, which otherwise is not available currently.
	Q_INVOKABLE void saveSettings(QObject* settings);
	Q_INVOKABLE void saveSettings(bb::cascades::multimedia::CameraSettings *settings);
	// Loads all saved settings from the file system and applies them to a settings object
	Q_INVOKABLE void populateSettings(QObject* settings);
	Q_INVOKABLE void populateSettings(bb::cascades::multimedia::CameraSettings *settings);
	// Clears out the settings object
	Q_INVOKABLE void clearSettings();

	// Takes all saved settings and applies them to a camera
	Q_INVOKABLE void restoreAndApplySettings(bb::cascades::multimedia::Camera *camera);

	// Checks if there are any saved settings
	Q_INVOKABLE bool isEmpty();

private:
	// This is where all our settings are stored
	QSettings* _qSettings;

};

} /* namespace camera */
} /* namespace community */
} /* namespace bb */

// Makes sure we can use these classes and enums in QML
QML_DECLARE_TYPE(bb::community::camera::CameraSettingsStore)
QML_DECLARE_TYPE(bb::community::camera::CameraSettingsStore::CameraSettingsType)

#endif /* CAMERASETTINGSSTORE_H_ */
