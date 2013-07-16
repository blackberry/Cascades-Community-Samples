/*
 * Crypto.hpp
 *
 *  Created on: 2013-07-15
 *      Author: robwilliams
 */

#ifndef CRYPTO_HPP_
#define CRYPTO_HPP_

#include <QString>

class Crypto {
public:
	virtual ~Crypto() {}

	virtual bool isValid() = 0;

	int lastError() {
		return _lastError;
	}

protected:
	Crypto(const char * name);
	bool maybeLog(const char * message, int result);
	void invalid();

private:
	QString _name;
	int _lastError;
};

#endif /* CRYPTO_HPP_ */
