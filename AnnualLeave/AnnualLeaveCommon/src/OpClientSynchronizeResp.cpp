#include <OpClientSynchronizeResp.hpp>
#include "common.hpp"
#include <QDataStream>

OpClientSynchronizeResp::OpClientSynchronizeResp() :
       _op_status(0)
{
    _op_type = OP_TYPE_CLIENT_SYNCHRONIZE_RESPONSE;
}

OpClientSynchronizeResp::~OpClientSynchronizeResp()
{
}

QByteArray OpClientSynchronizeResp::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);
    stream << opStatus();
    return serialized_data;
}

void OpClientSynchronizeResp::unmarshall(QByteArray serialized_data)
{
    QDataStream stream(&serialized_data, QIODevice::ReadOnly);
    stream >> _op_status;
}

int OpClientSynchronizeResp::opStatus()
{
    return _op_status;
}

void OpClientSynchronizeResp::setOpStatus(int op_status)
{
    _op_status = op_status;
}
