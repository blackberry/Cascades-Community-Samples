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
#ifndef AESPARAMS_H_
#define AESPARAMS_H_

#include <huaes.h>

#include "GlobalContext.hpp"

class AESParams : public Crypto {
public:
	AESParams(GlobalContext & gc);
	virtual ~AESParams();

	sb_Params aesParams() {
		return _aesParams;
	}

	GlobalContext & globalContext() {
		return _globalContext;
	}

	virtual bool isValid() {
		return _aesParams!=NULL;
	}

private:
	GlobalContext & _globalContext;
	sb_Params _aesParams;
};

#endif /* AESPARAMS_H_ */
