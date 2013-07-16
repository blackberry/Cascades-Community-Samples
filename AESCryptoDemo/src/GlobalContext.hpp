/*
 * GlobalContext.h
 *
 *  Created on: 2013-07-11
 *      Author: robwilliams
 */

#ifndef GLOBALCONTEXT_H_
#define GLOBALCONTEXT_H_

#include <huctx.h>

#include "Crypto.hpp"

class GlobalContext : public Crypto {
public:
	GlobalContext();
	virtual ~GlobalContext();

	sb_GlobalCtx ctx() {
		return _ctx;
	}

	virtual bool isValid() {
		return valid && _ctx!=NULL;
	}

private:
	sb_GlobalCtx _ctx;
	bool valid;
};

#endif /* GLOBALCONTEXT_H_ */
