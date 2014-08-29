/* Copyright (c) 2013 Chad Tetreault
 * http://palebanana.com - @chadtatro
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

#include "applicationui.hpp"
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>

// bbm
#include "bbm/BBMHandler.hpp"

// invoke
#include <bb/cascades/InvokeQuery>
#include <bb/cascades/Invocation>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>

// toasts
#include <bb/system/SystemToast>

// file saving
#include <Qt/qimage.h>
#include <QDir>
#include <Qt/qlist.h>

using namespace bb::cascades;
using namespace bb::system;
using namespace bb::platform;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
		QObject(app) {

	// prepare the localization
	m_pTranslator = new QTranslator(this);
	m_pLocaleHandler = new LocaleHandler(this);

	// invoke
	m_invokeManager = new bb::system::InvokeManager(this);

	if (!QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()),
			this, SLOT(onSystemLanguageChanged()))) {

		// This is an abnormal situation! Something went wrong!
		// Add own code to recover here
		qWarning() << "Recovering from a failed connect()";
	}
	// initial load
	onSystemLanguageChanged();

	//Kick off BBM Registration.
	//TODO: Define your own UUID here. You can generate one here: http://www.guidgenerator.com/
	const QString uuid(QLatin1String("fd34ee4a-86e1-4a2a-8c42-8c4743463400"));
	BBMHandler *bbmHandler = new BBMHandler(uuid, app);
	bbmHandler->registerApplication();

	// Create scene document from main.qml asset, the parent is set
	// to ensure the document gets destroyed properly at shut down.
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	//Expose the BBM Registration handler to main.qml.
	qml->setContextProperty("bbmHandler", bbmHandler);

	//Expose the ApplicationUI in main.qml
	qml->setContextProperty("app", this);

	// Retrieve the path to the app's working directory
	QString workingDir = QDir::currentPath();
	QDeclarativePropertyMap* dirPaths = new QDeclarativePropertyMap;

	// assets path
	dirPaths->insert("assets",
			QVariant(QString("file://" + workingDir + "/app/native/assets/")));

	// Expose 'dirPaths' to QML
	qml->setContextProperty("dirPaths", dirPaths);

	// Create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();

	// Set created root object as the application scene
	app->setScene(root);
}

// invoke handling
void ApplicationUI::invoke(const QString &target, const QString &action,
		const QString &mimetype, const QString &uri) {

	// Create a new invocation request
	bb::system::InvokeRequest request;
	request.setTarget(target);
	request.setAction(action);
	request.setUri(uri);
	m_invokeManager->invoke(request);
}

// convert the photo to base64 and return the data
QString ApplicationUI::copyToData(const QString& path) const {
	QFile sourceFile(path);

	if (sourceFile.open(QFile::ReadOnly)) {
		QByteArray base64Encoded = sourceFile.readAll();
		QByteArray a = base64Encoded.toBase64();
		sourceFile.close();

		// delete original file from the crop tool here
		// - this deleted the 'edit' version of the file picked
		// - prevents duplicate images from being created
		sourceFile.remove();
		return QString(a);
	}
	return QLatin1String("");
}

// save photo
QString ApplicationUI::savePhoto(const QString& base64) {
	QString fullsizePath;
	QString thumbnailPath;
	QString appFolder(QDir::homePath());
	appFolder.chop(4);

	QString timestamp = QString::number(
			QDateTime::currentMSecsSinceEpoch() / 1000);

	QString photoDirPath = appFolder + "shared/photos/filtermama-lite/";
	QString thumbDirPath = appFolder + "shared/photos/filtermama-lite/.tmp/";

	QDir appDir(photoDirPath);
	QDir appTempDir(thumbDirPath);

	// create filtermama dir
	if (!appDir.exists()) {
		appDir.mkpath(photoDirPath);
	}

	// create temp dir
	if (!appTempDir.exists()) {
		appTempDir.mkpath(thumbDirPath);
	}

	// build fullsize photo path
	fullsizePath = photoDirPath + "fm-lite-" + timestamp + ".png";

	// build thumbnail path
	thumbnailPath = thumbDirPath + "fm-lite-" + timestamp + ".png";

	// save the fullsize image
	QImage fullsizeImage;
	QByteArray base64Data = base64.toAscii();
	fullsizeImage.loadFromData(QByteArray::fromBase64(base64Data), "PNG");
	fullsizeImage.save(fullsizePath, "PNG");

	// save the thumbnail
	QImage thumbnailImage = fullsizeImage.scaledToHeight(150,
			Qt::SmoothTransformation);
	thumbnailImage.save(thumbnailPath, "PNG");

	Q_EMIT photosChanged(thumbnailPath);
	return fullsizePath;
}

// preapre to share the photo
Invocation* invocation;
void ApplicationUI::shareFile(QString fileName) {
	invocation = Invocation::create(
			InvokeQuery::create().parent(this).uri(
					QUrl::fromLocalFile(fileName)));
	connect(invocation, SIGNAL(armed()), this, SLOT(onArmed()));
	connect(invocation, SIGNAL(finished()), invocation, SLOT(deleteLater()));
}

void ApplicationUI::onArmed() {
	invocation->trigger("bb.action.SHARE");
}

// invoke the photo viewer card when user clicks on photo in their 'feed'
void ApplicationUI::invokePhotoViewer(const QString& path) const {
	InvokeManager invokeManager;
	InvokeRequest request;

	request.setTarget("sys.pictures.card.previewer");
	request.setAction("bb.action.VIEW");
	request.setMimeType("image/png");
	request.setUri(QUrl(path));
	invokeManager.invoke(request);
}

// when system language changes...
void ApplicationUI::onSystemLanguageChanged() {
	QCoreApplication::instance()->removeTranslator(m_pTranslator);
// Initiate, load and install the application translation files.
	QString locale_string = QLocale().name();
	QString file_name = QString("Boilerplate_Cascades_%1").arg(locale_string);
	if (m_pTranslator->load(file_name, "app/native/qm")) {
		QCoreApplication::instance()->installTranslator(m_pTranslator);
	}
}

// util for encoding strings
QByteArray ApplicationUI::encodeQString(const QString& toEncode) const {
	return toEncode.toUtf8();
}

// get list of thumbnails
QString ApplicationUI::getPhotos() const {
	QString appFolder(QDir::homePath());
	appFolder.chop(4);

	QString photoDirPath = appFolder + "shared/photos/filtermama-lite/";
	QString thumbDirPath = appFolder + "shared/photos/filtermama-lite/.tmp/";

	QDirIterator it(thumbDirPath, QDirIterator::Subdirectories);
	QString photos;

	while (it.hasNext()) {
		QByteArray imageFormat = QImageReader::imageFormat(it.next());

		if (imageFormat != "") {
			QFile fullsizePhoto(photoDirPath + it.fileName());

			// if the fullsize version of the thumb doesn't exist, delete the thumb
			if (fullsizePhoto.exists()) {
				photos += "file://" + it.filePath() + ",";
			} else {
				QFile thumbnailPhoto(it.filePath());
				thumbnailPhoto.remove();
			}
		}
	}
	return photos;
}

