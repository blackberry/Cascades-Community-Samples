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

#ifndef OUTBOUNDQUEUEMANAGER_HPP_
#define OUTBOUNDQUEUEMANAGER_HPP_

#include <Common.hpp>
#include <AdapterImpl.hpp>
#include <Operations.hpp>
#include <Settings.hpp>

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <errno.h>

class OutboundQueueManager: public QObject
{
Q_OBJECT

public:
    virtual ~OutboundQueueManager();
    static OutboundQueueManager *getInstance(QObject *parent = NULL);
    int start();
    int stop();
    int pause();
    int resume();
    int processQueue();

private slots:
    void onSettingsChanged();

private:
    OutboundQueueManager(QObject *parent = NULL);
    static OutboundQueueManager* _instance;
    bool _initialised;
    bool _paused;
    AdapterImpl *_adapterImpl;
    IAdapter *_iAdapter;
    Operations *_ops;
    bool _dumpQueueEnabled;
    Settings *_settings;

    int doOutSubmitBookingRequ(OpSubmitBookingRequ *request);
    int doOutHalfDayResp(OpHalfDaysUsedResp *request);
    int doOutApprovalOutcomeResp(OpApprovalOutcomeResp *response);
    int doOutCancelBookingRequ(OpCancelBookingRequ *request);
    int doOutEntitlementRequ(OpAnnualEntitlementRequ *request);
    int doOutUpdateBookingRequ(OpUpdateBookingRequ *request);
    int doOutApprovalTaskResp(OpApprovalTaskResp *response);
    int doOutApprovalTaskOutcomeRequ(OpApprovalTaskOutcomeRequ *request);
    int doOutAdapterDetailsResp(OpAdapterDetailsResp *response);
    int doOutAdapterStatusRequ(OpAdapterStatusRequ *request);
    int doOutClientSynchronizeRequ(OpClientSynchronizeRequ *request);
    int doOutServerSyncResultResp(OpServerSyncResultResp *response);

    void failBookingRequ(int LocalId);
    void failCancelRequ(int LocalId);
    void failUpdateRequ(int LocalId);
    void failEntitlementRequ();
    void removeRequest(int opId);

    void dumpQueue();
};

#endif /* OUTBOUNDQUEUEMANAGER_HPP_ */
