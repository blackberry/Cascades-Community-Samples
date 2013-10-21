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
#include "BlackBerryID.hpp"
#include <ids_blackberry_id.h>

#include <errno.h>

BlackBerryID::BlackBerryID(QObject * owner) :
		IDSWrapper(BLACKBERRY_ID_PROVIDER, owner) {
	// nothing to see here.
}

BlackBerryID::~BlackBerryID() {
	// nothing to see here.
}

void BlackBerryID::requestBlackBerryIDProperties() {
	// Which properties are you interested in? (ALL OF THEM!)
	const char* properties[] = { IDS_BBID_PROP_USERNAME,
			IDS_BBID_PROP_FIRSTNAME, IDS_BBID_PROP_SCREENNAME,
			IDS_BBID_PROP_LASTNAME, IDS_BBID_PROP_UID };

	ids_result_t result = ids_get_properties(m_idsProvider, BBID_PROPERTY_CORE,
			5, properties, handleProperties, failure, this, &m_requestId);

	failCheck(result,"Could not get properties");
}

void BlackBerryID::handleProperties(ids_request_id_t request_id,
		int property_count, const ids_property_t* properties, void* cb_data) {
	BlackBerryID * app = (BlackBerryID*) cb_data;
	app->handleProperties(request_id, property_count, properties);
}

void BlackBerryID::handleProperties(ids_request_id_t,
		int property_count, const ids_property_t* properties) {
	// Extract the properties - push them into a list for easy Qt consumption.
	QList<QVariantMap> propList;
	for (int i = 0; i < property_count; ++i) {
		QVariantMap toAdd;
		toAdd["name"] = properties[i].name;
		toAdd["value"] = properties[i].value;
		propList << toAdd;
	}
	emit propertiesReceived(propList);
}

