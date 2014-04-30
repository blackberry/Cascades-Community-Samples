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

#include <RollBackItem.hpp>

RollBackItem::RollBackItem()
	: _localId(-1)
	, _originating_op_id(-1)
	, _halfDays(0)
	, _year(0)
	, _newItem(false)
{}

RollBackItem::~RollBackItem() {}

QDataStream &operator<<(QDataStream &out, const RollBackItem &rollBackItem)
{
	int schemaVersion = ROLLBACK_ITEM_SCHEMA_VERSION_CURRENT;

	out << schemaVersion
		<< rollBackItem.getLocalId()
		<< rollBackItem.getOriginatingOpId()
		<< rollBackItem.getHalfDays()
		<< rollBackItem.getYear()
		<< rollBackItem.isNewItem();
	return out;
}

QDataStream &operator>>(QDataStream &in, RollBackItem &rollBackItem)
{
	int localId;
	int originating_op_id;
	int halfDays;
	int year;
	bool isNewItem;
	int schemaVersion;

	in >> schemaVersion;

	switch (schemaVersion) {
		case ROLLBACK_ITEM_SCHEMA_VERSION_CURRENT:
			in >> localId >> originating_op_id >> halfDays >> year >> isNewItem;
			rollBackItem.setLocalId(localId);
			rollBackItem.setOriginatingOpId(originating_op_id);
			rollBackItem.setHalfDays(halfDays);
			rollBackItem.setYear(year);
			rollBackItem.setNewItem(isNewItem);
			break;
//
// Handle older schema versions just in case application has been updated
//
//		case ROLLBACK_ITEM_SCHEMA_VERSION_X:
//			in >> localId >> originating_op_id >> halfDays >> year >> isNewItem >> xxx;
//			rollBackItem.setLocalId(localId);
//			rollBackItem.setOriginatingOpId(originating_op_id);
//			rollBackItem.setHalfDays(halfDays);
//			rollBackItem.setYear(year);
//			rollBackItem.setNewItem(isNewItem);
//			rollBackItem.setXXX(xxx);
//			break;

		default:
			qWarning() << "EEEE RollBackItem  unrecognised schema version detected" << schemaVersion << endl;
			break;
	}

	return in;
}

int RollBackItem::getHalfDays() const {
	return _halfDays;
}

void RollBackItem::setHalfDays(int halfDays) {
	_halfDays = halfDays;
}

int RollBackItem::getLocalId() const {
	return _localId;
}

void RollBackItem::setLocalId(int localId) {
	_localId = localId;
}

int RollBackItem::getOriginatingOpId() const {
	return _originating_op_id;
}

void RollBackItem::setOriginatingOpId(int originating_op_id) {
	_originating_op_id = originating_op_id;
}

int RollBackItem::getYear() const {
	return _year;
}

void RollBackItem::setYear(int year) {
	_year = year;
}

bool RollBackItem::isNewItem() const {
	return _newItem;
}

void RollBackItem::setNewItem(bool isNewItem) {
	_newItem = isNewItem;
}
