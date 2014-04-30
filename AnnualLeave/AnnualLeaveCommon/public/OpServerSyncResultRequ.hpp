#ifndef SERVER_SYNC_RESULT_REQU_HPP_
#define SERVER_SYNC_RESULT_REQU_HPP_
#include "OperationAl.hpp"
#include "BookingList.hpp"

class Q_DECL_EXPORT OpServerSyncResultRequ : public OperationAl {

public:
    OpServerSyncResultRequ();
	~OpServerSyncResultRequ();

	QByteArray marshall();
	void unmarshall(QByteArray serialized_data) ;

	BookingList* bookings();
	void setBookings(BookingList* bookings);

	BookingList *_bookings;

};

#endif /* SERVER_SYNC_RESULT_REQU_HPP_ */
