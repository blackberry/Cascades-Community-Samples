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

#include <EntitlementCache.hpp>

#define SETTINGS_KEY_CACHE_ENTITLEMENT "SETTINGS-KEY-CACHE-ENTITLEMENT"

#define LOGIT(...)
/*
#define LOGIT(...) \
	do { \
		EntitlementList::iterator __curr; \
		fprintf(stderr, "ENTITLEMENT EntitlementCache " __VA_ARGS__ "\n"); \
		for (__curr = _entitlementList->begin(); __curr != _entitlementList->end(); __curr++) { \
			fprintf(stderr, "\tyear=%d,\tcurrent=%d,\ttotal=%d\n", __curr->getYear(), __curr->getCurrent(), __curr->getTotal()); \
		} \
		fflush(stderr); \
	} while (0)
*/

EntitlementCache* EntitlementCache::_instance;

EntitlementCache::EntitlementCache(QObject *parent)
	: QObject(parent)
	, _entitlementList(new EntitlementList())
	, _settings(Settings::getInstance(this))
{
	restore();
}

EntitlementCache::~EntitlementCache()
{
	save();
	_instance = NULL;
	delete _entitlementList;
}

EntitlementCache *EntitlementCache::getInstance(QObject *parent)
{
    if (_instance == 0) {
        _instance = new EntitlementCache(parent);
    }
    return _instance;
}

QByteArray EntitlementCache::marshall()
{
    LOGIT("marshall()");
    QByteArray serialisedData;
    QDataStream stream(&serialisedData, QIODevice::WriteOnly);
	EntitlementList::iterator curr;
	int schemaVersion = ENTITLEMENT_CACHE_SCHEMA_VERSION_CURRENT;

	stream << schemaVersion;
	stream << _entitlementList->size();

	for (curr = _entitlementList->begin(); curr != _entitlementList->end(); curr++) {
		stream << *curr;
	}

    return serialisedData;
}

void EntitlementCache::unmarshall(QByteArray serialisedData)
{
    LOGIT("unmarshall()");
    QDataStream stream(&serialisedData, QIODevice::ReadOnly);
    int size;
    EntitlementCacheItem item;
	int schemaVersion;

	stream >> schemaVersion;
	stream >> size;

	clearCache();

	for (int i = 0; i < size; i++) {
		stream >> item;
		_entitlementList->append(item);
	}
}

void EntitlementCache::restore()
{
	qDebug() << "EEEE EntitlementCache::restore()" << endl;
    QVariant entitlement = _settings->getQvariantValueFor(SETTINGS_KEY_CACHE_ENTITLEMENT, "UNSET");
    if (entitlement.toString().compare("UNSET") != 0) {
    	unmarshall(entitlement.toByteArray());
    } else {
    	qWarning() << "EEEE EntitlementCache::restore() - no data to restore" << endl;
    }
    LOGIT("restore()");
}

void EntitlementCache::save()
{
	qDebug() << "EEEE EntitlementCache::save()" << endl;
    _settings->saveQvariantValueFor(SETTINGS_KEY_CACHE_ENTITLEMENT, QVariant(marshall()));
    LOGIT("save()");
}

void EntitlementCache::setP2pDefault(int year)
{
	qDebug() << "EEEE EntitlementCache::setP2pDefault - for year:" << year << endl;
	replaceOrAddItem(year, 50, 50);
    LOGIT("setP2pDefault()");
}

void EntitlementCache::addItem(int year, int current, int total)
{
	qDebug() << "EEEE EntitlementCache::addItem - for year, current, total:" << year << current << total << endl;

	EntitlementCacheItem item;

	item.setYear(year);
	item.setCurrent(current);
	item.setTotal(total);

	_entitlementList->append(item);
    LOGIT("addItem()");
}

bool EntitlementCache::isYearPresent(int year)
{
    LOGIT("isYearPresent()");
	EntitlementList::iterator curr;

	for (curr = _entitlementList->begin(); curr != _entitlementList->end(); curr++) {
		if (curr->getYear() == year) {
			qDebug() << "EEEE EntitlementCache::isYearPresent - year present for:" << year << endl;
			return true;
		}
	}
	qDebug() << "EEEE EntitlementCache::isYearPresent - NO year present for:" << year << endl;
	return false;
}

void EntitlementCache::replaceOrAddItem(int year, int current, int total)
{
	qDebug() << "EEEE EntitlementCache::replaceOrAddItem - for year, current, total:" << year << current << total << endl;
	EntitlementList::iterator curr;

	for (curr = _entitlementList->begin(); curr != _entitlementList->end(); curr++) {
		if (curr->getYear() == year) {
			curr->setCurrent(current);
			curr->setTotal(total);
			return;
		}
	}
	addItem(year, current, total);
    LOGIT("replaceOrAddItem()");
}

int EntitlementCache::getCurrent(int year)
{
	EntitlementList::iterator curr;
    LOGIT("getCurrent()");

	for (curr = _entitlementList->begin(); curr != _entitlementList->end(); curr++) {
		if (curr->getYear() == year) {
			return curr->getCurrent();
		}
	}
	return -1;
}

int EntitlementCache::getTotal(int year)
{
	EntitlementList::iterator curr;
    LOGIT("getTotal()");

	for (curr = _entitlementList->begin(); curr != _entitlementList->end(); curr++) {
		if (curr->getYear() == year) {
			return curr->getTotal();
		}
	}
	return -1;
}

void EntitlementCache::increment(int year, int delta)
{
	qDebug() << "EEEE EntitlementCache::increment - for year, delta:" << year << delta << endl;
    LOGIT("increment()");

	EntitlementList::iterator curr;

	for (curr = _entitlementList->begin(); curr != _entitlementList->end(); curr++) {
		if (curr->getYear() == year) {
			curr->setCurrent(curr->getCurrent() + delta);
			return;
		}
	}
}

void EntitlementCache::decrement(int year, int delta)
{
	qDebug() << "EEEE EntitlementCache::decrement - for year, delta:" << year << delta << endl;
    LOGIT("decrement()");
	EntitlementList::iterator curr;

	for (curr = _entitlementList->begin(); curr != _entitlementList->end(); curr++) {
		if (curr->getYear() == year) {
			curr->setCurrent(curr->getCurrent() - delta);
			return;
		}
	}
}

void EntitlementCache::clearCache()
{
	_entitlementList->clear();
    LOGIT("clearCache()");
}

void EntitlementCache::deleteYear(int year)
{
	EntitlementList::iterator curr = _entitlementList->begin();

	while (curr != _entitlementList->end()) {
		if (curr->getYear() == year) {
			curr = _entitlementList->erase(curr);
		} else {
			curr++;
		}
	}
    LOGIT("deleteYear()");
}

QList<int> EntitlementCache::years()
{
    LOGIT("years()");
	EntitlementList::iterator curr;
	QList<int> yearsInList;
	for (curr = _entitlementList->begin(); curr != _entitlementList->end(); curr++) {
		yearsInList.append(curr->getYear());
	}
	return yearsInList;
}
