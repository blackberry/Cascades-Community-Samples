/*
 * AESKey.h
 *
 *  Created on: 2013-07-11
 *      Author: robwilliams
 */

#ifndef AESKEY_H_
#define AESKEY_H_

#include <QByteArray>
#include "huaes.h"

#include "AESParams.hpp"

class AESKey : public Crypto {
public:
	AESKey(AESParams & params, const QByteArray & value);
	virtual ~AESKey();

	sb_Key aesKey() {
		return _aesKey;
	}

	AESParams & params() {
		return _params;
	}

	virtual bool isValid() {
		return _aesKey!=NULL;
	}

private:
	AESParams & _params;
	sb_Key _aesKey;
};

#endif /* AESKEY_H_ */
