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
#include "MainMenu.hpp"

using namespace bb::cascades;

MainMenu::MainMenu(Application *app) :  _appVersion(QString(Settings::AppVersion)) {

	qDebug() << "XXXX NFC Tool Lite V" << Settings::AppVersion;
	qDebug() << "XXXX loading main menu qml document";

	_app = app;

	createModules();

	qDebug() << "Invoke Startup Mode: " << _invokeManager->startupMode();

	_qml = QmlDocument::create("asset:///main.qml");
	_qml->setContextProperty("_mainMenu",      this);
	_qml->setContextProperty("_ndefFactory",  _ndefFactory);

	QDeclarativePropertyMap* propertyMap = new QDeclarativePropertyMap;

	propertyMap->insert("AppVersion",       QVariant(QString(Settings::AppVersion)));

	_qml->setContextProperty("_propertyMap", propertyMap);

	_root = _qml->createRootObject<AbstractPane>();
	connectSignals();
	_app->setScene(_root);
}

MainMenu::~MainMenu() {
	qDebug() << "XXXX MainMenu destructor";
	deleteModules();
}

void MainMenu::receivedInvokeRequest(const bb::system::InvokeRequest& request) {

    QByteArray data = request.data();
    QtMobilitySubset::QNdefMessage ndefMessage = QtMobilitySubset::QNdefMessage::fromByteArray(data);

    emit launchReader(QString("Tag read event detected"));

    handleNdefRequest(ndefMessage);
}

void MainMenu::handleNdefRequest(const QtMobilitySubset::QNdefMessage ndefMessage) {

	QList<QtMobilitySubset::QNdefRecord>::const_iterator ndefRecord;

	qDebug() << "XXXX Number of Records: " << ndefMessage.size();
	qDebug() << "XXXX raw message data:" << ndefMessage.toByteArray().toHex();

	for ( ndefRecord = ndefMessage.begin(); ndefRecord != ndefMessage.end(); ndefRecord++) {

		qDebug() << "XXXX TNF: " << ndefRecord->typeNameFormat();
	    qDebug() << "XXXX Type: " << ndefRecord->type();
	    qDebug() << "XXXX Id: " << ndefRecord->id();
	    qDebug() << "XXXX Payload: " << ndefRecord->payload().toHex();

	    if (ndefRecord->typeNameFormat() == QtMobilitySubset::QNdefRecord::NfcRtd) {

			qDebug() << "XXXX TNF Well Known Type detected";

			if (ndefRecord->type() == Settings::NfcRtdSmartPoster) {

				qDebug() << "XXXX Detected a Smart Poster Tag";

				emit message(QString("Detected a Smart Poster Tag"));

				QtMobilitySubset::QNdefMessage sPpayloadNdefMessage = QtMobilitySubset::QNdefMessage::fromByteArray(ndefRecord->payload());
				QList<QtMobilitySubset::QNdefRecord>::const_iterator spNdefRecord;

				for ( spNdefRecord = sPpayloadNdefMessage.begin(); spNdefRecord != sPpayloadNdefMessage.end(); spNdefRecord++) {

					if (spNdefRecord->type() == Settings::NfcRtdUri) {

						emit message(QString("URI: %1").arg(_ndefFactory->getUriFromUriRecordPayload(spNdefRecord->payload())));

					} else if (spNdefRecord->type() == Settings::NfcRtdText) {

						emit message(QString("Encoding: %1").arg(_ndefFactory->getEncodingFromTextRecordPayload(spNdefRecord->payload())));
						emit message(QString("Lang Code: %1").arg(_ndefFactory->getLangFromTextRecordPayload(spNdefRecord->payload())));
						emit message(QString("Text: %1").arg(_ndefFactory->getTextFromTextRecordPayload(spNdefRecord->payload())));
					}
				}

			} else if (ndefRecord->type() == Settings::NfcRtdUri) {
				qDebug() << "XXXX Detected a URI Tag";

				emit message(QString("Detected a URI Tag"));
				emit message(QString("URI: %1").arg(_ndefFactory->getUriFromUriRecordPayload(ndefRecord->payload())));

			} else if (ndefRecord->type() == Settings::NfcRtdText) {
				qDebug() << "XXXX Detected a Text Tag";

				emit message(QString("Encoding: %1").arg(_ndefFactory->getEncodingFromTextRecordPayload(ndefRecord->payload())));
				emit message(QString("Lang Code: %1").arg(_ndefFactory->getLangFromTextRecordPayload(ndefRecord->payload())));
				emit message(QString("Text: %1").arg(_ndefFactory->getTextFromTextRecordPayload(ndefRecord->payload())));
			}
	    } else if (ndefRecord->typeNameFormat() == QtMobilitySubset::QNdefRecord::ExternalRtd) {
			qDebug() << "XXXX TNF External Type detected";

			qDebug() << QString("XXXX Type   : %1").arg(QString(ndefRecord->type()));
			qDebug() << QString("XXXX Payload: %1").arg(QString(ndefRecord->payload()));

			emit message(QString("Detected an External Tag Type"));
			emit message(QString("Type   : %1").arg(QString(ndefRecord->type())));
			emit message(QString("Payload: %1").arg(QString(ndefRecord->payload())));
	    }
	}
}

void MainMenu::connectSignals(){

	qDebug() << "XXXX finding and caching NavigationPane object";

	NavigationPane* navpane = dynamic_cast<NavigationPane*>(_root);

    QObject::connect(_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
                               this,   SLOT(receivedInvokeRequest(const bb::system::InvokeRequest&)));
    QObject::connect(          this, SIGNAL(message(QVariant)),
    		                navpane,   SLOT(message(QVariant)));
	QObject::connect(          this, SIGNAL(launchReader(QVariant)),
			                navpane,   SLOT(launchReader(QVariant)));

	qDebug() << "XXXX ...done";
}

void MainMenu::deleteModules() {
	qDebug() << "XXXX deleting instances of all other modules...";

	if (_invokeManager) {
		delete _invokeManager;
		_invokeManager = 0;
	}

	if (_ndefFactory) {
		delete _ndefFactory;
		_ndefFactory = 0;
	}

	qDebug() << "XXXX ...done";
}

void MainMenu::createModules() {
	qDebug() << "XXXX creating instances of all other modules...";

	_ndefFactory = NdefFactory::getInstance();
    _invokeManager = new bb::system::InvokeManager();

	qDebug() << "XXXX ...done";
}

QString MainMenu::appVersion() const {
	return _appVersion;
}

void MainMenu::setAppVersion(QString appVersion) {
	if (_appVersion.compare(appVersion) == 0)
		return;

	_appVersion = appVersion;

	emit detectAppVersionChanged();
}

void MainMenu::cleanUpOnExit() {
	qDebug() << "XXXX Clean up on application exit";
}

