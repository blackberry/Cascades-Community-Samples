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

#ifndef ROLLBACKITEM_HPP_
#define ROLLBACKITEM_HPP_

#define ROLLBACK_ITEM_SCHEMA_VERSION_1 1
#define ROLLBACK_ITEM_SCHEMA_VERSION_CURRENT ROLLBACK_ITEM_SCHEMA_VERSION_1

class RollBackItem
{
public:
	RollBackItem();
	virtual ~RollBackItem();

	friend QDataStream &operator<<(QDataStream &out, const RollBackItem &rollBackItem);
	friend QDataStream &operator>>(QDataStream &in, RollBackItem &rollBackItem);
	int getHalfDays() const;
	void setHalfDays(int halfDays);
	int getLocalId() const;
	void setLocalId(int localId);
    int getOriginatingOpId() const;
    void setOriginatingOpId(int localId);
	int getYear() const;
	void setYear(int year);
	bool isNewItem() const;
	void setNewItem(bool isNewItem);

private:
	int _localId;
	int _originating_op_id;
	int _halfDays;
	int _year;
	bool _newItem;
};

#endif /* ROLLBACKITEM_HPP_ */
