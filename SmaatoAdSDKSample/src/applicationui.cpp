/*
 * Copyright (c) 2014 BlackBerry Limited.
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
#include <bb/cascades/Container>
#include <bb/cascades/Button>
#include <bb/cascades/ScreenIdleMode>
#include <bb/cascades/Window>
#include <SSmaatoAdView.h>
#include <SSmaatoAPI.h>
#include <QTimer>
#include <bb/data/JsonDataAccess>

using namespace bb::cascades;
using namespace bb::data;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
		QObject(app) {

	successCount = settings.value("successes", 0).toInt();
	failCount = settings.value("failures", 0).toInt();

	qmlRegisterType<SSmaatoAdView>("smaatosdk", 1, 0, "SSmaatoAdView");
	qmlRegisterType<SSmaatoAPI>("smaatosdk", 1, 0, "SSmaatoAPI");

	// Create scene document from main.qml asset, the parent is set
	// to ensure the document gets destroyed properly at shut down.
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
	qml->setContextProperty("app", this);

	// Create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();

	// Set created root object as the application scene
	app->setScene(root);

	adContainer = root->findChild<Container*>("imAContainer");
	clearStatsAction = root->findChild<DeleteActionItem*>("clearStats");

	if (!adContainer) {
		qDebug() << "Nope";
		return;
	}

	QObject::connect(clearStatsAction, SIGNAL(triggered()), SLOT(clearStats()));
	setQmlResultCounts();

	Application::instance()->mainWindow()->setScreenIdleMode(ScreenIdleMode::KeepAwake);

	adImage = root->findChild<ImageView*>("adImageView");

	mApi = new SSmaatoAPI("65819976", "923870645");
	QObject::connect(mApi, SIGNAL(adFetchFinished(QVariant)), this, SLOT(onAdFetchFinished(QVariant)));
	QObject::connect(mApi, SIGNAL(adFetchFailed(SSmaatoAPI::AdFetchError, QString, QString)), this, SLOT(onAdFetchFailed(SSmaatoAPI::AdFetchError, QString, QString)));
	mApi->setFormat(SSmaatoAPI::AdFormatImg);
	//mApi->setFormatStrict(true);
	mApi->fetchAd();
}

void ApplicationUI::onAdFetchFinished(const QVariant /* SSmaatoAd* */ ad) {
	if (!ad.isNull()) {
			SSmaatoAd* adptr = ad.value<SSmaatoAd*>(); SSmaatoAd::AdType adtype = adptr->getType();
			if (adtype == SSmaatoAd::AdTypeImg) {
				adptr->showAd();
				adImage->setVisible(true);
				adImage->setImage(adptr->getAdImage());
			}
	}
}

void ApplicationUI::onAdFetchFailed(SSmaatoAPI::AdFetchError error, const QString code, const QString message) {
	qDebug() << "Image fetch failed";
	adImage->setVisible(false);
}

void ApplicationUI::refresh() {
	mApi->fetchAd();
}

void ApplicationUI::clearStats() {
	successCount = 0;
	failCount = 0;
	setQmlResultCounts();
}

void ApplicationUI::setQmlResultCounts() {
	adContainer->setProperty("successCount", successCount);
	adContainer->setProperty("failCount", failCount);
	settings.setValue("successes", successCount);
	settings.setValue("failures", failCount);
}

void ApplicationUI::setSettingsParams(QString paramSet) {
	JsonDataAccess jda;
	QVariantMap parameters = jda.loadFromBuffer(paramSet).value<QVariantMap>();
	settings.setValue("age", parameters.value("age"));
	settings.setValue("gender", parameters.value("gender"));
	settings.setValue("zip", parameters.value("zip"));
	settings.setValue("tags", parameters.value("tags"));
	settings.setValue("adSpaceId", parameters.value("adSpaceId"));
	settings.setValue("publisherId", parameters.value("publisherId"));
}

QString ApplicationUI::getSettingsParams() {
	QString result;

	QVariantMap parameters;
	parameters["age"] = settings.value("age", -1);	//default age value for AdView
	parameters["gender"] = settings.value("gender", "");
	parameters["zip"] = settings.value("zip", "");
	parameters["tags"] = settings.value("tags", "");
	parameters["adSpaceId"] = settings.value("adSpaceId", "");
	parameters["publisherId"] = settings.value("publisherId", "");

	QVariant jsonData = QVariant(parameters);
	JsonDataAccess jda;
	jda.saveToBuffer(jsonData, &result);
	return result;
}

