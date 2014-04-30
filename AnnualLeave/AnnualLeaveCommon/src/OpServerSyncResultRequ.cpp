#include <OpServerSyncResultRequ.hpp>
#include "common.hpp"
#include <QDataStream>

OpServerSyncResultRequ::OpServerSyncResultRequ() :
    _bookings(new BookingList())
{
    _op_type = OP_TYPE_SERVER_SYNC_RESULT_REQUEST;
}

OpServerSyncResultRequ::~OpServerSyncResultRequ()
{
}

QByteArray OpServerSyncResultRequ::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);
    stream << _bookings->marshall();
    return serialized_data;
}

void OpServerSyncResultRequ::unmarshall(QByteArray serialized_data)
{
    _bookings->unmarshall(serialized_data);
}

BookingList* OpServerSyncResultRequ::bookings() {
    return _bookings;
}

void OpServerSyncResultRequ::setBookings(BookingList* bookings) {
    _bookings = bookings;
}
