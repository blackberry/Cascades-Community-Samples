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
#include "BlackBerryProfile.hpp"

#include <QDebug>
#include <bps/bps.h>

BlackBerryProfile::BlackBerryProfile(QObject * owner) :
		IDSWrapper(BLACKBERRY_PROFILE_PROVIDER, owner), m_inAllData(false), m_propertyFetch(
				getVendor) {
}

void BlackBerryProfile::listSuccess(ids_request_id_t request_id, int list_count,
		const char** list, void* object) {
	((BlackBerryProfile*) object)->listSuccess(request_id, list_count, list);
}

void BlackBerryProfile::listSuccess(ids_request_id_t requestId, int listCount,
		const char ** list) {

	qDebug() << "List Success" << "Count" << listCount;
	for (int i = 0; i < listCount; ++i) {
		m_propertyToFetch << list[i];
	}

	getSuccess(0, NULL);
}

void BlackBerryProfile::getSuccess(ids_request_id_t requestId,
		const ids_data_t * data, void * info) {
	((BlackBerryProfile*) info)->getSuccess(requestId, data);
}

void BlackBerryProfile::getSuccess(ids_request_id_t requestId,
		const ids_data_t * data) {
	if (data != NULL) {
		qDebug() << "Get Success";
		QVariantMap toAdd;
		toAdd["scope"] =
				m_propertyFetch == getApplication ? "Application" : "Vendor";
		toAdd["name"] = data->name;
		QByteArray source((char*) data->value, data->length);
		toAdd["value"] = QString(source);
		m_properties << toAdd;
		qDebug() << toAdd;
	}

	if (m_propertyToFetch.size() > 0) {
		QString toUse(m_propertyToFetch[m_propertyToFetch.size() - 1]);
		m_propertyToFetch.removeLast();
		ids_result_t result = ids_get_data(m_idsProvider,
				m_propertyFetch == getApplication ?
						IDS_PROFILE_TYPE_APP : IDS_PROFILE_TYPE_VENDOR,
				IDS_PROFILE_GET_DATA_DEFAULT, toUse.toUtf8().constData(),
				getSuccess, failure, this, &m_requestId);
		failCheck(result, "Could not get data");
	} else {
		if (m_propertyFetch == getApplication) {
			m_propertyFetch = getVendor;
			ids_result_t result = ids_list_data(m_idsProvider,
					IDS_PROFILE_TYPE_VENDOR, IDS_PROFILE_LIST_DATA_DEFAULT,
					listSuccess, failure, this, &m_requestId);
			failCheck(result, "Could not get next list");
		} else {
			m_inAllData = false;
			emit allData(m_properties);
			qDebug() << "End of request";
		}
	}
}

BlackBerryProfile::~BlackBerryProfile() {
	// Nothing to see here!
}

void BlackBerryProfile::requestAllData() {
	// Do you really NEED all data?
	// For your use case - fetching a single property just before
	// it's needed could be the better way to go...
	qDebug() << "Request all data";
	if (!m_inAllData) {
		m_propertyFetch = getApplication;
		m_inAllData = true;

		m_properties.clear();

		ids_result_t result = ids_list_data(m_idsProvider, IDS_PROFILE_TYPE_APP,
				IDS_PROFILE_LIST_DATA_DEFAULT, listSuccess, failure, this,
				&m_requestId);
		failCheck(result, "Could not get initial list");
	} else {
		qDebug() << "Request already in progress";
	}
}

void BlackBerryProfile::addUpdateProperty(bool isUpdate, QString scope,
		QString name, QString value) {
	int mode =
			scope.compare("application", Qt::CaseInsensitive) == 0 ?
					IDS_PROFILE_TYPE_APP : IDS_PROFILE_TYPE_VENDOR;

	// Always keep these when they are longer living than one line.
	QByteArray nameData(name.toUtf8());
	QByteArray valueData(value.toUtf8());

	ids_data_t data;
	data.name = nameData.constData();
	data.value = valueData.data();
	data.length = valueData.length();

	ids_result_t result;
	if (isUpdate) {
		result = ids_set_data(m_idsProvider, mode, IDS_PROFILE_SET_DATA_DEFAULT,
				&data, success, failure, this, &m_requestId);
	} else {
		result = ids_create_data(m_idsProvider, mode,
				IDS_PROFILE_CREATE_DATA_DEFAULT, &data, success, failure, this,
				&m_requestId);
	}
	failCheck(result, "Could not save data");
}

void BlackBerryProfile::success(ids_request_id_t request, void * data) {
	((BlackBerryProfile*) data)->success(request);
}

void BlackBerryProfile::success(ids_request_id_t request) {
	qDebug() << "Yay!"; // You may want to handle this further!
}

void BlackBerryProfile::deleteProperty(QString scope, QString name) {
	int mode =
			scope.compare("application", Qt::CaseInsensitive) == 0 ?
					IDS_PROFILE_TYPE_APP : IDS_PROFILE_TYPE_VENDOR;

	ids_result_t result = ids_delete_data(m_idsProvider, mode,
			IDS_PROFILE_DELETE_DATA_DEFAULT, name.toUtf8().constData(), success,
			failure, this, &m_requestId);
	failCheck(result, "Could not delete data");
}

