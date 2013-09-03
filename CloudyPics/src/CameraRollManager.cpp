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

#define APP_HOME_PATH "~/"

namespace bb {
namespace community {
namespace camera {

using namespace bb::system;
using namespace bb::data;

CameraRollManager::CameraRollManager(QObject *parent) :
		QObject(parent) {

	_cameraRollListDialog = 0;
	_relativePathFixer = QRegExp("^(?!\/)");
}

CameraRollManager::~CameraRollManager() {
	_cameraRollListDialog->deleteLater();
}

void CameraRollManager::createCameraRollDialog(QString cameraRollPath) {
	qDebug() << "+++++++ Creating _cameraRollDialog" << endl;
	if (_cameraRollListDialog != 0) {
		qDebug() << "+++++++ Deleting old _cameraRollDialog" << endl;
		_cameraRollListDialog->deleteLater();
	}

	QDir cameraRollDir = QDir(cameraRollPath);
	qDebug() << "+++++++ Current CRP: " << cameraRollDir << endl;
	_cameraRollListDialog = new SystemListDialog("Save", "Cancel");
	_cameraRollListDialog->setTitle("Choose camera roll path");

	//Make sure we only load the JSON once
	// In the future, could load it from the data folder, and listen for changes..

	if (_cameraRollList.length() == 0) {
		JsonDataAccess jda;

		QString jsonPath = getAppDirectory()
				+ "/app/native/assets/cameraroll.json";
		_cameraRollList = jda.load(jsonPath).toList();


		if (jda.hasError()) {
			qDebug() << "+++++++ JSON Error: " << jda.error();
		}
	}

	// Add each potential path to our dialog
	for (int i = 0; i < _cameraRollList.length(); i++) {
		QVariantMap cameraRollItem = _cameraRollList[i].toMap();
		qDebug() << "+++++++ JSON Data: " << cameraRollItem["path"] << endl;

		// If the "required path" exists, it means that option is valid and should be selectable
		// eg, if the Dropbox folder is there, we can save to Dropbox
		bool pathExists = QFile::exists(
				cameraRollItem["required path"].toString().replace(_relativePathFixer,
						getAppDirectory() + "/"));
		// We'll also check and see if this option is the current path, and should therefore be selected
		QDir listDir = QDir(
				cameraRollItem["path"].toString().replace(_relativePathFixer,
						getAppDirectory() + "/"));
		_cameraRollListDialog->appendItem(cameraRollItem["name"].toString(),
				pathExists, cameraRollDir == listDir);
		qDebug() << "+++++++ List CRP: " << listDir << endl;

	}
	qDebug() << "+++++++ _cameraRollListDialog created. " << endl;
}

void CameraRollManager::promptCameraRollPath(QString currentCameraRollPath) {

	// If we haven't explicitly created the dialog before, or we have a path we want to make sure is selected,
	// Create one now. The current camera roll path will not be selected though if we haven't specified it.
	qDebug() << "+++++++ Creating camera roll dialog if needed. " << endl;
	if (_cameraRollListDialog == 0 || currentCameraRollPath != 0) {
		createCameraRollDialog(currentCameraRollPath);
	}
	SystemUiResult::Type result = _cameraRollListDialog->exec();
	if (result == SystemUiResult::ConfirmButtonSelection) {
		qDebug() << "+++++ Confirmed" << endl;
		qDebug() << "+++++ ID selected: "
				<< _cameraRollListDialog->selectedIndices()[0] << endl;

		QVariantMap cameraRollItem =
				_cameraRollList[_cameraRollListDialog->selectedIndices()[0]].toMap();
		QString cameraPath = cameraRollItem["path"].toString().replace(_relativePathFixer,
				getAppDirectory() + "/");

		// We may need to create the actual final path (ie, add a "Photo" or "Camera" folder)
		QDir dir;
		if (!QFile::exists(cameraPath)) {
			dir.mkpath(cameraPath);
		}

		emit cameraRollPathUpdated(cameraPath);

/*		//Remake the dialog with the new, updated path.
		// This could actually be the wrong path, if the cameraRollPathUpdated signal doesn't connect to
		// something that successfully updates the path.
		if (currentCameraRollPath != 0) {
			createCameraRollDialog(cameraPath);
		}*/

	} else {
		emit cameraRollPathNotUpdated();
	}
	_cameraRollListDialog->deleteLater();
	_cameraRollListDialog = 0;
}

QString CameraRollManager::getAppDirectory() {

	// This is not easily available in QML otherwise.
	return QDir::currentPath();
}

} /* namespace camera */
} /* namespace community */
} /* namespace bb */
