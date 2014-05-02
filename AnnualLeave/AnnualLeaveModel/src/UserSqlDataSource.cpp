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

#include "UserSqlDataSource.hpp"
#include "Utilities.hpp"
#include "Entitlement.hpp"
#include "DbAnnualLeaveRecord.hpp"
#include "common.hpp"

#include <QFile>
#include <QDir>

#include <bb/cascades/Application>
#include <bb/system/SystemDialog>

#include <QtSql/QtSql>

using namespace bb::cascades;
using namespace bb::data;
using namespace bb::system;

UserSqlDataSource* UserSqlDataSource::_instance;
SqlDataAccess* UserSqlDataSource::_sda;

UserSqlDataSource::UserSqlDataSource(QObject *parent) :
        QObject(parent)
{

    QDir home = QDir::home();
    _source = home.absoluteFilePath(USER_DATABASE);

    _sda = new SqlDataAccess(_source);

    qDebug() << "DDDD does the database file already exist at " << _source;
    if (!databaseExists()) {
        qDebug() << "DDDD no, so let's create it";
        createDatabase();
    } else {
        qDebug() << "DDDD database already exists";
    }
    if (!_sda->connection().isOpen()) {
        bool ok = _sda->connection().open();
        qDebug() << "DDDD user database opened " << ok;
        if (!ok) {
            const QSqlError error = _sda->connection().lastError();
            qDebug() << "DDDD result=" << error;
            return;
        }
    } else {
        qDebug() << "DDDD database is already open";
    }

    bool conn_result = connect(this, SIGNAL(signalLoadRequestData(const QString&)), this, SLOT(onLoadRequestData(const QString&)), Qt::QueuedConnection);
    qDebug() << "DDDD loadRequestData signal connected=" << conn_result;

    conn_result = connect(this, SIGNAL(signalLoadTaskData()), this, SLOT(onLoadTaskData()), Qt::QueuedConnection);
    qDebug() << "DDDD loadTaskData signal connected=" << conn_result;

}

UserSqlDataSource::~UserSqlDataSource()
{
    delete _sda;
}

UserSqlDataSource* UserSqlDataSource::getInstance(QObject *parent)
{
    if (_instance == 0) {
        _instance = new UserSqlDataSource(parent);
    }
    return _instance;
}

bool UserSqlDataSource::databaseExists()
{
    QDir home = QDir::home();
    QString filePath = home.absoluteFilePath(USER_DATABASE);
    qDebug() << "DDDD databaseExists: " << filePath;
    QFile file(filePath);
    return file.exists();
}

void UserSqlDataSource::createDatabase()
{

    qDebug() << "DDDD creating database";

    if (_sda != 0) {
        qDebug() << "DDDD closing old database connection";
        _sda->connection().close();
    }
    QDir home = QDir::home();
    QString filePath = _source;

    bool deleted = QFile::remove(filePath);

    qDebug() << "DDDD database file deleted=" << deleted;

    QFile file(filePath);

    bool database_opened = _sda->connection().open();
    qDebug() << "DDDD createDatabase(): database opened " << database_opened;

    if (database_opened) {

        if (file.open(QIODevice::ReadWrite)) {
            qDebug() << "DDDD creating table annual_leave_requests";
            QString create_table_sql1(
                    "CREATE TABLE annual_leave_requests( local_request_id INTEGER PRIMARY KEY AUTOINCREMENT, leave_year integer, from_date_s integer, to_date_s integer, "
                            "first_day_half integer, last_day_half integer, day_half integer, "
                            "half_days integer default 0, booking_status integer default 0, last_update_op_id integer default 0, last_op_status integer default 0, note text, updating_leave_year integer default 0, updating_from_date_s integer default 0, updating_to_date_s integer default 0, "
                            "updating_first_day_half integer default 0, updating_last_day_half integer default 0, updating_day_half integer default 0"
                            ")");
            _sda->execute(create_table_sql1);
            const QSqlError error2 = _sda->connection().lastError();
            qDebug() << "DDDD result=" << error2;

            qDebug() << "DDDD creating table annual_entitlement";
            _sda->execute("CREATE TABLE annual_entitlement( leave_year INTEGER PRIMARY KEY, total_entitlement_half_days integer, current_entitlement_half_days integer);");
            const QSqlError error3 = _sda->connection().lastError();
            qDebug() << "DDDD result=" << error3;

            QString create_table_sql2(
                    "CREATE TABLE approval_tasks( task_id INTEGER PRIMARY KEY AUTOINCREMENT, task_type integer, user_id text, leave_year integer, from_date_s integer, to_date_s integer, "
                            "first_day_half integer, last_day_half integer, day_half integer, "
                            "half_days integer, task_status integer, decision integer, decision_date_s integer, local_request_id integer, note text, updating_leave_year integer, updating_from_date_s integer, updating_to_date_s integer, "
                            "updating_first_day_half integer, updating_last_day_half integer, updating_day_half integer, updating_half_days integer, originating_op_id integer"
                            ")");
            _sda->execute(create_table_sql2);
            const QSqlError error = _sda->connection().lastError();
            qDebug() << "DDDD result=" << error;

        } else {
            const QSqlError error = _sda->connection().lastError();
            qDebug() << "DDDD failed to create database. error=" << error;
        }
    } else {
        const QSqlError error = _sda->connection().lastError();
        qDebug() << "DDDD Don't have read/write access to db file. error=" << error;

    }
}

void UserSqlDataSource::reset()
{
    qDebug() << "DDDD reset is recreating the database";
    createDatabase();
}

void UserSqlDataSource::loadRequestData(QString leave_year)
{

    qDebug() << "DDDD loadRequestData(" << leave_year << ")";

    // The SQLite date() function returns the date in this format: YYYY-MM-DD
    _query = QString("select local_request_id, leave_year, date(from_date_s, 'unixepoch' , 'localtime') as 'from_date', ");
    _query.append("date(to_date_s, 'unixepoch', 'localtime') as 'to_date', half_days, booking_status, last_op_status, note, updating_leave_year, ");
    _query.append("date(updating_from_date_s, 'unixepoch' , 'localtime') as 'updating_from_date', date(updating_to_date_s, 'unixepoch' , 'localtime') as 'updating_to_date',");
    _query.append("first_day_half, last_day_half, day_half,");
    _query.append("updating_first_day_half, updating_last_day_half, updating_day_half");
    _query.append(" from annual_leave_requests where leave_year=%1");
    _query = _query.arg(leave_year);
    qDebug() << "DDDD _query=" << _query;
    qDebug() << "DDDD emitting loadRequestData() signal";
    emit signalLoadRequestData(_query);
}

void UserSqlDataSource::onLoadRequestData(const QString& sql)
{

    qDebug() << "DDDD onLoadRequestData: " << sql;
    QVariant list = _sda->execute(sql);

    _requests_model_list = list.value<QVariantList>();
    qDebug() << "DDDD about to emit signalRequestsLoaded: " << _requests_model_list;
    emit signalRequestsLoaded(_requests_model_list);
}

QVariant UserSqlDataSource::getRequests(int leave_year)
{
    qDebug() << "DDDD getRequests: " << leave_year;
    _query = QString("select local_request_id, leave_year, from_date_s,");
    _query.append("to_date_s, booking_status, ");
    _query.append("first_day_half, last_day_half, day_half");
    _query.append(" from annual_leave_requests where leave_year=%1");
    _query = _query.arg(leave_year);
    QVariant list = _sda->execute(_query);
    return list;
}

void UserSqlDataSource::loadTaskData()
{
    emit signalLoadTaskData();
}

void UserSqlDataSource::onLoadTaskData()
{
    qDebug() << "DDDD onLoadTaskData";

    // The SQLite date() function returns the date in this format: YYYY-MM-DD
    QString sql("select task_id, task_type, local_request_id, decision, user_id, leave_year, date(from_date_s, 'unixepoch' , 'localtime') as 'from_date', ");
    sql.append("date(to_date_s, 'unixepoch', 'localtime') as 'to_date', half_days, task_status, note,");
    sql.append("first_day_half, last_day_half, day_half, date(decision_date_s, 'unixepoch' , 'localtime') as 'decision_date',");
    sql.append("updating_leave_year, date(updating_from_date_s, 'unixepoch' , 'localtime') as 'updating_from_date', date(updating_to_date_s, 'unixepoch', 'localtime') as 'updating_to_date',");
    sql.append("updating_first_day_half, updating_last_day_half, updating_day_half, originating_op_id");
    sql.append(" from approval_tasks order by task_id desc");

    qDebug() << "DDDD sql=" << sql;
    QVariant list = _sda->execute(sql);

    _tasks_model_list = list.value<QVariantList>();
    qDebug() << "DDDD about to emit tasksLoaded: " << _tasks_model_list;
    emit signalTasksLoaded(_tasks_model_list);
}

int UserSqlDataSource::lastAlId()
{
    int local_request_id = -1;
    QVariant result = _sda->execute("select max(local_request_id) from annual_leave_requests");
    QVariantList list = result.value<QVariantList>();
    QVariant row = list.first();
    local_request_id = row.toMap().value("max(local_request_id)").toInt();
    return local_request_id;
}

int UserSqlDataSource::lastTaskId()
{
    int task_id = -1;
    QVariant result = _sda->execute("select max(task_id) from approval_tasks");
    QVariantList list = result.value<QVariantList>();
    QVariant row = list.first();
    task_id = row.toMap().value("max(task_id)").toInt();
    return task_id;
}

int UserSqlDataSource::getCurrentStatus(int local_request_id)
{
    int booking_status = -1;
    QString select_sql("select booking_status from annual_leave_requests where local_request_id=%1");
    select_sql = select_sql.arg(local_request_id);
    qDebug() << "DDDD getCurrentStatus:" << select_sql;
    QVariant result = _sda->execute(select_sql);
    QVariantList list = result.value<QVariantList>();
    if (list.size() > 0) {
        QVariant row = list.first();
        booking_status = row.toMap().value("booking_status").toInt();
    } else {
        booking_status = -1;
    }
    return booking_status;
}

int UserSqlDataSource::getLastOpStatus(int local_request_id)
{
    int last_op_status = -1;
    QString select_sql("select last_op_status from annual_leave_requests where local_request_id=%1");
    select_sql = select_sql.arg(local_request_id);
    qDebug() << "DDDD getLastOpStatus:" << select_sql;
    QVariant result = _sda->execute(select_sql);
    QVariantList list = result.value<QVariantList>();
    if (list.size() > 0) {
        QVariant row = list.first();
        last_op_status = row.toMap().value("last_op_status").toInt();
    } else {
        last_op_status = -1;
    }
    return last_op_status;
}

void UserSqlDataSource::updateEntitlement(int leave_year, int total_entitlement_half_days, int current_entitlement_half_days)
{
    QString insert_sql("INSERT OR REPLACE INTO annual_entitlement(leave_year, total_entitlement_half_days, current_entitlement_half_days) VALUES(%1,%2,%3)");
    insert_sql = insert_sql.arg(leave_year).arg(total_entitlement_half_days).arg(current_entitlement_half_days);
    qDebug() << "DDDD updateEntitlement:" << insert_sql;
    _sda->execute(insert_sql);
}

Entitlement UserSqlDataSource::getEntitlement(int leave_year)
{
    Entitlement entitlement;
    QString select_sql("select total_entitlement_half_days, current_entitlement_half_days from annual_entitlement where leave_year=%1");
    select_sql = select_sql.arg(leave_year);
    qDebug() << "DDDD getEntitlement:" << select_sql;
    QVariant result = _sda->execute(select_sql);
    QVariantList list = result.value<QVariantList>();
    if (list.size() > 0) {
        QVariant row = list.first();
        entitlement.setTotalEntitlementHalfDays(row.toMap().value("total_entitlement_half_days").toInt());
        entitlement.setCurrentEntitlementHalfDays(row.toMap().value("current_entitlement_half_days").toInt());
    } else {
        qDebug() << "DDDD no annual_entitlement data for " << leave_year;
        entitlement.setTotalEntitlementHalfDays(-1);
        entitlement.setCurrentEntitlementHalfDays(-1);
    }
    return entitlement;
}

int UserSqlDataSource::storeRequest(int fdate, int tdate, QString note, int leave_year, bool first_day_half, bool last_day_half, int day_half, int half_days)
{
    qDebug()
            << QString("DDDD storeRequest: from_date=%1 to_date=%2 note=%3 first_day_half=%4 last_day_half=%5 day_half=%6 half_days=%7").arg(fdate).arg(tdate).arg(note).arg(first_day_half).arg(last_day_half).arg(
                    day_half).arg(half_days);

    int fdh = Utilities::boolToInt(first_day_half);
    int ldh = Utilities::boolToInt(last_day_half);

    QString insert_sql("INSERT INTO annual_leave_requests(");
    insert_sql.append("leave_year, from_date_s, to_date_s, half_days, booking_status, last_op_status, note, updating_leave_year, updating_from_date_s, updating_to_date_s,");
    insert_sql.append("first_day_half, last_day_half, day_half) ");
    insert_sql.append("VALUES(%1,%2,%3,%4,%5,-1,'%6',0,0,0,%7, %8, %9)");
    insert_sql = insert_sql.arg(leave_year).arg(fdate).arg(tdate).arg(half_days).arg(STATUS_BOOKING_PENDING).arg(note.toAscii().constData()).arg(fdh).arg(ldh).arg(day_half);
    qDebug() << "DDDD storeRequest:" << insert_sql;
    _sda->execute(insert_sql);

    int id = lastAlId();

    // call Ops API to place submit_al request in outbound queue and notify the headless service that there is an operation to process

    loadRequestData(QString::number(leave_year));

    return id;
}

int UserSqlDataSource::bookingCount(int fdate, int tdate, bool first_day_half, bool last_day_half, int day_half)
{

    qDebug() << QString("DDDD bookingCount: from_date_s=%1 and to_date_s=%2 and first_day_half=%3 and last_day_half=%4 and day_half=%5").arg(fdate).arg(tdate).arg(first_day_half).arg(last_day_half).arg(day_half);

    int fdh = Utilities::boolToInt(first_day_half);
    int ldh = Utilities::boolToInt(last_day_half);

    int count = -1;
    QString count_sql("select count(*) from annual_leave_requests where from_date_s=%1 and to_date_s=%2 and first_day_half=%3 and last_day_half=%4 and day_half=%5");
    count_sql = count_sql.arg(fdate).arg(tdate).arg(fdh).arg(ldh).arg(day_half);
    qDebug() << "DDDD " << count_sql;
    QVariant result = _sda->execute(count_sql);
    QVariantList list = result.value<QVariantList>();
    QVariant row = list.first();
    qDebug() << "DDDD " << row;
    count = row.toMap().value("count(*)").toInt();
    return count;
}

void UserSqlDataSource::updateRequest(int local_request_id, int originating_op_id, int current_leave_year, int updating_leave_year, int updating_from_date_s, int updating_to_date_s,
        bool updating_first_day_half, bool updating_last_day_half, int updating_day_half, QString note)
{
    qDebug()
            << QString("DDDD updateRequest: local_request_id=%4 originating_op_id=%6 leave_year=%5 updating_from_date=%1 updating_to_date=%2 note=%3").arg(updating_from_date_s).arg(updating_to_date_s).arg(
                    note).arg(local_request_id).arg(updating_leave_year).arg(originating_op_id);
    QString update_sql(
            "UPDATE annual_leave_requests set updating_leave_year=%1, updating_from_date_s=%2, updating_to_date_s=%3, last_op_status=%5, note='%6', updating_first_day_half=%7, updating_last_day_half=%8, updating_day_half=%9, last_update_op_id=%10 where local_request_id=%11");
    update_sql = update_sql.arg(updating_leave_year).arg(updating_from_date_s).arg(updating_to_date_s).arg(STATUS_UPDATE_PENDING).arg(note.toAscii().constData()).arg(updating_first_day_half).arg(
            updating_last_day_half).arg(updating_day_half).arg(originating_op_id).arg(local_request_id);
    qDebug() << "DDDD updateRequest:" << update_sql;
    _sda->execute(update_sql);

    loadRequestData(QString::number(current_leave_year));

}

void UserSqlDataSource::applyBookingUpdate(int local_request_id, int half_days)
{
    qDebug() << QString("DDDD applyBookingUpdate: local_request_id=%1").arg(local_request_id);

    DbAnnualLeaveRecord current_values = getAnnualLeaveRequest(local_request_id);

    QString update_sql("UPDATE annual_leave_requests set leave_year=%1, from_date_s=%2, to_date_s=%3, "
            "first_day_half=%4, last_day_half=%5, day_half=%6, "
            "updating_leave_year=0, updating_from_date_s=0, updating_to_date_s=0,  "
            "updating_first_day_half=0, updating_last_day_half=0, updating_day_half=0, "
            "booking_status=%7, last_op_status=%8, half_days=%9 "
            "where local_request_id=%10");

    update_sql = update_sql.arg(current_values.updatingLeaveYear()).arg(current_values.updatingFromDateS()).arg(current_values.updatingToDateS()).arg(current_values.updatingFirstDayHalf()).arg(
            current_values.updatingLastDayHalf()).arg(current_values.updatingDayHalf()).arg(STATUS_BOOKING_APPROVED).arg(STATUS_UPDATE_OK).arg(half_days).arg(local_request_id);

    qDebug() << "DDDD applyBookingUpdate:" << update_sql;
    _sda->execute(update_sql);

    loadRequestData(QString::number(current_values.updatingLeaveYear()));
}

int UserSqlDataSource::countBookingUpdates(int local_request_id, int originating_op_id)
{
    qDebug() << QString("DDDD countBookingUpdates: local_request_id=%1 originating_op_id=%2").arg(local_request_id).arg(originating_op_id);
    int count = -1;
    QString count_sql("select count(*) from annual_leave_requests where local_request_id=%1 and last_update_op_id=%2");
    count_sql = count_sql.arg(local_request_id).arg(originating_op_id);
    qDebug() << "DDDD " << count_sql;
    QVariant result = _sda->execute(count_sql);
    QVariantList list = result.value<QVariantList>();
    QVariant row = list.first();
    qDebug() << "DDDD " << row;
    count = row.toMap().value("count(*)").toInt();
    return count;
}

int UserSqlDataSource::countTasks()
{
    qDebug() << QString("DDDD countTasks");
    int count = -1;
    QString count_sql("select count(*) from approval_tasks");
    QVariant result = _sda->execute(count_sql);
    QVariantList list = result.value<QVariantList>();
    QVariant row = list.first();
    count = row.toMap().value("count(*)").toInt();
    return count;
}

DbAnnualLeaveRecord UserSqlDataSource::getAnnualLeaveRequest(int local_request_id)
{

    DbAnnualLeaveRecord record;

    QString select_sql("select * from annual_leave_requests where local_request_id=%1");
    select_sql = select_sql.arg(local_request_id);
    qDebug() << "DDDD getAnnualLeaveRequest:" << select_sql;
    QVariant result = _sda->execute(select_sql);
    QVariantList list = result.value<QVariantList>();

    int leave_year;
    int from_date_s;
    int to_date_s;
    int day_half;
    int first_day_half;
    int last_day_half;
    int half_days;
    int booking_status;
    QString note;
    int updating_leave_year;
    int updating_from_date_s;
    int updating_to_date_s;
    int updating_first_day_half;
    int updating_last_day_half;
    int updating_day_half;

    if (list.size() > 0) {
        QVariant row = list.first();
        leave_year = row.toMap().value("leave_year").toInt();
        from_date_s = row.toMap().value("from_date_s").toInt();
        to_date_s = row.toMap().value("to_date_s").toInt();
        first_day_half = row.toMap().value("first_day_half").toInt();
        last_day_half = row.toMap().value("last_day_half").toInt();
        day_half = row.toMap().value("day_half").toInt();
        half_days = row.toMap().value("half_days").toInt();
        booking_status = row.toMap().value("booking_status").toInt();
        note = row.toMap().value("note").toString();
        updating_leave_year = row.toMap().value("updating_leave_year").toInt();
        updating_from_date_s = row.toMap().value("updating_from_date_s").toInt();
        updating_to_date_s = row.toMap().value("updating_to_date_s").toInt();
        updating_first_day_half = row.toMap().value("updating_first_day_half").toInt();
        updating_last_day_half = row.toMap().value("updating_last_day_half").toInt();
        updating_day_half = row.toMap().value("updating_day_half").toInt();

        record.setLocalRequestId(local_request_id);
        record.setLeaveYear(leave_year);
        record.setFromDateS(from_date_s);
        record.setToDateS(to_date_s);
        record.setFirstDayHalf(first_day_half);
        record.setLastDayHalf(last_day_half);
        record.setDayHalf(day_half);
        record.setHalfDays(half_days);
        record.setAlStatus(booking_status);
        record.setNote(note);
        record.setUpdatingLeaveYear(updating_leave_year);
        record.setUpdatingFromDateS(updating_from_date_s);
        record.setUpdatingToDateS(updating_to_date_s);
        record.setUpdatingFirstDayHalf(updating_first_day_half);
        record.setUpdatingLastDayHalf(updating_last_day_half);
        record.setUpdatingDayHalf(updating_day_half);

    } else {
        record.setLocalRequestId(-1);
    }
    return record;
}

int UserSqlDataSource::getApprovalTaskYear(int task_id)
{

    QString select_sql("select leave_year from approval_tasks where task_id=%1");
    select_sql = select_sql.arg(task_id);
    qDebug() << "DDDD getApprovalTaskYear:" << select_sql;
    QVariant result = _sda->execute(select_sql);
    QVariantList list = result.value<QVariantList>();

    int leave_year;

    if (list.size() > 0) {
        QVariant row = list.first();
        leave_year = row.toMap().value("leave_year").toInt();
    } else {
        leave_year = -1;
    }
    return leave_year;
}

void UserSqlDataSource::updateHalfDays(int local_request_id, int half_days)
{
    qDebug() << QString("DDDD updateHalfDays: local_request_id=%2 half_days=%1").arg(half_days).arg(local_request_id);
    QString update_sql("UPDATE annual_leave_requests set half_days=%1 where local_request_id=%2");
    update_sql = update_sql.arg(half_days).arg(local_request_id);
    qDebug() << "DDDD updateHalfDays:" << update_sql;
    _sda->execute(update_sql);

}

void UserSqlDataSource::cancelRequest(int local_request_id)
{
    updateLastOpStatus(local_request_id, STATUS_CANCELLATION_PENDING);
}

void UserSqlDataSource::updateRequestStatus(int local_request_id, int op_status)
{

    qDebug() << "DDDD updateRequestStatus local_request_id=" << local_request_id << " op_status=" << op_status;

    QString update_sql("UPDATE annual_leave_requests set last_op_status=");
    update_sql.append(QString::number(op_status));

    switch (op_status) {
        case STATUS_BOOKING_PENDING:
            update_sql.append(", booking_status=").append(QString::number(STATUS_BOOKING_PENDING)).append(" where local_request_id=").append(QString::number(local_request_id));
            break;
        case STATUS_BOOKING_SUBMITTED:
            update_sql.append(", booking_status=").append(QString::number(STATUS_BOOKING_SUBMITTED)).append(" where local_request_id=").append(QString::number(local_request_id));
            break;
        case STATUS_BOOKING_APPROVED:
            update_sql.append(", booking_status=").append(QString::number(STATUS_BOOKING_APPROVED)).append(" where local_request_id=").append(QString::number(local_request_id));
            break;
        case STATUS_BOOKING_REJECTED:
            update_sql.append(", booking_status=").append(QString::number(STATUS_BOOKING_REJECTED)).append(" where local_request_id=").append(QString::number(local_request_id));
            break;
        case STATUS_CANCELLATION_PENDING:
            // don't change the booking status since the booking itself has not yet had its status changed
            update_sql.append(" where local_request_id=").append(QString::number(local_request_id));
            break;
        case STATUS_CANCELLATION_SUBMITTED:
            // don't change the booking status since the booking itself has not yet had its status changed
            update_sql.append(" where local_request_id=").append(QString::number(local_request_id));
            break;
        case STATUS_CANCELLATION_OK:
            update_sql.append(", booking_status=").append(QString::number(STATUS_CANCELLATION_OK)).append(" where local_request_id=").append(QString::number(local_request_id));
            break;
        case STATUS_CANCELLATION_REJECTED:
            // don't change the booking status since the booking itself has not yet had its status changed
            update_sql.append(" where local_request_id=").append(QString::number(local_request_id));
            break;
        case STATUS_UPDATE_PENDING:
            // don't change the booking status since the booking itself has not yet had its status changed
            update_sql.append(" where local_request_id=").append(QString::number(local_request_id));
            break;
        case STATUS_UPDATE_SUBMITTED:
            // don't change the booking status since the booking itself has not yet had its status changed
            update_sql.append(" where local_request_id=").append(QString::number(local_request_id));
            break;
        case STATUS_UPDATE_OK:
            // updated OK so this means the booking is approved
            update_sql.append(", booking_status=").append(QString::number(STATUS_BOOKING_APPROVED)).append(" where local_request_id=").append(QString::number(local_request_id));
            break;
        case STATUS_UPDATE_REJECTED:
            // don't change the booking status since the booking itself has not yet had its status changed
            update_sql.append(" where local_request_id=").append(QString::number(local_request_id));
            break;
        default:
            qDebug() << "DDDD ERROR: unexpected op_status in updateRequestStatus:" << op_status;
    }

    qDebug() << "DDDD updateRequestStatus:" << update_sql;
    _sda->execute(update_sql);

}

void UserSqlDataSource::updateLastOpStatus(int local_request_id, int op_status)
{
    qDebug() << "DDDD updateLastOpStatus local_request_id=" << local_request_id << " op_status=" << op_status;
    QString update_sql("UPDATE annual_leave_requests set last_op_status=%1 where local_request_id=%2");
    update_sql = update_sql.arg(op_status).arg(local_request_id);
    qDebug() << "DDDD updateLastOpStatus:" << update_sql;
    _sda->execute(update_sql);
}

void UserSqlDataSource::setOriginatingOpId(int local_request_id, int originating_op_id)
{
    qDebug() << "DDDD setOriginatingOpId local_request_id=" << local_request_id << " originating_op_id=" << originating_op_id;
    QString update_sql("UPDATE annual_leave_requests set last_update_op_id=%1 where local_request_id=%2");
    update_sql = update_sql.arg(originating_op_id).arg(local_request_id);
    qDebug() << "DDDD setOriginatingOpId:" << update_sql;
    _sda->execute(update_sql);
}

void UserSqlDataSource::clearProcessingStatus(int local_request_id, int leave_year)
{
    qDebug() << "DDDD clearProcessingStatus local_request_id=" << local_request_id;
    QString update_sql("UPDATE annual_leave_requests set last_op_status=-1 where local_request_id=%1");
    update_sql = update_sql.arg(local_request_id);
    qDebug() << "DDDD clearProcessingStatus:" << update_sql;
    _sda->execute(update_sql);
    loadRequestData(QString::number(leave_year));
}

void UserSqlDataSource::deleteRequest(int local_request_id)
{
    qDebug() << QString("DDDD deleteRequest: local_request_id=%1").arg(local_request_id);
    QString delete_sql("delete from annual_leave_requests where local_request_id=%1");
    delete_sql = delete_sql.arg(local_request_id);
    qDebug() << "DDDD deleteRequest:" << delete_sql;
    _sda->execute(delete_sql);

}

int UserSqlDataSource::deleteRequest(qint64 from_date_s, qint64 to_date_s, bool first_day_half, bool last_day_half, int day_half)
{
    qDebug()
            << QString("DDDD deleteRequest: from_date_s=%1 to_date_s=%2 first_day_half=%3 last_day_half=%4 day_half=%5").arg(from_date_s).arg(to_date_s).arg(first_day_half).arg(last_day_half).arg(
                    day_half);
    int count = bookingCount(from_date_s, to_date_s, first_day_half, last_day_half, day_half);

    if (count == 1) {
        int fdh = Utilities::boolToInt(first_day_half);
        int ldh = Utilities::boolToInt(last_day_half);

        QString delete_sql("delete from annual_leave_requests where from_date_s=%1 and to_date_s=%2 and first_day_half=%3 and last_day_half=%4 and day_half=%5");
        delete_sql = delete_sql.arg(from_date_s).arg(to_date_s).arg(fdh).arg(ldh).arg(day_half);
        qDebug() << "DDDD deleteRequest:" << delete_sql;
        _sda->execute(delete_sql);
    }
    return count;
}

void UserSqlDataSource::deleteApprovalTask(int task_id)
{
    qDebug() << QString("DDDD deleteApprovalTask: task_id=%1").arg(task_id);
    QString delete_sql("delete from approval_tasks where task_id=%1");
    delete_sql = delete_sql.arg(task_id);
    qDebug() << "DDDD deleteApprovalTask:" << delete_sql;
    _sda->execute(delete_sql);

}

int UserSqlDataSource::storeApprovalTask(QString user_id, int task_type, int fdate, int tdate, int leave_year, bool first_day_half, bool last_day_half, int day_half, int half_days,
        int local_request_id, QString note, int updating_fdate, int updating_tdate, int updating_leave_year, bool updating_first_day_half, bool updating_last_day_half, int updating_day_half,
        int updating_half_days, int originating_op_id)
{

    qDebug() << QString("DDDD storeApprovalTask: user_id=%1 local_request_id=%2 task_type=%3").arg(user_id).arg(local_request_id).arg(task_type);
    int fdh = Utilities::boolToInt(first_day_half);
    int ldh = Utilities::boolToInt(last_day_half);

    int ufdh = Utilities::boolToInt(updating_first_day_half);
    int uldh = Utilities::boolToInt(updating_last_day_half);

    QString insert_sql("INSERT INTO approval_tasks(");
    insert_sql.append("user_id, decision, leave_year, from_date_s, to_date_s, first_day_half, last_day_half, day_half, half_days, task_status, local_request_id, note, ");
    insert_sql.append(
            "task_type, updating_leave_year, updating_from_date_s, updating_to_date_s, updating_first_day_half, updating_last_day_half, updating_day_half, updating_half_days, originating_op_id) ");
    insert_sql.append("VALUES('%1',0,%2,%3,%4,%5,'%6',%7, %8, %9, %10, '%11', %12, %13, %14, %15, %16, %17, %18, %19, %20)");
    insert_sql = insert_sql.arg(user_id).arg(leave_year).arg(fdate).arg(tdate).arg(fdh).arg(ldh).arg(day_half).arg(half_days).arg(TASK_STATUS_AWAITING_MANAGER).arg(local_request_id).arg(
            note.toAscii().constData()).arg(task_type).arg(updating_leave_year).arg(updating_fdate).arg(updating_tdate).arg(ufdh).arg(uldh).arg(updating_day_half).arg(updating_half_days).arg(
            originating_op_id);
    qDebug() << "DDDD storeApprovalTask:" << insert_sql;
    _sda->execute(insert_sql);

    loadTaskData();

    return lastTaskId();
}

void UserSqlDataSource::updateApprovalTask(int task_id, int decision)
{
    qDebug() << QString("DDDD updateApprovalTask: task_id=%1 decision=%2").arg(task_id).arg(decision);
    QString update_sql("UPDATE approval_tasks set task_status=%1, decision=%2, decision_date_s=%3 where task_id=%4");
    QDateTime now = QDateTime::currentDateTime();
    now.setTime(QTime(0, 0, 0, 0));
    int dec_date = now.toMSecsSinceEpoch() / 1000;
    update_sql = update_sql.arg(TASK_STATUS_PENDING).arg(decision).arg(dec_date).arg(task_id);
    qDebug() << "DDDD updateApprovalTask:" << update_sql;
    _sda->execute(update_sql);
    loadTaskData();
}

void UserSqlDataSource::updateApprovalTaskProcessingStatus(int task_id, int task_status)
{
    qDebug() << QString("DDDD updateApprovalTaskProcessingStatus: task_id=%1 decision=%2").arg(task_id).arg(task_status);
    QString update_sql("UPDATE approval_tasks set task_status=%1 where task_id=%2");
    update_sql = update_sql.arg(task_status).arg(task_id);
    qDebug() << "DDDD updateApprovalTaskProcessingStatus:" << update_sql;
    _sda->execute(update_sql);
    loadTaskData();
}

void UserSqlDataSource::copyFileToDataFolder(const QString fileName)
{
    // Since we need read and write access to the file, it has
    // to be moved to a folder where we have access to it. First,
    // we check if the file already exists (previously copied).
    QString dataFolder = QDir::homePath();
    QString newFileName = dataFolder + "/" + fileName;
    QFile newFile(newFileName);
    newFile.open(QIODevice::ReadWrite);

    if (!newFile.exists()) {

        // If the file is not already in the data folder, we copy it from the
        // assets folder (read only) to the data folder (read and write).
        QString appFolder(QDir::homePath());
        appFolder.chop(4);
        QString originalFileName = appFolder + fileName;
        QFile originalFile(originalFileName);
        originalFile.open(QIODevice::ReadWrite);

        if (originalFile.exists()) {

            QFileInfo fileInfo(newFileName);
            QDir().mkpath(fileInfo.dir().path());

            if (!originalFile.copy(newFileName)) {
                qDebug() << "Failed to copy file to path: " << newFileName;
            }

        } else {
            qDebug() << "Failed to copy file data base file does not exists.";
        }
        qDebug() << "Copied Successfully!";
    }
}

