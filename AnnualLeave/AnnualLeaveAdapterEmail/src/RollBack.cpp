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

#include <RollBack.hpp>

#define SETTINGS_KEY_CACHE_ROLLBACK "SETTINGS-KEY-CACHE-ROLLBACK"

#define LOGIT(...)
/*
#define LOGIT(...) \
	do { \
		RollBackList::iterator __curr; \
		fprintf(stderr, "ENTITLEMENT RollBack " __VA_ARGS__ "\n"); \
		for (__curr = _rollBackList->begin(); __curr != _rollBackList->end(); __curr++) { \
			fprintf(stderr, "\tnewItem=%s,\tlocalId=%d,\toriginalOpId=%d,\tyear=%d,\thalfDays=%d\n", \
				   (__curr->isNewItem() ? "true" : "false"), __curr->getLocalId(), __curr->getOriginatingOpId(), __curr->getYear(), __curr->getHalfDays()); \
		} \
		fflush(stderr); \
	} while (0)
*/

RollBack* RollBack::_instance;

RollBack::RollBack(QObject *parent)
	: QObject(parent)
	, _rollBackList(new RollBackList())
	, _settings(Settings::getInstance(this))
{
	restore();
}

RollBack::~RollBack()
{
	save();
	_instance = NULL;
	delete _rollBackList;
}

RollBack *RollBack::getInstance(QObject *parent)
{
    if (_instance == 0) {
        _instance = new RollBack(parent);
    }
    return _instance;
}

QByteArray RollBack::marshall()
{
    LOGIT("marshall()");
    QByteArray serialisedData;
    QDataStream stream(&serialisedData, QIODevice::WriteOnly);
    RollBackList::iterator curr;
	int schemaVersion = ROLLBACK_SCHEMA_VERSION_CURRENT;

	stream << schemaVersion;
	stream << _rollBackList->size();

	for (curr = _rollBackList->begin(); curr != _rollBackList->end(); curr++) {
		stream << *curr;
	}

    return serialisedData;
}

void RollBack::unmarshall(QByteArray serialisedData)
{
    QDataStream stream(&serialisedData, QIODevice::ReadOnly);
    int size;
    RollBackItem item;
	int schemaVersion;

	stream >> schemaVersion;
	stream >> size;

	clear();

	for (int i = 0; i < size; i++) {
		stream >> item;
		_rollBackList->append(item);
	}
    LOGIT("unmarshall()");
}

void RollBack::restore()
{
	qDebug() << "EEEE RollBack::restore()" << endl;
    QVariant rollBack = _settings->getQvariantValueFor(SETTINGS_KEY_CACHE_ROLLBACK, "UNSET");
    if (rollBack.toString().compare("UNSET") != 0) {
    	unmarshall(rollBack.toByteArray());
    } else {
    	qWarning() << "EEEE RollBack::restore() - no data to restore" << endl;
    }
    LOGIT("restore()");
}

void RollBack::save()
{
	qDebug() << "EEEE RollBack::save()" << endl;
    _settings->saveQvariantValueFor(SETTINGS_KEY_CACHE_ROLLBACK, QVariant(marshall()));
    LOGIT("save()");
}

void RollBack::clear()
{
	_rollBackList->clear();
    LOGIT("clear()");
}

void RollBack::addItem(int localId, int originating_op_id, int year, int halfDays)
{
	addOldItem(localId, originating_op_id, year, halfDays);
    LOGIT("addItem()");
}

void RollBack::addOldItem(int localId, int originating_op_id, int year, int halfDays)
{
	qDebug() << "EEEE RollBack::addOldItem() - localId, originating_op_id, year, halfDays" << localId << originating_op_id << year << halfDays << endl;

	RollBackItem item;

	item.setLocalId(localId);
	item.setOriginatingOpId(originating_op_id);
	item.setYear(year);
	item.setHalfDays(halfDays);
	item.setNewItem(false);

	_rollBackList->append(item);
    LOGIT("addOldItem()");
}

void RollBack::addNewItem(int localId, int originating_op_id, int year, int halfDays)
{
	qDebug() << "EEEE RollBack::addNewItem() - localId, originating_op_id, year, halfDays" << localId << originating_op_id << year << halfDays << endl;

	RollBackItem item;

	item.setLocalId(localId);
	item.setOriginatingOpId(originating_op_id);
	item.setYear(year);
	item.setHalfDays(halfDays);
	item.setNewItem(true);

	_rollBackList->append(item);
    LOGIT("addNewItem()");
}

void RollBack::deleteItem(int localId, int originating_op_id)
{
	qDebug() << "EEEE RollBack::deleteItem() - localId, originating_op_id" << localId << originating_op_id << endl;

	RollBackList::iterator curr = _rollBackList->begin();

	while (curr != _rollBackList->end()) {
		if ((curr->getLocalId() == localId) && (curr->getOriginatingOpId() == originating_op_id)) {
			curr = _rollBackList->erase(curr);
		} else {
			curr++;
		}
	}
	LOGIT("deleteItem()");
}

void RollBack::deleteItem(int localId)
{
	qDebug() << "EEEE RollBack::deleteItem() - localId" << localId << endl;

	RollBackList::iterator curr = _rollBackList->begin();

	while (curr != _rollBackList->end()) {
		if (curr->getLocalId() == localId) {
			curr = _rollBackList->erase(curr);
		} else {
			curr++;
		}
	}
	LOGIT("deleteItem()");
}

bool RollBack::isItemPresent(int localId)
{
	qDebug() << "EEEE RollBack::isItemPresent() - localId" << localId << endl;
    LOGIT("isItemPresent()");

	RollBackList::iterator curr;

	for (curr = _rollBackList->begin(); curr != _rollBackList->end(); curr++) {
		if (curr->getLocalId() == localId) {
			qDebug() << "EEEE RollBack::isItemPresent()" << true << endl;
			return true;
		}
	}
	qDebug() << "EEEE RollBack::isItemPresent()" << false << endl;
	return false;
}

bool RollBack::isItemPresent(int localId, int originating_op_id)
{
	qDebug() << "EEEE RollBack::isItemPresent() - localId, originating_op_id" << localId << originating_op_id << endl;
    LOGIT("isItemPresent()");

	RollBackList::iterator curr;

	for (curr = _rollBackList->begin(); curr != _rollBackList->end(); curr++) {
		if ((curr->getLocalId() == localId) && (curr->getOriginatingOpId() == originating_op_id)) {
			qDebug() << "EEEE RollBack::isItemPresent()" << true << endl;
			return true;
		}
	}
	qDebug() << "EEEE RollBack::isItemPresent()" << false << endl;
	return false;
}

int RollBack::getHalfDays(int localId, int originating_op_id)
{
	return getOldHalfDays(localId, originating_op_id);
}

int RollBack::getYear(int localId, int originating_op_id)
{
	return getOldYear(localId, originating_op_id);
}

int RollBack::getOldHalfDays(int localId, int originating_op_id)
{
	qDebug() << "EEEE RollBack::getOldHalfDays() - localId, originating_op_id" << localId << originating_op_id << endl;
    LOGIT("getOldHalfDays()");

	RollBackList::iterator curr;

	for (curr = _rollBackList->begin(); curr != _rollBackList->end(); curr++) {
		if (!(curr->isNewItem()) && (curr->getLocalId() == localId) && (curr->getOriginatingOpId() == originating_op_id)) {
			qDebug() << "EEEE RollBack::getHalfDays()" << curr->getHalfDays() << endl;
			return curr->getHalfDays();
		}
	}
	qDebug() << "EEEE RollBack::getHalfDays()" << 0 << endl;
	return 0;
}

int RollBack::getOldYear(int localId, int originating_op_id)
{
	qDebug() << "EEEE RollBack::getOldYear() - localId, originating_op_id" << localId << originating_op_id << endl;
    LOGIT("getOldYear()");

	RollBackList::iterator curr;

	for (curr = _rollBackList->begin(); curr != _rollBackList->end(); curr++) {
		if (!(curr->isNewItem()) && (curr->getLocalId() == localId) && (curr->getOriginatingOpId() == originating_op_id)) {
			qDebug() << "EEEE RollBack::getYear()" << curr->getYear() << endl;
			return curr->getYear();
		}
	}
	qDebug() << "EEEE RollBack::getYear()" << 0 << endl;
	return 0;
}

int RollBack::getNewHalfDays(int localId, int originating_op_id)
{
	qDebug() << "EEEE RollBack::getNewHalfDays() - localId, originating_op_id" << localId << originating_op_id << endl;
    LOGIT("getNewHalfDays()");

	RollBackList::iterator curr;

	for (curr = _rollBackList->begin(); curr != _rollBackList->end(); curr++) {
		if ((curr->isNewItem()) && (curr->getLocalId() == localId) && (curr->getOriginatingOpId() == originating_op_id)) {
			qDebug() << "EEEE RollBack::getHalfDays()" << curr->getHalfDays() << endl;
			return curr->getHalfDays();
		}
	}
	qDebug() << "EEEE RollBack::getHalfDays()" << 0 << endl;
	return 0;
}

int RollBack::getNewYear(int localId, int originating_op_id)
{
	qDebug() << "EEEE RollBack::getNewYear() - localId, originating_op_id" << localId << originating_op_id << endl;
    LOGIT("getNewYear()");

	RollBackList::iterator curr;

	for (curr = _rollBackList->begin(); curr != _rollBackList->end(); curr++) {
		if ((curr->isNewItem()) && (curr->getLocalId() == localId) && (curr->getOriginatingOpId() == originating_op_id)) {
			qDebug() << "EEEE RollBack::getYear()" << curr->getYear() << endl;
			return curr->getYear();
		}
	}
	qDebug() << "EEEE RollBack::getYear()" << 0 << endl;
	return 0;
}
