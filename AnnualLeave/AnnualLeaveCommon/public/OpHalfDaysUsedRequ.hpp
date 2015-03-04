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

#ifndef HALF_DAYS_USED_REQU_HPP_
#define HALF_DAYS_USED_REQU_HPP_
#include "OperationAl.hpp"
#include <QString>

class Q_DECL_EXPORT OpHalfDaysUsedRequ : public OperationAl {

public:
	OpHalfDaysUsedRequ();
	~OpHalfDaysUsedRequ();

	QByteArray marshall();
	void unmarshall(QByteArray serialized_data) ;

    int localRequestId();
    void setLocalRequestId(int local_request_id);

	int halfDays();
	void setHalfDays(int half_days);

	// public so that we can easily use the >> operator when unmarshalling
    int _local_request_id;
	int _half_days;

};

#endif /* HALF_DAYS_USED_REQU_HPP_ */
