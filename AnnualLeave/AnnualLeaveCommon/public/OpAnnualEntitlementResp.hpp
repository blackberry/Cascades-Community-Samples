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

#ifndef ANNUAL_ENTITLEMENT_RESP_HPP_
#define ANNUAL_ENTITLEMENT_RESP_HPP_
#include "OperationAl.hpp"

class Q_DECL_EXPORT OpAnnualEntitlementResp : public OperationAl {

public:
	OpAnnualEntitlementResp();
	~OpAnnualEntitlementResp();

	QByteArray marshall();
	void unmarshall(QByteArray serialized_data) ;

	int opStatus();
	void setOpStatus(int op_status);

    int requestStatus();
    void setRequestStatus(int status);

    int leaveYear();
    void setLeaveYear(int year);

    int totalEntitlementHalfDays();
	void setTotalEntitlementHalfDays(int half_days);

    int currentEntitlementHalfDays();
    void setCurrentEntitlementHalfDays(int half_days);

	int _op_status;
    int _request_status;
    int _leave_year;
	int _total_entitlement_half_days;
    int _current_entitlement_half_days;

};

#endif /* ANNUAL_ENTITLEMENT_RESP_HPP_ */
