/*
 * CameraRollManager.h
 *
 *  Created on: Apr 5, 2013
 *      Author: pbernhardt
 */

#ifndef CAMERAROLLMANAGER_H_
#define CAMERAROLLMANAGER_H_

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

class CameraRollManager: public QObject {
	Q_OBJECT

	Q_PROPERTY(Camera* camera READ getCamera WRITE setCamera NOTIFY cameraChanged)
public:
	CameraRollManager(QObject *parent=0);
	virtual ~CameraRollManager();



	Q_INVOKABLE void setCamera(bb::cascades::multimedia::Camera *camera);
	Q_INVOKABLE Camera* getCamera();
	Q_INVOKABLE QString getAppDirectory();
	Q_INVOKABLE bool setCameraRoll(QString path);

public slots:
	void promptCameraRollPath();

signals:
	void cameraChanged();
	void cameraRollError(QString error);

private:

	void createCameraDialog();

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
#endif /* CAMERAROLLMANAGER_H_ */
