/**
   Copyright (c) 2013 BlackBerry Limited.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**/
#include "Crypto.hpp"

#include <QDebug>
#include <sbreturn.h>

#include "SBError.hpp"

Crypto::Crypto(const char * name) : _name(name), _lastError(SB_SUCCESS) {
}

bool Crypto::maybeLog(const char * message, int result) {
	if (result!=SB_SUCCESS) {
		qDebug() << "FAILED" << _name << message << result << SBError::getErrorText(result);
		_lastError = result;
		return false;
	}
	return true;
}

void Crypto::invalid() {
	qDebug() << "FAILED invalid params given to " << _name;
}
