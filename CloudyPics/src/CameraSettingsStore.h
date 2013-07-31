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
//using namespace bb::system;

class CameraSettingsStore: public QObject {
	Q_OBJECT

public:
	enum CameraSettings {
		CameraMode,
		CameraRollPath,
		CaptureResolution,
		FlashMode,
		FocusMode,
		FocusRegion,
		ZoomLevel,
		SceneMode,
		ShootingMode
	};

	CameraSettingsStore(QObject *parent=0);
	virtual ~CameraSettingsStore();

	Q_INVOKABLE void saveSetting(CameraSettings setting, QVariant value);
	Q_INVOKABLE QVariant restoreSetting(CameraSettings setting);

	Q_INVOKABLE void saveSettings(CameraSettings *settings);
	Q_INVOKABLE void restoreSettings(CameraSettings *settings);

	Q_INVOKABLE void restoreAndApplySettings(Camera *camera);

private:
	QVariantList _cameraSettingsList;
	QSettings _settings;


};

} /* namespace camera */
} /* namespace community */
} /* namespace bb */
#endif /* CAMERASETTINGSSTORE_H_ */
