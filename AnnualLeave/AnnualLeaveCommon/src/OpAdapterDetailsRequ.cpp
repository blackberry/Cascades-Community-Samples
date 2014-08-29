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

#include <OpAdapterDetailsRequ.hpp>
#include "common.hpp"
#include <QDataStream>

OpAdapterDetailsRequ::OpAdapterDetailsRequ() :
       _op_status(-1), _adapter_name(""), _adapter_version(""), _adapter_description(""), _is_configured(false)
{
    _op_type = OP_TYPE_ADAPTER_DETAILS_REQUEST;
}

OpAdapterDetailsRequ::~OpAdapterDetailsRequ()
{
}

QByteArray OpAdapterDetailsRequ::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);
    stream << opStatus();
    stream << adapterName();
    stream << adapterVersion();
    stream << adapterDescription();
    stream << isConfigured();
    return serialized_data;
}

void OpAdapterDetailsRequ::unmarshall(QByteArray serialized_data)
{
    QDataStream stream(&serialized_data, QIODevice::ReadOnly);
    stream >> _op_status >> _adapter_name >> _adapter_version >> _adapter_description >> _is_configured;
}

int OpAdapterDetailsRequ::opStatus()
{
    return _op_status;
}

void OpAdapterDetailsRequ::setOpStatus(int op_status)
{
    _op_status = op_status;
}

QString OpAdapterDetailsRequ::adapterName() {
    return _adapter_name;
}

void OpAdapterDetailsRequ::setAdapterName(QString name) {
    _adapter_name = name;
}

QString OpAdapterDetailsRequ::adapterVersion() {
    return _adapter_version;
}

void OpAdapterDetailsRequ::setAdapterVersion(QString version) {
    _adapter_version = version;
}

QString OpAdapterDetailsRequ::adapterDescription() {
    return _adapter_description;
}

void OpAdapterDetailsRequ::setAdapterDescription(QString desc) {
    _adapter_description = desc;
}

bool OpAdapterDetailsRequ::isConfigured() {
    return _is_configured;
}

void OpAdapterDetailsRequ::setConfigured(bool b) {
    _is_configured = b;
}
