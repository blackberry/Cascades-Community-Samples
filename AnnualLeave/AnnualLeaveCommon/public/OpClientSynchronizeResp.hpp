#ifndef CLIENT_SYNCHRONIZE_RESP_HPP_
#define CLIENT_SYNCHRONIZE_RESP_HPP_
#include "OperationAl.hpp"

class Q_DECL_EXPORT OpClientSynchronizeResp : public OperationAl {

public:
	OpClientSynchronizeResp();
	~OpClientSynchronizeResp();

	QByteArray marshall();
	void unmarshall(QByteArray serialized_data) ;

	int opStatus();
	void setOpStatus(int op_status);

	int _op_status;

};

#endif /* CLIENT_SYNCHRONIZE_RESP_HPP_ */
