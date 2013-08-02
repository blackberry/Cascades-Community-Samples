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
#include "CameraSettingsType.h"

namespace bb {
namespace community {
namespace camera {

using namespace bb::cascades;
using namespace bb::cascades::multimedia;
//using namespace bb::system;

class CameraSettingsStore: public QObject {
	Q_OBJECT

/*	Q_ENUMS(CameraSettingType)

	enum CameraSettingType {
		CameraMode,
		CameraRollPath,
		CaptureResolution,
		FlashMode,
		FocusMode,
		FocusRegion,
		ZoomLevel,
		SceneMode,
		ShootingMode,
		CameraSettingTypeCount
	};*/

public:
	CameraSettingsStore(QObject *parent=0);
	virtual ~CameraSettingsStore();

	Q_INVOKABLE void saveSetting(CameraSettingType::Type setting, QVariant value);
	Q_INVOKABLE QVariant loadSetting(CameraSettingType::Type setting);
	Q_INVOKABLE void deleteSetting(CameraSettingType::Type setting);

	Q_INVOKABLE void populateSetting(CameraSettingType::Type setting, CameraSettings *settings);

	Q_INVOKABLE void saveSettings(bb::cascades::multimedia::CameraSettings* settings);
	Q_INVOKABLE void saveSettings(QObject* settings);
	Q_INVOKABLE void populateSettings(bb::cascades::multimedia::CameraSettings *settings);
	Q_INVOKABLE void clearSettings(bb::cascades::multimedia::CameraSettings *settings);

	Q_INVOKABLE void restoreAndApplySettings(bb::cascades::multimedia::Camera *camera);

	Q_INVOKABLE bool isEmpty();

private:
	//QVariantList _cameraSettingsList;
	QSettings* _qSettings;


};

} /* namespace camera */
} /* namespace community */
} /* namespace bb */
#endif /* CAMERASETTINGSSTORE_H_ */
