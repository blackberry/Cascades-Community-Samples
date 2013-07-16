/*
 * AESParams.h
 *
 *  Created on: 2013-07-11
 *      Author: robwilliams
 */

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
