/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
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

static ApplicationUI *appInstance = 0;
static void btEvent(const int event, const char *btAddr, const char *eventData);
static void btLeAdvertisementCb(const char *bdaddr, int8_t rssi, const char *data, int len, void *userData);

static void btLeAdvertisementCb(const char *bdaddr, int8_t rssi, const char *data, int len, void *userData)
{
	Q_UNUSED(userData)

	if (appInstance) {
		appInstance->parseBeaconData(data, len, rssi, bdaddr);
	}
}

static void btEvent(const int event, const char *btAddr, const char *eventData)
{
	Q_UNUSED(btAddr)

	QString eventName;

	if (appInstance) {
		eventName = appInstance->btEventName(event);
		appInstance->emitMessageForUi(QString("BT event_name=%1").arg(eventName));
	}

	qDebug() << "BBBB BT event_name=" << eventName << endl;

	if (eventData != NULL) {
		QString eventDataStr = QString(*eventData);
		qDebug() << "BBBB BT event_data=" << eventDataStr;
	} else {
		qDebug() << "BBBB BT event_data=NULL";
	}
}

bt_le_callbacks_t leCallbacks = { btLeAdvertisementCb, NULL, NULL };

ApplicationUI::ApplicationUI()
    : QObject()
	, _model(new GroupDataModel(this))
    , _state(STATE_STOPPED)
{
	appInstance = this;

	_pTranslator = new QTranslator(this);
    _pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(_pLocaleHandler, SIGNAL(systemLanguageChanged()),
                                           this, SLOT(onSystemLanguageChanged()));
    Q_ASSERT(res);
    Q_UNUSED(res);

    onSystemLanguageChanged();

    _qml = QmlDocument::create("asset:///main.qml").parent(this);
    _qml->setContextProperty("app", this);
    _root = _qml->createRootObject<AbstractPane>();
    _mainPage = _root->findChild<QObject*>("mainPage");

    // ============== Message to be sent to page

    QObject::connect(       this, SIGNAL(message(QVariant)),
                       _mainPage, SLOT(onMessage(QVariant)));

    // ============== Hook up buttons

    QObject::connect(  _mainPage, SIGNAL(startListening()),
                            this, SLOT(onStartListening()));
    QObject::connect(  _mainPage, SIGNAL(stopListening()),
                            this, SLOT(onStopListening()));

    Application::instance()->setScene(_root);
}

ApplicationUI::~ApplicationUI() {
	deinitBluetooth();
	appInstance = NULL;
}

void ApplicationUI::initBluetooth()
{
	_model->clear();

	if (bt_device_init(btEvent) == EOK) {
		qDebug() << "BBBB bt_device_init() OK" << endl;
	} else {
		qDebug() << "BBBB bt_device_init() FAIL " << strerror(errno) << endl;
		return;
	}

	if (bt_le_init(&leCallbacks) == EOK) {
		qDebug() << "BBBB bt_le_init() OK" << endl;
	} else {
		qDebug() << "BBBB bt_le_init() FAIL " << strerror(errno) << endl;
		return;
	}

    if (bt_le_set_scan_params(1000, 100, BT_LE_ADVERT_SCAN_PASSIVE ) == EOK) {
        qDebug() << "BBBB bt_le_set_scan_params() OK" << endl;
    } else {
        qDebug() << "BBBB bt_le_set_scan_params() FAIL " << strerror(errno) << endl;
        return;
    }

	if (bt_le_add_scan_device(BT_LE_BDADDR_ANY, NULL) == EOK) {
		qDebug() << "BBBB bt_le_add_scan_device() OK" << endl;
	} else {
		qDebug() << "BBBB bt_le_add_scan_device() FAIL " << strerror(errno) << endl;
	}
}

void ApplicationUI::deinitBluetooth()
{
	qDebug() << "BBBB Calling bt_le_deinit()" << endl;
	bt_le_deinit();
	qDebug() << "BBBB Calling bt_device_deinit()" << endl;
	bt_device_deinit();
}

void ApplicationUI::onStartListening()
{
    qDebug() << "GGGG onStartListening";
    _state = STATE_STARTED;
    emit stateChanged();
	initBluetooth();
}

void ApplicationUI::onStopListening()
{
    qDebug() << "GGGG onStopListening";
    _state = STATE_STOPPED;
    emit stateChanged();
	deinitBluetooth();
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(_pTranslator);
    QString locale_string = QLocale().name();
    QString file_name = QString("TestRandom_%1").arg(locale_string);
    if (_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(_pTranslator);
    }
}

void ApplicationUI::emitMessageForUi(const QVariant &text) {
	emit message(text);
}

QString ApplicationUI::btEventName(int event) {
	switch (event) {
	case BT_EVT_ACCESS_CHANGED:
		return QString("BT_EVT_ACCESS_CHANGED");
	case BT_EVT_RADIO_SHUTDOWN:
		return QString("BT_EVT_RADIO_SHUTDOWN");
	case BT_EVT_RADIO_INIT:
		return QString("BT_EVT_RADIO_INIT");
	case BT_EVT_CONFIRM_NUMERIC_REQUEST:
		return QString("BT_EVT_CONFIRM_NUMERIC_REQUEST");
	case BT_EVT_PAIRING_COMPLETE:
		return QString("BT_EVT_PAIRING_COMPLETE");
	case BT_EVT_DEVICE_ADDED:
		return QString("BT_EVT_DEVICE_ADDED");
	case BT_EVT_DEVICE_DELETED:
		return QString("BT_EVT_DEVICE_DELETED");
	case BT_EVT_SERVICE_CONNECTED:
		return QString("BT_EVT_SERVICE_CONNECTED");
	case BT_EVT_SERVICE_DISCONNECTED:
		return QString("BT_EVT_SERVICE_DISCONNECTED");
	case BT_EVT_FAULT:
		return QString("BT_EVT_FAULT");
	case BT_EVT_UNDEFINED_EVENT:
		return QString("BT_EVT_UNDEFINED_EVENT");
	default:
		return QString("UNKNOWN EVENT:%1").arg(event);
	}
}

void ApplicationUI::parseBeaconData(const char *data, int len, int8_t rssi, const char *bdaddr)
{
	BtleAdvertData advertData;
	if (advertData.parse(QByteArray(data, len))) {
		qDebug() << "BBBB advert data parsed OK" << endl;
		if (advertData.hasBeaconData()) {
			qDebug() << "BBBB beacon data present in advert data"
					 << endl;
			qDebug() << "BBBB beacon UUID    "
					 << advertData.beaconUuid().toHex()
					 << endl;
			qDebug() << "BBBB beacon Major   " << advertData.beaconMajor()
					 << endl;
			qDebug() << "BBBB beacon Minor   " << advertData.beaconMinor()
					 << endl;
			qDebug() << "BBBB beacon Strength"
					 << advertData.calibratedStrength() << "dBm"
					 << endl;
			qDebug() << "BBBB beacon path loss"
					 << advertData.calibratedStrength() - rssi << "dBm"
					 << endl;

            QVariantMap entry;
            entry["MAC"]   = QString(bdaddr);
            entry["UUID"]  = advertData.beaconUuidAsString();
            entry["MAJOR"] = advertData.beaconMajor();
            entry["MINOR"] = advertData.beaconMinor();
            entry["RSSI"]  = advertData.calibratedStrength();
            entry["LOSS"]  = advertData.calibratedStrength()-rssi;

			qDebug() << "BBBB entry[\"MAC\"]" << entry["MAC"] << endl;

            if (!_model->isEmpty()) {
            	bool duplicate = false;
                for (QVariantList indexPath = _model->first(); !indexPath.isEmpty(); indexPath = _model->after(indexPath)) {
                    QVariantMap targetEntry = _model->data(indexPath).toMap();
                    if (targetEntry["MAC"].toString().compare(entry["MAC"].toString()) == 0) {
            			qDebug() << "BBBB duplicate found in model" << endl;
            			duplicate = true;
                    	_model->updateItem(indexPath, entry);
            			break;
                    }
                }
                if (!duplicate) {
        			qDebug() << "BBBB Inserting new item in model" << endl;
                    _model->insert(entry);
                }
            } else {
    			qDebug() << "BBBB Adding first item in model" << endl;
                _model->insert(entry);
            }
		} else {
			qDebug() << "BBBB no beacon present data in advert data" << endl;
		}
	} else {
		qDebug() << "BBBB advert data parse FAIL" << endl;
	}
}

bb::cascades::DataModel* ApplicationUI::model() const
{
    return _model;
}

int ApplicationUI::state() {
    return _state;
}
