/*
 * SBError.hpp
 *
 *  Created on: 2013-07-15
 *      Author: robwilliams
 */

#ifndef SBERROR_HPP_
#define SBERROR_HPP_

#include <QMap>
#include <QString>

class SBError {
public:
	static QString getErrorText(int error);

private:
	SBError() {}
	virtual ~SBError() {}
};

#endif /* SBERROR_HPP_ */
