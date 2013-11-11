/* Copyright (c) 2013 BlackBerry Limited.
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

#include "MqttError.h"

namespace mqtt_client {

const int MqttError::PASS_CODE=0;
const int MqttError::UNKNOWN_ERROR_CODE=-1;
const int MqttError::DEFAULT_ERROR_CODE=-99;
const MqttError MqttError::PASS(PASS_CODE, "Pass");
const MqttError MqttError::UNKNOWN_ERROR(UNKNOWN_ERROR_CODE, "Unknown error");

MqttError::MqttError(int errCode, const string &desc)
	: errorCode(errCode)
	, errorDescription(desc)
{
}

MqttError::MqttError(const string &desc)
	: errorCode(MqttError::DEFAULT_ERROR_CODE)
	, errorDescription(desc)
{
}

int MqttError::getCode() const
{
	return errorCode;
}

string MqttError::getDescription() const
{
	return errorDescription;
}

void MqttError::set(const string &description, int code)
{
	errorDescription = description;
	this->errorCode = code;
}

bool MqttError::operator == (const MqttError &rhs) const
{
	return ((errorCode == rhs.errorCode) && (errorDescription.compare(rhs.errorDescription)==0));
}

bool MqttError::operator != (const MqttError &rhs) const
{
	return !(*this == rhs);
}

} /* namespace mqtt_client */
