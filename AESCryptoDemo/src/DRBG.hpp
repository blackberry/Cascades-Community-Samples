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
#ifndef DRBG_H_
#define DRBG_H_

#include <QByteArray>
#include <hurandom.h>

#include "GlobalContext.hpp"

class DRBG : public Crypto {
public:
	DRBG(GlobalContext & context);
	virtual ~DRBG();

	int getBytes(QByteArray & buffer);

	virtual bool isValid() {
		return rngCtx!=NULL;
	}

private:
	GlobalContext & context;
	sb_RNGCtx rngCtx;
};

#endif /* DRBG_H_ */
