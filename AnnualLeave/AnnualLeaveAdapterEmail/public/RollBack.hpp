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

#ifndef ROLLBACK_HPP_
#define ROLLBACK_HPP_

#define ROLLBACK_SCHEMA_VERSION_1 1
#define ROLLBACK_SCHEMA_VERSION_CURRENT ROLLBACK_SCHEMA_VERSION_1

#include <RollBackItem.hpp>
#include <Settings.hpp>
#include <common.hpp>

#include <QObject>
#include <QList>
#include <QDataStream>

typedef QList<RollBackItem> RollBackList;

class Q_DECL_EXPORT RollBack
	: public QObject
{
	Q_OBJECT

public:
	virtual ~RollBack();
	static RollBack *getInstance(QObject *parent);
	void addItem(int localId, int originating_op_id, int year, int halfDays);
	void addOldItem(int localId, int originating_op_id, int year, int halfDays);
	void addNewItem(int localId, int originating_op_id, int year, int halfDays);
	void deleteItem(int localId, int originating_op_id);
	void deleteItem(int localId);
	void save();
	void restore();
	void clear();
	bool isItemPresent(int localId);
	bool isItemPresent(int localId, int originating_op_id);
	int getHalfDays(int localId, int originating_op_id);
	int getYear(int localId, int originating_op_id);
	int getOldHalfDays(int localId, int originating_op_id);
	int getOldYear(int localId, int originating_op_id);
	int getNewHalfDays(int localId, int originating_op_id);
	int getNewYear(int localId, int originating_op_id);

private:
	RollBack(QObject *parent=NULL);
	static RollBack* _instance;
	RollBackList *_rollBackList;
    Settings *_settings;

    QByteArray marshall();
    void unmarshall(QByteArray serializedData);
};

#endif /* ROLLBACK_HPP_ */
