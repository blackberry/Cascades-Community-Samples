/*
 * DRBG.h
 *
 *  Created on: 2013-07-15
 *      Author: robwilliams
 */

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
