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

#ifndef ANNUAL_ENTITLEMENT_REQU_HPP_
#define ANNUAL_ENTITLEMENT_REQU_HPP_
#include "OperationAl.hpp"
#include <QString>

class Q_DECL_EXPORT OpAnnualEntitlementRequ : public OperationAl {

public:
	OpAnnualEntitlementRequ();
	~OpAnnualEntitlementRequ();

	QByteArray marshall();
	void unmarshall(QByteArray serialized_data) ;

	int leaveYear();
	void setLeaveYear(int year);

	int _leave_year;

};

#endif /* ANNUAL_ENTITLEMENT_REQU_HPP_ */
