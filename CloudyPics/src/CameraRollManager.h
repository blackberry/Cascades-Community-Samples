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
	// Prompts the user to pick the camera roll path.
	//  Fires cameraRollPathUpdated with the updated path if they select one
	//  Fires cameraRollPathNotUpdated if they cancel
	//  Will create the camera roll dialog
	void promptCameraRollPath(QString currentCameraRollPath = "");

	// Creates the camera roll dialog. Loads the options from the filesystem
	// if they aren't already in memory. This can be a tad slow, so best to do
	// this when the app has a free moment.
	void createCameraRollDialog(QString cameraRollPath = "");

signals:
	// Fired after calling promptCameraRollPath when the user has selected a camera roll path
	void cameraRollPathUpdated(QString cameraRollPath);
	// Fired after calling promptCameraRollPath when the user has cancelled out
	void cameraRollPathNotUpdated();

private:

	SystemListDialog *_cameraRollListDialog;
	QVariantList _cameraRollList;
	QRegExp _relativePathFixer;

};

} /* namespace camera*/
} /* namespace community */
} /* namespace bb */
#endif /* CAMERAROLLMANAGER_H_ */
