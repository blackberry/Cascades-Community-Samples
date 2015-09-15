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

#include <SimpleTestCalendar.hpp>

SimpleTestCalendar* SimpleTestCalendar::_instance;

SimpleTestCalendar::SimpleTestCalendar(QObject *parent)
	: QObject(parent)
	, _publicHolidaysAvailable(false)
	, _publicHolidays(new PublicHolidays())
{
	 qDebug() << "TTTT SimpleTestCalendar::SimpleTestCalendar()" << endl;

	 if (initialisePublicHolidays()) {
		 _publicHolidaysAvailable = true;
	 }
}

SimpleTestCalendar::~SimpleTestCalendar()
{
	 qDebug() << "TTTT SimpleTestCalendar::~SimpleTestCalendar()" << endl;

	_instance = NULL;
	delete _publicHolidays;
	_publicHolidays = NULL;
}

SimpleTestCalendar *SimpleTestCalendar::getInstance(QObject *parent)
{
    if (_instance == 0) {
        _instance = new SimpleTestCalendar(parent);
    }
    return _instance;
}

bool SimpleTestCalendar::dateRangeIsValid(qint64 fromDate, qint64 toDate)
{
	QDateTime fDateTime;
    QDateTime tDateTime;
    QDateTime cDateTime;

    fDateTime.setMSecsSinceEpoch(fromDate);
    tDateTime.setMSecsSinceEpoch(toDate);
    cDateTime = QDateTime::currentDateTime();

    QDate fDate = fDateTime.date();
    QDate tDate = tDateTime.date();
    QDate cDate = cDateTime.date();

    int daysBetween = fDate.daysTo(tDate);

    if (daysBetween < 0) {
        return false;
    }
    if ((fDate.year() < cDate.year()) || (cDate.year() > tDate.year())) {
    	return false;
    }
    return true;
}

int SimpleTestCalendar::calculateHalfDays(qint64 fromDateS, qint64 toDateS, bool firstDayHalf, bool lastDayHalf)
{
    QDateTime fDateTime;
    QDateTime tDateTime;

    fDateTime.setMSecsSinceEpoch(fromDateS * 1000);
    tDateTime.setMSecsSinceEpoch(toDateS * 1000);

    QDate fDate = fDateTime.date();
    QDate tDate = tDateTime.date();

    int daysBetween = fDate.daysTo(tDate);
    int totalHalfDays;

    if (daysBetween < 0) {
        return 0;
    }

    QDate currDate;
    int weekendDays = 0;

    for (currDate = fDate; currDate <= tDate; currDate = currDate.addDays(1)) {
        if (isAWeekendDay(currDate)|| isAPublicHoliday(currDate)) {
            weekendDays++;
        }
    }

    totalHalfDays = (daysBetween - weekendDays + 1) * 2;

    if ((daysBetween == 0) && firstDayHalf && lastDayHalf) {
        totalHalfDays = 1;
    } else if ((daysBetween > 0) && firstDayHalf) {
        totalHalfDays--;
    } else if ((daysBetween > 0) && lastDayHalf) {
        totalHalfDays--;
    }

    return totalHalfDays;
}

bool SimpleTestCalendar::isAWeekendDay(QDate date)
{
	/*
	 * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE *
	 * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE *
	 *
	 * TODO: for testing only
	 *
	 * This is for illustrative and test purposes only. This sort of information
	 * in real life depends on regional, and cultural variations. This assumes
	 * the Western convention that the Weekend is Saturday and Sunday rather
	 * than, say, Friday and Saturday as in other regions.
	 *
	 * In the end this is an Adapter Implementation decision
	 *
	 * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE *
	 * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE *
	 */

	if ((date.dayOfWeek() == 6) || (date.dayOfWeek() == 7)) {
    	return true;
    }
    return false;
}

bool SimpleTestCalendar::isAPublicHoliday(QDate date)
{
	if (!_publicHolidaysAvailable) {
		return false;
	}

	PublicHolidays listOfDates = *_publicHolidays;
	PublicHolidays::iterator curr;

	for (curr = listOfDates.begin(); curr != listOfDates.end(); curr++) {
		if ((*curr) == date) {
			return true;
		}
	}
	return false;
}

bool SimpleTestCalendar::initialisePublicHolidays()
{
	/*
	 * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE *
	 * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE *
	 *
	 * TODO: for testing only
	 *
	 * This is for illustrative and test purposes only. This sort of information
	 * in real life depends on regional variations. This date could be cached from
	 * a Web Service as an example to allow offline date calculations.
	 *
	 * In the end this is an Adapter Implementation decision
	 *
	 * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE *
	 * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE * NOTE *
	 */

	//--------------------------------------------------------------------------------
	// 2014 - Bank holiday in England and Wales  - https://www.gov.uk/bank-holidays --
	//--------------------------------------------------------------------------------

	_publicHolidays->append(QDate(2014,  1,  1)); // Bank holiday in England and Wales - New Year’s Day
	_publicHolidays->append(QDate(2014,  4, 18)); // Bank holiday in England and Wales - Good Friday
	_publicHolidays->append(QDate(2014,  4, 21)); // Bank holiday in England and Wales - Easter Monday
	_publicHolidays->append(QDate(2014,  5,  5)); // Bank holiday in England and Wales - Early May bank holiday
	_publicHolidays->append(QDate(2014,  5, 26)); // Bank holiday in England and Wales - Spring bank holiday
	_publicHolidays->append(QDate(2014,  8, 25)); // Bank holiday in England and Wales - Summer bank holiday
	_publicHolidays->append(QDate(2014, 12, 25)); // Bank holiday in England and Wales - Christmas Day
	_publicHolidays->append(QDate(2014, 12, 26)); // Bank holiday in England and Wales - Boxing Day
	_publicHolidays->append(QDate(2014, 12, 29)); // BlackBerry day UK
	_publicHolidays->append(QDate(2014, 12, 30)); // BlackBerry day UK
	_publicHolidays->append(QDate(2014, 12, 31)); // BlackBerry day UK

	//--------------------------------------------------------------------------------
	// 2015 - Bank holiday in England and Wales  - https://www.gov.uk/bank-holidays --
	//--------------------------------------------------------------------------------

	_publicHolidays->append(QDate(2015,  1,  1)); // Bank holiday in England and Wales - New Year’s Day
	_publicHolidays->append(QDate(2015,  4,  3)); // Bank holiday in England and Wales - Good Friday
	_publicHolidays->append(QDate(2015,  4,  6)); // Bank holiday in England and Wales - Easter Monday
	_publicHolidays->append(QDate(2015,  5,  4)); // Bank holiday in England and Wales - Early May bank holiday
	_publicHolidays->append(QDate(2015,  5, 25)); // Bank holiday in England and Wales - Spring bank holiday
	_publicHolidays->append(QDate(2015,  8, 31)); // Bank holiday in England and Wales - Summer bank holiday
	_publicHolidays->append(QDate(2015, 12, 25)); // Bank holiday in England and Wales - Christmas Day
	_publicHolidays->append(QDate(2015, 12, 28)); // Bank holiday in England and Wales - Boxing Day (substitute)
	_publicHolidays->append(QDate(2015, 12, 29)); // BlackBerry day UK
	_publicHolidays->append(QDate(2015, 12, 30)); // BlackBerry day UK
	_publicHolidays->append(QDate(2015, 12, 31)); // BlackBerry day UK

	return true;
}
