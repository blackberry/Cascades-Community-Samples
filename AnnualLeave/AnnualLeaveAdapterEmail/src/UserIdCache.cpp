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

#include <UserIdCache.hpp>

#define SETTINGS_KEY_CACHE_USERID "SETTINGS-KEY-CACHE-USERID"

#define LOGIT(...)
/*
#define LOGIT(...) \
	do { \
		UserIdList::iterator __curr; \
		fprintf(stderr, "USERID UserIdCache " __VA_ARGS__ "\n"); \
		for (__curr = _userIdList->begin(); __curr != _userIdList->end(); __curr++) { \
			fprintf(stderr, "\tuserId=%s,\temail=%s,\ttaskId=%d\n", \
					__curr->getUserId().toLatin1().constData(), \
					__curr->getEmailAddress().toLatin1().constData(), \
					__curr->getTaskId()); \
		} \
		fflush(stderr); \
	} while (0)
*/
UserIdCache *UserIdCache::_instance;

UserIdCache::UserIdCache(QObject *parent)
	: QObject(parent)
	, _userIdList(new UserIdList())
	, _settings(Settings::getInstance(this))
{
	restore();
}

UserIdCache::~UserIdCache()
{
	save();
	_instance = NULL;
	delete _userIdList;
}

UserIdCache *UserIdCache::getInstance(QObject *parent)
{
    if (_instance == 0) {
        _instance = new UserIdCache(parent);
    }
    return _instance;
}

QByteArray UserIdCache::marshall()
{
    LOGIT("marshall()");
    QByteArray serialisedData;
    QDataStream stream(&serialisedData, QIODevice::WriteOnly);
    UserIdList::iterator curr;
	int schemaVersion = USERID_CACHE_SCHEMA_VERSION_CURRENT;

	stream << schemaVersion;
	stream << _userIdList->size();

	for (curr = _userIdList->begin(); curr != _userIdList->end(); curr++) {
		stream << *curr;
	}

    return serialisedData;
}

void UserIdCache::unmarshall(QByteArray serialisedData)
{
    LOGIT("unmarshall()");
    QDataStream stream(&serialisedData, QIODevice::ReadOnly);
    int size;
    UserIdCacheItem item;
	int schemaVersion;

	stream >> schemaVersion;
	stream >> size;

	clear();

	for (int i = 0; i < size; i++) {
		stream >> item;
		_userIdList->append(item);
	}
}

void UserIdCache::restore()
{
	qDebug() << "EEEE UserIdCache::restore()" << endl;
    QVariant userIdCacheData = _settings->getQvariantValueFor(SETTINGS_KEY_CACHE_USERID, "UNSET");
    if (userIdCacheData.toString().compare("UNSET") != 0) {
    	unmarshall(userIdCacheData.toByteArray());
    } else {
    	qWarning() << "EEEE UserIdCache::restore() - no data to restore" << endl;
    }
    LOGIT("restore()");
}

void UserIdCache::save()
{
	qDebug() << "EEEE UserIdCache::save()" << endl;
    _settings->saveQvariantValueFor(SETTINGS_KEY_CACHE_USERID, QVariant(marshall()));
    LOGIT("save()");
}

void UserIdCache::clear()
{
	_userIdList->clear();
    LOGIT("clear()");
}

bool UserIdCache::foundUserIdMatch(const QString &userId)
{
    LOGIT("foundUserIdMatch()");

    UserIdList::iterator curr;

	for (curr = _userIdList->begin(); curr != _userIdList->end(); curr++) {
		if ((curr->getUserId()).compare(userId) == 0) {
			qDebug() << "EEEE UserIdCache::foundUserIdMatch - found userId=" << curr->getUserId() << endl;
			return true;
		}
	}
	qWarning() << "EEEE UserIdCache::foundUserIdMatch - NO userId found for userId=" << curr->getUserId() << endl;
	return false;
}

bool UserIdCache::foundTaskIdMatch(int taskId)
{
    LOGIT("foundTaskIdMatch()");

    UserIdList::iterator curr;

	for (curr = _userIdList->begin(); curr != _userIdList->end(); curr++) {
		if (curr->getTaskId() == taskId) {
			qDebug() << "EEEE UserIdCache::foundTaskIdMatch - found taskId=" << curr->getTaskId() << endl;
			return true;
		}
	}
	qWarning() << "EEEE UserIdCache::foundTaskIdMatch - NO taskId found for taskId=" << curr->getTaskId() << endl;
	return false;
}

QString UserIdCache::userId(int taskId)
{
    LOGIT("userId()");

    UserIdList::iterator curr;

	for (curr = _userIdList->begin(); curr != _userIdList->end(); curr++) {
		if (curr->getTaskId() == taskId) {
			qDebug() << "EEEE UserIdCache::userId - found userId=" << curr->getUserId() <<  "for taskId=" << taskId << endl;
			return curr->getUserId();
		}
	}
	qWarning() << "EEEE UserIdCache::userId - NO userId found for taskId=" << taskId << endl;
	return "";
}

QString UserIdCache::emailAddress(int taskId)
{
    LOGIT("emailAddress()");

    UserIdList::iterator curr;

	for (curr = _userIdList->begin(); curr != _userIdList->end(); curr++) {
		if (curr->getTaskId() == taskId) {
			qDebug() << "EEEE UserIdCache::emailAddress - found email=" << curr->getEmailAddress() <<  "for taskId=" << taskId << endl;
			return curr->getEmailAddress();
		}
	}
	qWarning() << "EEEE UserIdCache::emailAddress - NO email found for taskId=" << taskId << endl;
	return "";
}

void UserIdCache::add(const QString &userId, const QString &senderEmail)
{
	qDebug() << "EEEE UserIdCache::add - for userId, senderEmail:" << userId << senderEmail << endl;

	UserIdCacheItem item;

	item.setUserId(userId);
	item.setEmailAddress(senderEmail);
	item.setTaskId(-1);

	_userIdList->append(item);
    LOGIT("add()");
}

void UserIdCache::updateItem(const QString &userId, int taskId)
{
    LOGIT("updateItem() A");

    UserIdList::iterator curr;

	for (curr = _userIdList->begin(); curr != _userIdList->end(); curr++) {
		if (((curr->getUserId()).compare(userId) == 0) && (curr->getTaskId() == -1)) {
			qDebug() << "EEEE UserIdCache::updateItem - unassigned entry found for userId=" << userId << endl;
			curr->setTaskId(taskId);
		    LOGIT("updateItem() B");
			return;
		}
	}
	qWarning() << "EEEE UserIdCache::updateItem - NO unassigned entries found for userId=" << userId << endl;
}

void UserIdCache::removeItem(int taskId)
{
	UserIdList::iterator curr = _userIdList->begin();

	while (curr != _userIdList->end()) {
		if (curr->getTaskId() == taskId) {
			curr = _userIdList->erase(curr);
		} else {
			curr++;
		}
	}
	LOGIT("removeItem()");
}
