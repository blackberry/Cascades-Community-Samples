/*
 * AESKey.cpp
 *
 *  Created on: 2013-07-11
 *      Author: robwilliams
 */

#include <QDebug>
#include <sbreturn.h>

#include "AESKey.hpp"

AESKey::AESKey(AESParams & p, const QByteArray & content) :
		Crypto("AESKey"), _params(p), _aesKey(NULL) {
	int rc = hu_AESKeySet(_params.aesParams(), content.length() * 8,
			(unsigned char *) content.constData(), &_aesKey,
			_params.globalContext().ctx());
	maybeLog("AESKey", rc);
}

AESKey::~AESKey() {
	if (_aesKey != NULL) {
		int rc = hu_AESKeyDestroy(_params.aesParams(), &_aesKey,
				_params.globalContext().ctx());
		maybeLog("~AESKey", rc);
		_aesKey = NULL;
	}
}

