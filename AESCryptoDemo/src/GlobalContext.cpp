/*
 * GlobalContext.cpp
 *
 *  Created on: 2013-07-11
 *      Author: robwilliams
 */

#include <QDebug>
#include <sbreturn.h>
#include <hugse56.h>
#include <huseed.h>

#include "GlobalContext.hpp"

bool falseFunc() {
	return false;
}

GlobalContext::GlobalContext() :
		Crypto("GlobalContext"), _ctx(NULL), valid(false) {
	valid = maybeLog("Create", hu_GlobalCtxCreateDefault(&_ctx))
			&& maybeLog("Register", hu_RegisterSbg56(_ctx))
			&& maybeLog("Register Seed", hu_RegisterSystemSeed(_ctx))
			&& maybeLog("Init SBG 56", hu_InitSbg56(_ctx));
}

GlobalContext::~GlobalContext() {
	if (_ctx != NULL) {
		int rc = hu_GlobalCtxDestroy(&_ctx);
		maybeLog("GlobalCtxDestroy", rc);
		_ctx = NULL;
	}
}

