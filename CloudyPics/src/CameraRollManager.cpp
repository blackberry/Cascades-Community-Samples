/*
 * CameraRollManager.cpp
 *
 *  Created on: Apr 5, 2013
 *      Author: pbernhardt
 */

#include "CameraRollManager.h"

#include <bb/system/SystemListDialog>
#include <bb/system/SystemUiResult.hpp>
#include <bb/data/JsonDataAccess>

#include <QDir>
#include <QFile>

namespace bb {
namespace community {
namespace camera {

using namespace bb::system;
using namespace bb::data;

CameraRollManager::CameraRollManager(QObject *parent) :
		QObject(parent) {
	//Really should call createCameraRollDialog first
	_cameraRollListDialog = 0;
}

CameraRollManager::~CameraRollManager() {
	_cameraRollListDialog->deleteLater();
}

void CameraRollManager::createCameraRollDialog(QString cameraRollPath) {
	delete _cameraRollListDialog;
	QDir cameraRollDir = QDir(cameraRollPath);
	qDebug() << "+++++++ Current CRP: " << cameraRollDir << endl;
	_cameraRollListDialog = new SystemListDialog("Save", "Cancel");

	if (_cameraRollList.length() == 0) {
		JsonDataAccess jda;

		QString jsonPath = getAppDirectory()
				+ "/app/native/assets/cameraroll.json";
		_cameraRollList = jda.load(jsonPath).toList();

		_cameraRollListDialog->setTitle("Choose camera roll path");
		if (jda.hasError()) {
			qDebug() << "+++++++ JSON Error: " << jda.error();
		}
	}

	for (int i = 0; i < _cameraRollList.length(); i++) {
		QVariantMap cameraRollItem = _cameraRollList[i].toMap();
		qDebug() << "+++++++ JSON Data: " << cameraRollItem["path"] << endl;

		bool pathExists = QFile::exists(
				cameraRollItem["required path"].toString().replace("~/",
						getAppDirectory() + "/"));
		QDir listDir = QDir(cameraRollItem["path"].toString().replace("~/", getAppDirectory() + "/"));
		_cameraRollListDialog->appendItem(cameraRollItem["name"].toString(),
				pathExists, cameraRollDir == listDir);//cameraRollPath == cameraRollItem["path"].toString().replace("~/", getAppDirectory()));
		qDebug() << "+++++++ List CRP: " << listDir << endl;

	}
	qDebug() << "+++++++ _cameraRollListDialog created. " << endl;
}

void CameraRollManager::promptCameraRollPath() {

	if (_cameraRollListDialog == 0) {
		createCameraRollDialog("");
	}
	SystemUiResult::Type result = _cameraRollListDialog->exec();
	if (result == SystemUiResult::ConfirmButtonSelection) {
		qDebug() << "+++++ Confirmed" << endl;
		qDebug() << "+++++ ID selected: "
				<< _cameraRollListDialog->selectedIndices()[0] << endl;

		QVariantMap cameraRollItem =
				_cameraRollList[_cameraRollListDialog->selectedIndices()[0]].toMap();
		QString cameraPath = cameraRollItem["path"].toString().replace("~/",
				getAppDirectory() + "/");

		QDir dir;
		if (!QFile::exists(cameraPath)) {
			dir.mkpath(cameraPath);
		}

		emit cameraRollPathUpdated(cameraPath);
		createCameraRollDialog(cameraPath);
	} else {
		emit cameraRollPathNotUpdated();
	}
}

QString CameraRollManager::getAppDirectory() {

	return QDir::currentPath();
}

} /* namespace camera */
} /* namespace community */
} /* namespace bb */
