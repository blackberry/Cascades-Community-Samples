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

#ifndef SIMPLETESTCALENDAR_HPP_
#define SIMPLETESTCALENDAR_HPP_

#include <QObject>
#include <QTime>
#include <errno.h>

typedef QList<QDate> PublicHolidays;

class Q_DECL_EXPORT SimpleTestCalendar
	: public QObject
{
	Q_OBJECT

public:
	virtual ~SimpleTestCalendar();
	static SimpleTestCalendar *getInstance(QObject *parent);
	int calculateHalfDays(qint64 fromDate, qint64 toDate, bool firstDayHalf, bool lastDayHalf);
	bool dateRangeIsValid(qint64 fromDate, qint64 toDate);

private:
	SimpleTestCalendar(QObject *parent=NULL);
	static SimpleTestCalendar* _instance;
	bool _publicHolidaysAvailable;

	PublicHolidays *_publicHolidays;

	bool isAPublicHoliday(QDate date);
	bool isAWeekendDay(QDate date);
	bool initialisePublicHolidays();
};

#endif /* SIMPLETESTCALENDAR_HPP_ */
