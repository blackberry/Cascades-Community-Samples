/* Copyright (c) 2012 Research In Motion Limited.
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
#include "NdefType.hpp"

NdefType::NdefType(tnf_type_t tnf, const char* type) {
	qDebug() << "Constructing NdefType";
	_TNF = tnf;
	_type = const_cast<char *>(type);
	qDebug() << "Done constructing NdefType";
}

tnf_type_t NdefType::getTNF() {
	return _TNF;
}

char* NdefType::getType() {
	return _type;
}
