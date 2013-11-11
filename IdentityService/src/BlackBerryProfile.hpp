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
#ifndef BLACKBERRYPROFILE_HPP_
#define BLACKBERRYPROFILE_HPP_

#include <QObject>

#include <ids.h>
#include <ids_blackberry_profile.h>

#include <QList>
#include <QVariantMap>

#include "IDSWrapper.hpp"

class BlackBerryProfile: public IDSWrapper {
Q_OBJECT
public:
	BlackBerryProfile(QObject * owner);
	virtual ~BlackBerryProfile();

	// Request all of the profile data. In your use case -
	// you may not want to do this... Why not fetch single
	// properties when needed?
	void requestAllData();

	// Add or update a property.
	void addUpdateProperty(bool isUpdate, QString scope, QString name,
			QString value);

	// Delete a property.
	void deleteProperty(QString scope, QString name);

signals:
	// Emitted when all data has been retreived from the server.
	void allData(QList<QVariantMap> entries);

private:
	enum AllDataState {
		getApplication, getVendor
	};
	AllDataState m_propertyFetch;

	QList<QString> m_propertyToFetch;
	QList<QVariantMap> m_properties;

	// Static -> Instance version
	static void success(ids_request_id_t request_id, void* cb_data);
	void success(ids_request_id_t);

	// Static -> Instance version
	static void listSuccess(ids_request_id_t, int, const char**, void*);
	void listSuccess(ids_request_id_t, int, const char**);

	// Static -> Instance version
	static void getSuccess(ids_request_id_t, const ids_data_t*, void*);
	void getSuccess(ids_request_id_t, const ids_data_t*);

	bool m_inAllData;

};

#endif /* BLACKBERRYPROFILE_HPP_ */
