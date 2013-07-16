#include "Crypto.hpp"

#include <QDebug>
#include <sbreturn.h>

#include "SBError.hpp"

Crypto::Crypto(const char * name) : _name(name), _lastError(SB_SUCCESS) {
}

bool Crypto::maybeLog(const char * message, int result) {
	if (result!=SB_SUCCESS) {
		qDebug() << "FAILED" << _name << message << result << SBError::getErrorText(result);
		_lastError = result;
		return false;
	}
	return true;
}

void Crypto::invalid() {
	qDebug() << "FAILED invalid params given to " << _name;
}
