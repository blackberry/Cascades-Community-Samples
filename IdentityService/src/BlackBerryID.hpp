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
#ifndef BLACKBERRYID_HPP_
#define BLACKBERRYID_HPP_

#include <QObject>
#include <QList>
#include <QVariantMap>

#include "IDSWrapper.hpp"

class BlackBerryID: public IDSWrapper {
Q_OBJECT
public:
	BlackBerryID(QObject * owner = NULL);
	virtual ~BlackBerryID();

	// Request the list of properties.
	void requestBlackBerryIDProperties();

signals:
	// Emitted when the list is received.
	void propertiesReceived(QList<QVariantMap>);

private:
	// Static and instance version for the callback.
	static void handleProperties(ids_request_id_t request_id,
			int property_count, const ids_property_t* properties,
			void* cb_data);
	void handleProperties(ids_request_id_t request_id, int property_count,
			const ids_property_t* properties);

};

#endif /* BLACKBERRYID_HPP_ */
