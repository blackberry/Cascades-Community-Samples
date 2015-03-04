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

#ifndef USERIDCACHE_HPP_
#define USERIDCACHE_HPP_

#define USERID_CACHE_SCHEMA_VERSION_1 1
#define USERID_CACHE_SCHEMA_VERSION_CURRENT USERID_CACHE_SCHEMA_VERSION_1

#include <UserIdCacheItem.hpp>
#include <Settings.hpp>
#include <common.hpp>

#include <QObject>
#include <QList>
#include <QDataStream>

typedef QList<UserIdCacheItem> UserIdList;

class Q_DECL_EXPORT UserIdCache
	: public QObject
{
	Q_OBJECT

public:
	virtual ~UserIdCache();
	static UserIdCache *getInstance(QObject *parent);

	void clear();
	void save();
	void restore();

	bool foundUserIdMatch(const QString &userId);
	bool foundTaskIdMatch(int taskId);
	QString userId(int taskId);
	QString emailAddress(int taskId);
	void add(const QString &userId, const QString &senderEmail);
	void updateItem(const QString &userId, int taskId);
	void removeItem(int taskId);

private:
	UserIdCache(QObject *parent=NULL);
	static UserIdCache *_instance;
	UserIdList *_userIdList;
    Settings *_settings;

    QByteArray marshall();
    void unmarshall(QByteArray serializedData);
};

#endif /* USERIDCACHE_HPP_ */
