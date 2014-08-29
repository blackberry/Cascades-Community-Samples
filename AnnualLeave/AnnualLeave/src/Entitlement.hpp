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

#ifndef ENTITLEMENT_HPP_
#define ENTITLEMENT_HPP_

#include <Entitlement.hpp>

class Entitlement {

public:
	Entitlement();
	~Entitlement();

	int leaveYear();
	void setLeaveYear(int year);

    int totalEntitlementHalfDays();
    void setTotalEntitlementHalfDays(int half_days);

    int currentEntitlementHalfDays();
    void setCurrentEntitlementHalfDays(int half_days);

	int _leave_year;
    int _total_entitlement_half_days;
    int _current_entitlement_half_days;

};

#endif /* ENTITLEMENT_HPP_ */
