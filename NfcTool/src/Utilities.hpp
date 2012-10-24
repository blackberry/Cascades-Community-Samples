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
#ifndef Utilities_h
#define Utilities_h

#include "stdint.h"
#include <QChar>

class Utilities {

public:

	static uint8_t hexToInt(QChar c1, QChar c2);
	static void hexToIntArray(QString hex_string,uint8_t* output);
	static bool isValidHex(QString hex_string);

private:
	static const QString HEX_CHARS;

};

#endif // ifndef Utilities_h
