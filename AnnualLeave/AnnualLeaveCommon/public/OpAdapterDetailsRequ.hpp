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

#ifndef ADAPTER_DETAILS_REQU_HPP_
#define ADAPTER_DETAILS_REQU_HPP_
#include "OperationAl.hpp"

class Q_DECL_EXPORT OpAdapterDetailsRequ : public OperationAl {

public:
    OpAdapterDetailsRequ();
	~OpAdapterDetailsRequ();

	QByteArray marshall();
	void unmarshall(QByteArray serialized_data) ;

    int opStatus();
    void setOpStatus(int op_status);

	QString adapterName();
	void setAdapterName(QString name);

    QString adapterVersion();
    void setAdapterVersion(QString version);

    QString adapterDescription();
    void setAdapterDescription(QString description);

    bool isConfigured();
    void setConfigured(bool b);

    int _op_status;
    QString _adapter_name;
    QString _adapter_version;
    QString _adapter_description;
    bool _is_configured;

};

#endif /* ADAPTER_DETAILS_REQU_HPP_ */
