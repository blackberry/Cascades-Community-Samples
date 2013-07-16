/*
 * AESParams.cpp
 *
 *  Created on: 2013-07-11
 *      Author: robwilliams
 */

#include <QDebug>

#include "AESParams.hpp"

AESParams::AESParams(GlobalContext & g) :
		Crypto("AESParams"), _aesParams(NULL), _globalContext(g) {
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

