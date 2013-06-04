/*
 * CameraRollManager.h
 *
 *  Created on: Apr 5, 2013
 *      Author: pbernhardt
 */

#ifndef CAMERAROLLMANAGER_H_
#define CAMERAROLLMANAGER_H_

#include <QObject>
#include <QSettings>
#include <bb/system/SystemListDialog>
#include <bb/cascades/multimedia/camera>
#include <bb/cascades/multimedia/CameraSettings>


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

public slots:
	void promptCameraRollPath();

signals:
	void cameraChanged();

private:

	void createCameraDialog();

	Camera *_camera;
	CameraSettings *_cameraSettings;
	QSettings _settings;
	QString _email;
	SystemListDialog *_cameraRollListDialog;
	QVariantList _cameraRollList;


};

} /* namespace camera*/
} /* namespace community */
} /* namespace bb */
#endif /* CAMERAROLLMANAGER_H_ */
