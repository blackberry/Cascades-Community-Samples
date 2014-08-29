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

#include "AdapterSettings.hpp"

AdapterSettings* AdapterSettings::_instance;
Settings* AdapterSettings::_settings;

AdapterSettings::AdapterSettings(QObject *parent) :
        QObject(parent)
{
}

AdapterSettings::~AdapterSettings()
{
}

AdapterSettings* AdapterSettings::getInstance(QObject *parent)
{
    if (_instance == 0) {
        _instance = new AdapterSettings(parent);
    }
    return _instance;
}

const QString AdapterSettings::accountName() const
{
    return _accountName;
}

void AdapterSettings::setAccountName(const QString accountName)
{
    _accountName = accountName;
    validateAdapterSettings();
}

const QString AdapterSettings::p2pClientUserId() const
{
    return _p2pClientUserId;
}

void AdapterSettings::setP2pClientUserId(const QString p2pClientUserId)
{
    _p2pClientUserId = p2pClientUserId;
    validateAdapterSettings();
}

const QString AdapterSettings::p2pModeApproverEmail() const
{
    return _p2pModeApproverEmail;
}

void AdapterSettings::setP2pModeApproverEmail(const QString p2pModeApproverEmail)
{
    _p2pModeApproverEmail = p2pModeApproverEmail;
    validateAdapterSettings();
}

const QString AdapterSettings::p2pModeApproverName() const
{
    return _p2pModeApproverName;
}

void AdapterSettings::setP2pModeApproverName(const QString p2pModeApproverName)
{
    _p2pModeApproverName = p2pModeApproverName;
    validateAdapterSettings();
}

bool AdapterSettings::isp2pModeEnabled() const
{
    return _p2pModeEnabled;
}

void AdapterSettings::setP2pModeEnabled(bool p2pModeEnabled)
{
    _p2pModeEnabled = p2pModeEnabled;
    validateAdapterSettings();
}

const QString AdapterSettings::providerName() const
{
    return _providerName;
}

void AdapterSettings::setProviderName(const QString providerName)
{
    _providerName = providerName;
    validateAdapterSettings();
}

const QString AdapterSettings::serviceEmail() const
{
    return _serviceEmail;
}

void AdapterSettings::setServiceEmail(const QString serviceEmail)
{
    _serviceEmail = serviceEmail;
    validateAdapterSettings();
}

const QString AdapterSettings::serviceName() const
{
    return _serviceName;
}

void AdapterSettings::setServiceName(const QString serviceName)
{
    _serviceName = serviceName;
    validateAdapterSettings();
}

bool AdapterSettings::isConfigured()
{
    return _configured;
}

void AdapterSettings::setConfigured(bool b)
{
    _configured = b;
    int configured_value=0;
    if (_configured) {
        configured_value = 1;
    }
    _settings = Settings::getInstance(this);

    bool persisted_configured=false;
    QString configured_setting = _settings->getValueFor(SETTINGS_KEY_ADAPTER_CONFIGURED, "0");
    (configured_setting.compare("1") == 0) ? persisted_configured = true : persisted_configured = false;

    if (persisted_configured != _configured) {
        qDebug() << "EEEE saving modified adapter configured setting:" << configured_value;
        _settings->saveValueFor(SETTINGS_KEY_ADAPTER_CONFIGURED, QString::number(configured_value));
    } else {
        qDebug() << "EEEE adapter configured setting has not changed";
    }
}

QString AdapterSettings::toString()
{

    return QString("accountName=%1 p2pClientUserId=%2 p2pModeApproverEmail=%3 p2pModeApproverName=%4 isp2pModeEnabled=%5 providerName=%6 serviceEmail=%7 serviceName=%8").arg(_accountName).arg(
            _p2pClientUserId).arg(_p2pModeApproverEmail).arg(_p2pModeApproverName).arg(_p2pModeEnabled).arg(_providerName).arg(_serviceEmail).arg(_serviceName);

}

void AdapterSettings::validateAdapterSettings()
{

    // validating for P2P use only in anticipation of INTEGRATION MODE being moved to a different adapter

    if (accountName().length() > 0 && providerName().length() > 0 && p2pModeApproverName().length() > 0 && p2pModeApproverEmail().length() > 0 && p2pClientUserId().length() > 0) {
        qDebug() << "EEEE adapter has values for all required settings";
        setConfigured(true);
    } else {
        qDebug() << "EEEE adapter does not have values for all required settings";
        setConfigured(false);
    }

}
