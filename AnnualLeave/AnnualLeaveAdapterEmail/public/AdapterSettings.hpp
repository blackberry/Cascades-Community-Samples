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

#ifndef AdapterSettings_HPP_
#define AdapterSettings_HPP_

#include <QObject>
#include "Settings.hpp"

class AdapterSettings : public QObject {
	Q_OBJECT

public:

	virtual ~AdapterSettings();

	static AdapterSettings* getInstance(QObject *parent);
    const QString accountName() const;
    void setAccountName(const QString accountName);
    const QString p2pClientUserId() const;
    void setP2pClientUserId(const QString p2pClientUserId);
    const QString p2pModeApproverEmail() const;
    void setP2pModeApproverEmail(const QString p2pModeApproverEmail);
    const QString p2pModeApproverName() const;
    void setP2pModeApproverName(const QString p2pModeApproverName);
    bool isp2pModeEnabled() const;
    void setP2pModeEnabled(bool p2pModeEnabled);
    const QString providerName() const;
    void setProviderName(const QString providerName);
    const QString serviceEmail() const;
    void setServiceEmail(const QString serviceEmail);
    const QString serviceName() const;
    void setServiceName(const QString serviceName);

    bool isConfigured();
    void setConfigured(bool b);

    QString toString();

private:

	AdapterSettings(QObject *parent = 0);
	static AdapterSettings* _instance;
    static Settings* _settings;

    void validateAdapterSettings();

    // used to select the email account that we receive messages into
    QString _accountName;

    // used to select the email account that we receive messages into
    QString _providerName;

    // email address of an "approval service" used instead of a manager's personal email address. USED IN INTEGRATION MODE
    QString _serviceEmail;

    // name of an "approval service" used instead of a manager name. USED IN INTEGRATION MODE
    QString _serviceName;

    // indicates whether or not we are operating in P2P mode. Would like to refactor so this is the only mode used by this adapter and move the other code to an
    // "email integration adapter"
    bool _p2pModeEnabled;

    // used in creation of MessageRecipient for sending emails to the approving manager. USED IN P2P MODE.
    QString _p2pModeApproverName;

    // email address of the approving manager. USED IN P2P MODE.
    QString _p2pModeApproverEmail;

    // User ID of the current application used. Can see what this might be used for in INTEGRATION MODE but don't understand purpose in P2P MODE.
    QString _p2pClientUserId;

    bool _configured;

};

#endif /* AdapterSettings_HPP_ */
