/**
   Copyright (c) 2013 BlackBerry Limited.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**/
#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/DataModel>
#include <bb/cascades/GroupDataModel>
#include <bb/cascades/Sheet>

#include <bb/system/SystemToast>

#include <bps/bps.h>

#include <QVariantMap>

#include <ids_blackberry_id.h>

using namespace bb::cascades;
using namespace bb::system;

QString getAllMessages();

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
		QObject(app) {
	m_blackBerryID = new BlackBerryID(this);
	m_blackBerryProfile = new BlackBerryProfile(this);

	connect(m_blackBerryProfile, SIGNAL(allData(QList<QVariantMap>)), this,
			SLOT(onAllData(QList<QVariantMap>)));
	connect(m_blackBerryProfile, SIGNAL(error(QString,int)), this,
			SLOT(onError(QString,int)));
	connect(m_blackBerryID, SIGNAL(propertiesReceived(QList<QVariantMap>)),
			this, SLOT(onIDProperties(QList<QVariantMap>)));
	connect(m_blackBerryID, SIGNAL(error(QString,int)), this,
			SLOT(onError(QString,int)));

	m_propertyData = new GroupDataModel(QList<QString>() << "scope" << "name",
			this);
	m_propertyData->setGrouping(ItemGrouping::ByFullValue);

	m_propertyIsEdit = false;

	// Create scene document from main.qml asset, the parent is set
	// to ensure the document gets destroyed properly at shut down.
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	qml->setContextProperty("app", this);

	// Create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();

	qml = QmlDocument::create("asset:///PropertyEdit.qml").parent(this);
	qml->setContextProperty("app", this);
	m_propertyEdit = qml->createRootObject<Sheet>();

	// Set created root object as the application scene
	app->setScene(root);

	getBlackBerryID();
	refreshProperties();
}

void ids_call_failed(ids_request_id_t request_id, ids_result_t result,
		const char* info, void* cb_data) {
	fprintf(stderr, "Req %u [%s]: Failure: %d [%s]\n", request_id, (char*)cb_data, result, info?info:"NULL" );
	char buf[128];
	sprintf( buf, "Code: %d [info:%s]", result, info?info:"NULL" );
	qDebug() << cb_data << buf;
}

static int ids_io_handler(int fd, int flags, void *) {
	if (ids_process_msg(fd) != IDS_SUCCESS) {
		return BPS_FAILURE;
	}
	return BPS_SUCCESS;
}

void ApplicationUI::getBlackBerryID() {
	m_blackBerryID->requestBlackBerryIDProperties();
}

void ApplicationUI::addProperty() {
	setPropertyIsEdit(false);
	setPropertyScope("application");
	setPropertyName("");
	setPropertyValue("");
	m_propertyEdit->open();
}

void ApplicationUI::editProperty(QString scope, QString name) {
	qDebug() << "edit Property" << scope << name;
	for (int i = 0; i < m_properties.size(); ++i) {
		QVariantMap data(m_properties[i]);
		qDebug() << data;
		if (data["scope"].toString().compare(scope, Qt::CaseInsensitive) == 0
				&& data["name"].toString() == name) {
			setPropertyIsEdit(true);
			setPropertyScope(data["scope"].toString().toLower());
			setPropertyName(data["name"].toString());
			setPropertyValue(data["value"].toString());
			qDebug() << "About to open";
			m_propertyEdit->open();
			return;
		}
	}
	qDebug() << "Didn't edit anything!!!";
}

void ApplicationUI::deleteProperty(QString scope, QString name) {
	m_blackBerryProfile->deleteProperty(scope, name);
	refreshProperties();
}

bool ApplicationUI::saveProperty(QString scope, QString name, QString value) {
	QString failure;
	qDebug() << scope << name << value;
	if (scope == "") {
		failure = "Scope is not set. ";
	}
	if (name == "") {
		failure += "Name is not set. ";
	}
	if (value == "") {
		failure += "Value is not set. ";
	}
	if (failure != "") {
		toast(failure);
		return false;
	}
	m_blackBerryProfile->addUpdateProperty(m_propertyIsEdit, scope, name,
			value);

	refreshProperties();
	return true;
}

void ApplicationUI::toast(QString message) {
	SystemToast * t = new SystemToast(this);
	t->setBody(message);
	t->show();
}

void ApplicationUI::onAllData(QList<QVariantMap> all) {
	qDebug() << "On All Data";

	m_properties = all;
	m_propertyData->deleteLater();

	m_propertyData = new GroupDataModel(QList<QString>() << "scope" << "name",
			this);
	m_propertyData->setGrouping(ItemGrouping::ByFullValue);
	m_propertyData->insertList(all);

	emit propertyDataChanged(m_propertyData);
	qDebug() << "Property Count Changed";
	emit propertyCountChanged(m_propertyData->size());
}

void ApplicationUI::refreshProperties() {
	emit propertyCountChanged(-1);
	m_blackBerryProfile->requestAllData();
}

void ApplicationUI::onIDProperties(QList<QVariantMap> properties) {
	for (int i = 0; i < properties.length(); ++i) {
		QVariantMap current(properties[i]);

		QString name(current["name"].toString());
		QString value(current["value"].toString());

		if (name == IDS_BBID_PROP_USERNAME) {
			setUserName(value);
		} else if (name == IDS_BBID_PROP_FIRSTNAME) {
			setFirstName(value);
		} else if (name == IDS_BBID_PROP_LASTNAME) {
			setLastName(value);
		} else if (name == IDS_BBID_PROP_SCREENNAME) {
			setScreenName(value);
		}
	}
}

void ApplicationUI::onError(QString message, int error) {
	if (error < 0) {
		toast(message);
	} else {
		toast(QString("%2\nError: %1").arg(error).arg(message));
	}
}


