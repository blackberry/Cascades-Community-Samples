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

#ifndef UserSqlDataSource_HPP_
#define UserSqlDataSource_HPP_

#include "Entitlement.hpp"
#include "DbAnnualLeaveRecord.hpp"

#include <QObject>
#include <bb/data/DataSource>
#include <bb/data/SqlDataAccess>
#include <bb/system/SystemUiResult>

using namespace bb::data;

class UserSqlDataSource: public QObject
{
    Q_OBJECT

public:

    virtual ~UserSqlDataSource();

    static UserSqlDataSource* getInstance(QObject *parent);

    Q_INVOKABLE
    void loadRequestData(QString leave_year);Q_INVOKABLE
    void loadTaskData();Q_INVOKABLE
    int storeRequest(int fdate, int tdate, QString note, int leave_year, bool first_day_half, bool last_day_half, int day_half, int half_days);Q_INVOKABLE
    void updateRequestStatus(int local_request_id, int request_status);Q_INVOKABLE
    void updateHalfDays(int local_request_id, int half_days);Q_INVOKABLE
    void deleteRequest(int local_request_id);Q_INVOKABLE
    void deleteApprovalTask(int task_id);Q_INVOKABLE
    void reset();Q_INVOKABLE
    void updateEntitlement(int leave_year, int total_entitlement_half_days, int current_entitlement_half_days);Q_INVOKABLE
    Entitlement getEntitlement(int leave_year);

    void createDatabase();
    int lastAlId();
    int lastTaskId();
    void cancelRequest(int local_request_id);
    int getCurrentStatus(int local_request_id);
    int getLastOpStatus(int local_request_id);
    void updateRequest(int local_request_id, int originating_op_id, int current_leave_year, int updating_leave_year, int updating_from_date_s, int updating_to_date_s, bool updating_first_day_half,
            bool updating_last_day_half, int updating_day_half, QString note);
    void updateLastOpStatus(int local_request_id, int op_status);
    void setOriginatingOpId(int local_request_id, int originating_op_id);
    void applyBookingUpdate(int local_request_id, int half_days);
    int countBookingUpdates(int local_request_id, int originating_op_id);
    void clearProcessingStatus(int local_request_id, int leave_year);
    int storeApprovalTask(QString user_id, int task_type, int fdate, int tdate, int leave_year, bool first_day_half, bool last_day_half, int day_half, int half_days, int local_request_id,
            QString note, int updating_fdate, int updating_tdate, int updating_leave_year, bool updating_first_day_half, bool updating_last_day_half, int updating_day_half, int updating_half_days, int originating_op_id);
    void updateApprovalTask(int taskid, int decision);
    void updateApprovalTaskProcessingStatus(int task_id, int task_status);
    int countTasks();
    DbAnnualLeaveRecord getAnnualLeaveRequest(int local_request_id);
    int getApprovalTaskYear(int task_id);
    int bookingCount(int fdate, int tdate, bool first_day_half, bool last_day_half, int day_half);
    QVariant getRequests(int leave_year);
    int deleteRequest(qint64 from_date_s, qint64 to_date_s, bool first_day_half, bool last_day_half, int day_half);

    signals:
    void signalLoadRequestData(const QString&);
    void signalLoadTaskData();
    void sourceChanged(QString source);
    void queryChanged(QString query);
    void signalRequestsLoaded(const QVariantList &data);
    void signalTasksLoaded(const QVariantList &data);

private slots:
    void onLoadRequestData(const QString& sql);
    void onLoadTaskData();

private:

    UserSqlDataSource(QObject *parent = 0);
    bool databaseExists();
    void copyFileToDataFolder(const QString fileName);

    static UserSqlDataSource* _instance;
    static SqlDataAccess* _sda;
    QString _source;
    QString _query;
    int _local_request_id;
    int _leave_year;
    QVariantList _requests_model_list;
    QVariantList _tasks_model_list;
};

#endif /* UserSqlDataSource_HPP_ */
