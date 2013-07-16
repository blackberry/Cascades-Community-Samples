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

#include "DRBG.hpp"

DRBG::DRBG(GlobalContext & gc) :
		Crypto("DRBG"), context(gc), rngCtx(NULL) {
	int rc = hu_RngDrbgCreate(HU_DRBG_HASH, 112, false, 0, NULL, NULL, &rngCtx,
			context.ctx());
	maybeLog("DRBGCreate", rc);
}

DRBG::~DRBG() {
	if (rngCtx != NULL) {
		int rc = hu_RngDrbgDestroy(&rngCtx, context.ctx());
		maybeLog("DRBGDestroy", rc);
		rngCtx = NULL;
	}
}

int DRBG::getBytes(QByteArray & buffer) {
	if (isValid()) {
		int rc = hu_RngGetBytes(rngCtx, buffer.length(),
				(unsigned char *) buffer.data(), context.ctx());
		maybeLog("RNGGetBytes", rc);
		return rc;
	}
	return -1;
}

