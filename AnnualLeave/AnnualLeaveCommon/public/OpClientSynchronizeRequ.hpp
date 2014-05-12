#ifndef CLIENT_SYNCHRONIZE_REQU_HPP_
#define CLIENT_SYNCHRONIZE_REQU_HPP_
#include "OperationAl.hpp"

class Q_DECL_EXPORT OpClientSynchronizeRequ : public OperationAl {

public:
    OpClientSynchronizeRequ();
	~OpClientSynchronizeRequ();

	QByteArray marshall();
	void unmarshall(QByteArray serialized_data) ;

	int leaveYear();
	void setLeaveYear(int year);

	int _leave_year;

};

#endif /* CLIENT_SYNCHRONIZE_REQU_HPP_ */
