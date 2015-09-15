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

#include "Utilities.hpp"
#include "stdint.h"
#include "huctx.h"
#include <QChar>
#include <QString>
#include <QDate>
#include <QDebug>
#include <bb/system/SystemDialog>
#include <bb/system/SystemToast>
#include "husha2.h"
#include "hugse56.h"
#include "sbreturn.h"
#include "sbmem.h"
#include "common.hpp"
#include <bb/utility/StringValidator>

const QString Utilities::HEX_CHARS = QString("0123456789ABCDEF");

const char* Utilities::LAST_OP_STATUS_ICONS[] = { "asset:///images/waiting.png", "asset:///images/manager.png", "asset:///images/approved.png", "asset:///images/denied.png",
        "asset:///images/cancellation_pending.png", "asset:///images/cancellation_submitted.png", "asset:///images/cancelled.png", "asset:///images/cancellation_rejected.png",
        "asset:///images/update_pending.png", "asset:///images/update_submitted.png", "asset:///images/updated.png", "asset:///images/update_rejected.png" };

const int Utilities::NUMBER_OF_STATUS_VALUES = 12;

uint8_t Utilities::hexToInt(QChar c1, QChar c2)
{
    c1 = c1.toUpper();
    c2 = c2.toUpper();
    uint8_t result = 0;
    result = HEX_CHARS.indexOf(c1) * 16;
    result = result + HEX_CHARS.indexOf(c2);
    return result;
}

void Utilities::hexToIntArray(QString hex_string, uint8_t* output)
{
    const size_t numdigits = hex_string.length() / 2;
    for (size_t i = 0; i != numdigits; ++i) {
        output[i] = hexToInt(hex_string.at(2 * i), hex_string.at(2 * i + 1));
    }
}

QString Utilities::intToHex(int decimal)
{
    QString hexadecimal;
    hexadecimal.setNum(decimal, 16);
    return hexadecimal;
}

void Utilities::alert(QString msg)
{
    bb::system::SystemDialog *dial;
    dial = new bb::system::SystemDialog("OK", NULL);
    dial->setTitle("Alert");
    dial->setBody(msg);
    dial->setDismissAutomatically(true);
    dial->show();
}

void Utilities::toast(QString msg)
{
    bb::system::SystemToast toast;
    toast.setBody(msg);
    toast.setPosition(bb::system::SystemUiPosition::MiddleCenter);
    toast.exec();
}

QString Utilities::toLocalDateFormat(QString date_string)
{
    // converts from the standard SQLite date string format of YYYY-MM-DD to a format appropriate to the locale
    QDate the_date = QDate::fromString(date_string, "yyyy-MM-dd");
    return the_date.toString(Qt::DefaultLocaleShortDate);
}

QString Utilities::statusName(int status)
{
    switch (status) {
        case STATUS_BOOKING_PENDING:
            return QString("REQUESTED");
            break;
        case STATUS_BOOKING_SUBMITTED:
            return QString("REQUESTED");
            break;
        case STATUS_BOOKING_APPROVED:
            return QString("APPROVED");
            break;
        case STATUS_BOOKING_REJECTED:
            return QString("REJECTED");
            break;
        case STATUS_CANCELLATION_PENDING:
            return QString("CANCELLING");
            break;
        case STATUS_CANCELLATION_SUBMITTED:
            return QString("CANCELLING");
            break;
        case STATUS_CANCELLATION_OK:
            return QString("CANCELLED");
            break;
        case STATUS_CANCELLATION_REJECTED:
            return QString("CANCELLATION REJECTED");
            break;
        case STATUS_UPDATE_PENDING:
            return QString("UPDATING");
            break;
        case STATUS_UPDATE_SUBMITTED:
            return QString("UPDATING");
            break;
        case STATUS_UPDATE_OK:
            return QString("UPDATED");
            break;
        case STATUS_UPDATE_REJECTED:
            return QString("UPDATE REJECTED");
            break;
        case STATUS_FAILED:
            return QString("OPERATION FAILED");
            break;
        default:
            return QString("Unrecognised status:%1").arg(status);
    }
}

QString Utilities::statusText(int booking_status, int last_op_status)
{

    switch (last_op_status) {
        case -1:
            return statusName(booking_status);
        case 0:
            return statusName(booking_status);
        case 1:
            return statusName(booking_status);
        case 2:
            return statusName(booking_status);
        case 3:
            return statusName(booking_status);
        case 4:
            return statusName(booking_status) + " (" + statusName(last_op_status) + ")";
        case 5:
            return statusName(booking_status) + " (" + statusName(last_op_status) + ")";
        case 6:
            return statusName(booking_status);
        case 7:
            return statusName(booking_status) + " (" + statusName(last_op_status) + ")";
        case 8:
            return statusName(booking_status) + " (" + statusName(last_op_status) + ")";
        case 9:
            return statusName(booking_status) + " (" + statusName(last_op_status) + ")";
        case 10:
            return statusName(booking_status) + " (" + statusName(last_op_status) + ")";
        case 11:
            return statusName(booking_status) + " (" + statusName(last_op_status) + ")";
        default:
            qDebug() << "QQQQ ERROR: unexpected last_op_status in statusText:" << last_op_status;
            return QString("ERROR - UNRECOGNISED STATUS %1").arg(last_op_status);
    }

}

QString Utilities::statusIcon(int booking_status, int last_op_status)
{
    QString icon_path("asset:///images/unknown.png");

    if (last_op_status < NUMBER_OF_STATUS_VALUES) {
        if (last_op_status > -1) {
            icon_path = QString(LAST_OP_STATUS_ICONS[last_op_status]);
        } else {
            icon_path = QString(LAST_OP_STATUS_ICONS[booking_status]);
        }
    }
    return icon_path;
}

QString Utilities::taskText(int task_status, int decision, QString user_id, int half_days, QString from_date)
{

    QString status("");

    status = taskStatus(task_status, decision);

    float days = 0.0;
    if (half_days > 0) {
        days = half_days / 2.0;
    }
    QString day_text("day");
    if (days > 1) {
        day_text = "days";
    }

    QString text("%1 : %2 (%3 %4)");

    text = text.arg(user_id).arg(from_date).arg(days).arg(day_text);

    return text;

}

QString Utilities::taskIcon(int task_type, int task_status, int decision)
{
    qDebug() << "GGGG taskIcon task_type=" << task_type << ",task_status=" << task_status << ",decision=" << decision;
    QString icon_path("asset:///images/unknown.png");

    switch (task_type) {
        case TASK_TYPE_BOOKING_APPROVAL:
            switch (task_status) {
                case TASK_STATUS_AWAITING_MANAGER:
                    return QString("asset:///images/manager.png");
                case TASK_STATUS_PENDING:
                    return QString("asset:///images/waiting.png");
                case TASK_STATUS_SUBMITTED:
                    if (decision == TASK_APPROVAL_DECISION_APPROVED) {
                        return QString("asset:///images/approved.png");
                    } else {
                        return QString("asset:///images/denied.png");
                    }
                case TASK_STATUS_FAILED:
                    return QString("asset:///images/failed.png");
                default:
                    qDebug() << "QQQQ ERROR: unexpected task_status in taskIcon:" << task_status;
                    return icon_path;
            }
            break;
        case TASK_TYPE_CANCEL_APPROVAL:
            switch (task_status) {
                case TASK_STATUS_AWAITING_MANAGER:
                    return QString("asset:///images/cancellation_submitted.png");
                case TASK_STATUS_PENDING:
                    return QString("asset:///images/cancellation_pending.png");
                case TASK_STATUS_SUBMITTED:
                    if (decision == TASK_APPROVAL_DECISION_APPROVED) {
                        return QString("asset:///images/cancelled.png");
                    } else {
                        return QString("asset:///images/cancellation_rejected.png");
                    }
                case TASK_STATUS_FAILED:
                    return QString("asset:///images/failed.png");
                default:
                    qDebug() << "QQQQ ERROR: unexpected task_status in taskIcon:" << task_status;
                    return icon_path;
            }
            break;
        case TASK_TYPE_UPDATE_APPROVAL:
            switch (task_status) {
                case TASK_STATUS_AWAITING_MANAGER:
                    return QString("asset:///images/update_submitted.png");
                case TASK_STATUS_PENDING:
                    return QString("asset:///images/update_pending.png");
                case TASK_STATUS_SUBMITTED:
                    if (decision == TASK_APPROVAL_DECISION_APPROVED) {
                        return QString("asset:///images/updated.png");
                    } else {
                        return QString("asset:///images/update_rejected.png");
                    }
                case TASK_STATUS_FAILED:
                    return QString("asset:///images/failed.png");
                default:
                    qDebug() << "QQQQ ERROR: unexpected task_status in taskIcon:" << task_status;
                    return icon_path;
            }
            break;
        default:
            qDebug() << "QQQQ ERROR: unexpected task_type in taskIcon:" << task_type;
            return icon_path;
    }
}

QString Utilities::taskDecision(int decision)
{

    QString text("");

    switch (decision) {
        case 0:
            return "UNDECIDED";
        case 1:
            text = "APPROVED";
            break;
        case 2:
            text = "REJECTED";
            break;
        default:
            text = "UNKNOWN";
            break;
    }

    return text;

}

QString Utilities::taskStatus(int task_status, int decision)
{

    QString status("");

    switch (task_status) {
        case TASK_STATUS_AWAITING_MANAGER:
            status = QString("Awaiting Manager's Decision");
            break;
        case TASK_STATUS_PENDING:
            status = QString("Pending");
            break;
        case TASK_STATUS_SUBMITTED:
            if (decision == TASK_APPROVAL_DECISION_APPROVED) {
                status = QString("Approved");
            } else {
                status = QString("Rejected");
            }
            break;
        case TASK_STATUS_FAILED:
            status = QString("Failed");
            break;
        default:
            qDebug() << "QQQQ ERROR: unexpected task_status in taskText:" << task_status;
            return QString("ERROR - UNRECOGNISED STATUS %1").arg(task_status);
    }

    return status;

}

int Utilities::boolToInt(bool b)
{
    if (b) {
        return 1;
    } else {
        return 0;
    }
}

bool Utilities::sameDates(QDateTime datetime1, QDateTime datetime2)
{
    QDate date1 = datetime1.date();
    QDate date2 = datetime2.date();
    return (date1 == date2);
}

qint64 Utilities::toDateOnlyS(QDateTime datetime) {
    datetime.setTime(QTime(0,0,0,0));
    qint64 epoch_secs = datetime.toMSecsSinceEpoch() / 1000;
    return epoch_secs;
}

qint64 Utilities::toDateOnlyMs(QDateTime datetime) {
    datetime.setTime(QTime(0,0,0,0));
    qint64 epoch_secs = datetime.toMSecsSinceEpoch();
    return epoch_secs;
}

qint64 Utilities::toSeconds(QDate date) {
    QDateTime datetime(date, QTime(0,0,0,0),Qt::UTC);
    qint64 epoch_secs = datetime.toMSecsSinceEpoch() / 1000;
    return epoch_secs;
}

qint64 Utilities::toMilliSeconds(QDate date) {
    QDateTime datetime(date, QTime(0,0,0,0),Qt::UTC);
    qint64 epoch_secs = datetime.toMSecsSinceEpoch();
    return epoch_secs;
}

bool Utilities::isValidEmailAddress(QString email_address) {
    return bb::utility::StringValidator::isEmailAddress(email_address);
}
