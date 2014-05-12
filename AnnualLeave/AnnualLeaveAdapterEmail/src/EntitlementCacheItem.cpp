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

#include <EntitlementCacheItem.hpp>

EntitlementCacheItem::EntitlementCacheItem()
	: _year(-1)
	, _current(-1)
	, _total(-1)
{}

EntitlementCacheItem::~EntitlementCacheItem() {}

QDataStream &operator<<(QDataStream &out, const EntitlementCacheItem &entitlementCacheItem)
{
	int schemaVersion = ENTITLEMENT_CACHE_ITEM_SCHEMA_VERSION_CURRENT;

	out << schemaVersion
		<< entitlementCacheItem.getYear()
		<< entitlementCacheItem.getCurrent()
		<< entitlementCacheItem.getTotal();
	return out;
}

QDataStream &operator>>(QDataStream &in, EntitlementCacheItem &entitlementCacheItem)
{
	int year;
	int current;
	int total;
	int schemaVersion;

	in >> schemaVersion;

	switch (schemaVersion) {
		case ENTITLEMENT_CACHE_ITEM_SCHEMA_VERSION_CURRENT:
			in >> year >> current >> total;
			entitlementCacheItem.setYear(year);
			entitlementCacheItem.setCurrent(current);
			entitlementCacheItem.setTotal(total);
			break;
//
// Handle older schema versions just in case application has been updated
//
//		case ENTITLEMENT_CACHE_ITEM_SCHEMA_VERSION_CURRENT_x:
//			in >> year >> current >> total >> xxx;
//			entitlementCacheItem.setYear(year);
//			entitlementCacheItem.setCurrent(current);
//			entitlementCacheItem.setTotal(total);
//			entitlementCacheItem.setXXX(xxx);
//			break;

		default:
			qWarning() << "EEEE EntitlementCacheItem  unrecognised schema version detected" << schemaVersion << endl;
			break;
	}


	return in;
}

int EntitlementCacheItem::getCurrent() const {
	return _current;
}

void EntitlementCacheItem::setCurrent(int current) {
	_current = current;
}

int EntitlementCacheItem::getTotal() const {
	return _total;
}

void EntitlementCacheItem::setTotal(int total) {
	_total = total;
}

int EntitlementCacheItem::getYear() const {
	return _year;
}

void EntitlementCacheItem::setYear(int year) {
	_year = year;
}
