#include <OpServerSyncResultResp.hpp>
#include "common.hpp"
#include <QDataStream>

OpServerSyncResultResp::OpServerSyncResultResp() :
       _op_status(0)
{
    _op_type = OP_TYPE_SERVER_SYNC_RESULT_RESPONSE;
}

OpServerSyncResultResp::~OpServerSyncResultResp()
{
}

QByteArray OpServerSyncResultResp::marshall()
{
    QByteArray serialized_data;
    QDataStream stream(&serialized_data, QIODevice::WriteOnly);
    stream << opStatus();
    return serialized_data;
}

void OpServerSyncResultResp::unmarshall(QByteArray serialized_data)
{
    QDataStream stream(&serialized_data, QIODevice::ReadOnly);
    stream >> _op_status;
}

int OpServerSyncResultResp::opStatus()
{
    return _op_status;
}

void OpServerSyncResultResp::setOpStatus(int op_status)
{
    _op_status = op_status;
}
