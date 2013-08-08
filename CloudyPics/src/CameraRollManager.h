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

namespace bb {
namespace community {
namespace camera {

using namespace bb::system;

class CameraRollManager: public QObject {
	Q_OBJECT

public:
	CameraRollManager(QObject *parent=0);
	virtual ~CameraRollManager();

	Q_INVOKABLE QString getAppDirectory();

public slots:
	void promptCameraRollPath();
	void createCameraRollDialog(QString cameraRollPath);

signals:
	void cameraRollPathUpdated(QString cameraRollPath);
	void cameraRollPathNotUpdated();

private:

	SystemListDialog *_cameraRollListDialog;
	QVariantList _cameraRollList;

};

} /* namespace camera*/
} /* namespace community */
} /* namespace bb */
#endif /* CAMERAROLLMANAGER_H_ */
