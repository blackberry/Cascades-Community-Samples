/*
 * CameraSettingsManager.h
 *
 *  Created on: Apr 5, 2013
 *      Author: pbernhardt
 */

#ifndef CAMERASETTINGSMANAGER_H_
#define CAMERASETTINGSMANAGER_H_

#define CAMERA_ROLL_PATH "cameraRollPath"
#define CAMERA_ROLL_INDEX "cameraRollIndex"

#include <QObject>
#include <QSettings>
#include <bb/system/SystemListDialog>
#include <bb/cascades/multimedia/camera>
#include <bb/cascades/multimedia/CameraSettings>
#include <qsettings.h>;


namespace bb {
namespace community {
namespace camera {

using namespace bb::cascades;
using namespace bb::cascades::multimedia;
using namespace bb::system;

class CameraSettingsManager: public QObject {
	Q_OBJECT

	Q_PROPERTY(Camera* camera READ getCamera WRITE setCamera NOTIFY cameraChanged)
	Q_PROPERTY(QString cameraRollPath READ getCameraRollPath WRITE setCameraRollPath NOTIFY cameraRollPathUpdated)
public:
	CameraSettingsManager(QObject *parent=0);
	virtual ~CameraSettingsManager();

	Q_INVOKABLE void setCamera(bb::cascades::multimedia::Camera *camera);
	Q_INVOKABLE Camera* getCamera();
	Q_INVOKABLE QString getAppDirectory();
	Q_INVOKABLE bool setCameraRollPath(QString path);
	Q_INVOKABLE QString getCameraRollPath();

public slots:
	void promptCameraRollPath(bool setCameraRollPath);

signals:
	void cameraRollPathUpdated(QString alsoSetCameraRollPath);
	void cameraRollPathNotUpdated();
	void cameraChanged();
	void cameraRollError(QString error);

private:

	void createCameraRollDialog();

	Camera *_camera;
	CameraSettings *_cameraSettings;
	QSettings _settings;
	QString _email;
	SystemListDialog *_cameraRollListDialog;
	QVariantList _cameraRollList;
	QString _cameraRollPath;
	int _cameraRollIndex;

	//const QString CAMERA_ROLL_PATH = "cameraRollPath";


};

} /* namespace camera*/
} /* namespace community */
} /* namespace bb */
#endif /* CAMERASETTINGSMANAGER_H_ */
