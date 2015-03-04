#include <OpClientSynchronizeRequ.hpp>
#include "common.hpp"
#include <QDataStream>

OpClientSynchronizeRequ::OpClientSynchronizeRequ() :
        _leave_year(-1)
{
    _op_type = OP_TYPE_CLIENT_SYNCHRONIZE_REQUEST;
}

OpClientSynchronizeRequ::~OpClientSynchronizeRequ()
{
}

QByteArray OpClientSynchronizeRequ::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);
    stream << leaveYear();
    return serialized_data;
}

void OpClientSynchronizeRequ::unmarshall(QByteArray serialized_data)
{
    QDataStream stream(&serialized_data, QIODevice::ReadOnly);
    stream >> _leave_year;
}

int OpClientSynchronizeRequ::leaveYear()
{
    return _leave_year;
}

void OpClientSynchronizeRequ::setLeaveYear(int year)
{
    _leave_year = year;
}
