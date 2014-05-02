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

#ifndef Utilities_h
#define Utilities_h

#include "stdint.h"
#include <QChar>
#include "huctx.h"
#include "sbdef.h"
#include <bb/utility/StringValidator>

class Q_DECL_EXPORT Utilities {

public:
	static uint8_t hexToInt(QChar c1, QChar c2);
	static void hexToIntArray(QString hex_string,uint8_t* output);
	static QString intToHex(int decimal);
	static void alert(QString msg);
    static void toast(QString msg);
    static QString toLocalDateFormat(QString date_string);
    static QString statusName(int status_value);
    static QString statusText(int booking_status, int last_op_status);
    static QString statusIcon(int booking_status, int last_op_status);
    static QString taskStatus(int task_status, int decision);
    static QString taskText(int task_status, int decision, QString user_id, int half_days,QString from_date);
    static QString taskIcon(int task_type, int task_status, int decision);
    static QString taskDecision(int decision);
    static int boolToInt(bool b);
    static bool sameDates(QDateTime date1, QDateTime date2);
    static qint64 toDateOnlyS(QDateTime datetime);
    static qint64 toDateOnlyMs(QDateTime datetime);
    static qint64 toSeconds(QDate date);
    static qint64 toMilliSeconds(QDate date);
    static bool isValidEmailAddress(QString email_address);

private:
	static const QString HEX_CHARS;
	static const char* LAST_OP_STATUS_ICONS [];
	static const int NUMBER_OF_STATUS_VALUES;
};

#endif // ifndef Utilities_h
