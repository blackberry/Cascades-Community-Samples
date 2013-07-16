/*
 * DRBG.cpp
 *
 *  Created on: 2013-07-15
 *      Author: robwilliams
 */

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

