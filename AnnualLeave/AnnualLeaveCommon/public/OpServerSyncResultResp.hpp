#ifndef SERVER_SYNC_RESULT_RESP_HPP_
#define SERVER_SYNC_RESULT_RESP_HPP_
#include "OperationAl.hpp"

class Q_DECL_EXPORT OpServerSyncResultResp : public OperationAl {

public:
    OpServerSyncResultResp();
	~OpServerSyncResultResp();

	QByteArray marshall();
	void unmarshall(QByteArray serialized_data) ;

	int opStatus();
	void setOpStatus(int op_status);

	int _op_status;

};

#endif /* SERVER_SYNC_RESULT_RESP_HPP_ */
