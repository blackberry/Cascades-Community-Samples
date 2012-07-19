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
#ifndef NDEFTYPE_H
#define NDEFTYPE_H

#include <QtCore/QDebug>
#include <nfc/nfc_types.h>

class NdefType {

private:
	tnf_type_t _TNF;
	char* _type;

public:
	NdefType(tnf_type_t tnf, const char* type);
	tnf_type_t getTNF();
	char* getType();

};

#endif // ifndef NDEFTYPE_H
