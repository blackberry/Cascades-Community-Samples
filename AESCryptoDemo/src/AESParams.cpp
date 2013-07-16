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
#include <QDebug>

#include "AESParams.hpp"

AESParams::AESParams(GlobalContext & g) :
		Crypto("AESParams"), _globalContext(g), _aesParams(NULL) {
	int rc = hu_AESParamsCreate(SB_AES_CBC, SB_AES_128_BLOCK_BITS, NULL, NULL,
			&_aesParams, _globalContext.ctx());
	maybeLog("AESParamsCreate", rc);
}

AESParams::~AESParams() {
	if (_aesParams != NULL) {
		// cowardly ignoring return code.
		int rc = hu_AESParamsDestroy(&_aesParams, _globalContext.ctx());
		maybeLog("AESParamsDestroy", rc);
		_aesParams = NULL;
	}
}

