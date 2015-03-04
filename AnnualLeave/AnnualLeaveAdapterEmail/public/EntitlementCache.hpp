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

#ifndef ENTITLEMENTCACHE_HPP_
#define ENTITLEMENTCACHE_HPP_

#define ENTITLEMENT_CACHE_SCHEMA_VERSION_1 1
#define ENTITLEMENT_CACHE_SCHEMA_VERSION_CURRENT ENTITLEMENT_CACHE_SCHEMA_VERSION_1

#include <EntitlementCacheItem.hpp>
#include <Settings.hpp>
#include <common.hpp>

#include <QObject>
#include <QList>
#include <QDataStream>

typedef QList<EntitlementCacheItem> EntitlementList;

class Q_DECL_EXPORT EntitlementCache
	: public QObject
{
	Q_OBJECT

public:
	virtual ~EntitlementCache();
	static EntitlementCache *getInstance(QObject *parent);
	void addItem(int year, int current, int total);
	bool isYearPresent(int year);
	void replaceOrAddItem(int year, int current, int total);
	void clearCache();
	int getCurrent(int year);
	int getTotal(int year);
	void deleteYear(int year);
	void save();
	void restore();
	void setP2pDefault(int year);
	void increment(int year, int delta);
	void decrement(int year, int delta);
	QList<int> years();

private:
	EntitlementCache(QObject *parent=NULL);
	static EntitlementCache* _instance;
	EntitlementList *_entitlementList;
    Settings *_settings;

    QByteArray marshall();
    void unmarshall(QByteArray serializedData);
};

#endif /* ENTITLEMENTCACHE_HPP_ */
