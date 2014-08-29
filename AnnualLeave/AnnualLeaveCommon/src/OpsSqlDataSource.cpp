/*
 * Copyright (c) 2014 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "OpsSqlDataSource.hpp"
#include "common.hpp"
#include "OpAnnualEntitlementRequ.hpp"
#include <QFile>
#include <QDir>

#include <bb/cascades/Application>

#include <QtSql/QtSql>

using namespace bb::cascades;
using namespace bb::data;

OpsSqlDataSource* OpsSqlDataSource::_instance;
SqlDataAccess* OpsSqlDataSource::_sda;

OpsSqlDataSource::OpsSqlDataSource(QObject *parent) :
        QObject(parent)
{

    qDebug() << "OOOO  constructing OpsSqlDataSource";

    QDir home = QDir::home();
    _source = home.absoluteFilePath(OPS_QUEUE_DATABASE);

    _sda = new SqlDataAccess(_source);

    qDebug() << "OOOO does the database file already exist at " << _source;
    if (!databaseExists()) {
        qDebug() << "OOOO no, so let's create it";
        createDatabase();
    } else {
        qDebug() << "OOOO database already exists";
    }
    if (!_sda->connection().isOpen()) {
        bool ok = _sda->connection().open();
        qDebug() << "OOOO Ops database opened " << ok;
        if (!ok) {
            const QSqlError error = _sda->connection().lastError();
            qDebug() << "OOOO result=" << error;
            return;
        }
    } else {
        qDebug() << "OOOO database is already open";
    }

}

OpsSqlDataSource::~OpsSqlDataSource()
{
    delete _sda;
}

OpsSqlDataSource* OpsSqlDataSource::getInstance(QObject *parent)
{
    if (_instance == 0) {
        _instance = new OpsSqlDataSource(parent);
    }
    return _instance;
}

bool OpsSqlDataSource::databaseExists()
{
    QDir home = QDir::home();
    QString filePath = home.absoluteFilePath(OPS_QUEUE_DATABASE);
    qDebug() << "OOOO databaseExists: " << filePath;
    QFile file(filePath);
    return file.exists();
}

void OpsSqlDataSource::createDatabase()
{

    qDebug() << "OOOO creating ops queue database";

    if (_sda != 0) {
        qDebug() << "OOOO closing old database connection";
        _sda->connection().close();
    }
    QDir home = QDir::home();
    QString filePath = _source;

    bool deleted = QFile::remove(filePath);

    qDebug() << "OOOO database file deleted=" << deleted;

    QFile file(filePath);

    qDebug() << "OOOO setting database name=" << filePath;
    _sda->connection().setDatabaseName(filePath);

    bool database_opened = _sda->connection().open();
    qDebug() << "OOOO createDatabase(): database opened " << database_opened;

    if (database_opened) {

        if (file.open(QIODevice::ReadWrite)) {

            // Note:
            // not all operations relate to an annual leave request and therefore do not have a local_request_id
            // for those that do, however, local_request_id is included in the inbound_op_queue and outbound_op_queue tables as a foreign key and, most
            // importantly, to allow selection of operations objects from the queues by local_request_id, which is useful in some scenarios
            //
            // Typically a value of -1 is used to indicate "no local request ID"

            qDebug() << "OOOO creating table inbound_ops_queue";
            _sda->execute("CREATE TABLE inbound_ops_queue( in_op_id INTEGER PRIMARY KEY AUTOINCREMENT, op_type integer, local_request_id integer, payload blob );");
            const QSqlError error = _sda->connection().lastError();
            qDebug() << "OOOO result=" << error;

            qDebug() << "OOOO creating table outbound_ops_queue";
            _sda->execute("CREATE TABLE outbound_ops_queue( out_op_id INTEGER PRIMARY KEY AUTOINCREMENT, op_type integer, local_request_id integer, payload blob );");
            const QSqlError error2 = _sda->connection().lastError();
            qDebug() << "OOOO result=" << error2;

        } else {
            const QSqlError error = _sda->connection().lastError();
            qDebug() << "OOOO failed to create _sda->connection(). error=" << error;
        }
    } else {
        const QSqlError error = _sda->connection().lastError();
        qDebug() << "OOOO Don't have read/write access to db file. error=" << error;

    }
}

void OpsSqlDataSource::reset()
{
    qDebug() << "OOOO reset is recreating the database";
    createDatabase();
}

QVariantList OpsSqlDataSource::inboundQueueOps()
{

    qDebug() << "OOOO OpsSqlDataSource::inboundQueueOps";
    QVariant result = _sda->execute("select * from inbound_ops_queue");
    QVariantList records = result.value<QVariantList>();
    return records;
}

QVariantList OpsSqlDataSource::outboundQueueOps()
{
    QVariant result = _sda->execute("select * from outbound_ops_queue");
    QVariantList records = result.value<QVariantList>();
    return records;
}

int OpsSqlDataSource::storeInbound(int op_type, int local_request_id, QByteArray payload)
{
    qDebug() << QString("OOOO storeInbound: op_type=%1 local_request_id=%2").arg(op_type).arg(local_request_id);
    qDebug() << QString("OOOO storeInbound: payload length=%1").arg(payload.size());

    QString hex_payload(payload.toHex());

    QString insert_sql("INSERT INTO inbound_ops_queue(op_type, local_request_id, payload) VALUES(%1,%2,X'%3')");
    insert_sql = insert_sql.arg(op_type).arg(local_request_id).arg(hex_payload);
    qDebug() << "OOOO storeInbound:" << insert_sql;
    _sda->execute(insert_sql);

    int id = lastInboundId();

    return id;

}

int OpsSqlDataSource::storeOutbound(int op_type, int local_request_id, QByteArray payload)
{
    qDebug() << QString("OOOO storeOutbound: op_type=%1 id=%2").arg(op_type).arg(local_request_id);
    qDebug() << QString("OOOO storeOutbound: payload length=%1").arg(payload.size());

    QString hex_payload(payload.toHex());

    QString insert_sql("INSERT INTO outbound_ops_queue(op_type, local_request_id, payload) VALUES(%1,%2,X'%3')");
    insert_sql = insert_sql.arg(op_type).arg(local_request_id).arg(hex_payload);
    qDebug() << "OOOO storeOutbound:" << insert_sql;
    _sda->execute(insert_sql);

    const QSqlError error = _sda->connection().lastError();
    qDebug() << "OOOO result=" << error;

    int id = lastOutboundId();

    return id;

}

bool OpsSqlDataSource::annualEntitelementRequExists(int leave_year)
{
    QVariant result = _sda->execute(QString("select payload from outbound_ops_queue where op_type=%1").arg(OP_TYPE_ANNUAL_ENTITLEMENT_REQUEST));
    QVariantList sql_rows = result.value<QVariantList>();
    QVariantList::iterator j;
    for (j = sql_rows.begin(); j != sql_rows.end(); j++) {
        QMap<QString, QVariant> map = (*j).toMap();
        QByteArray payload = map["payload"].toByteArray();
        OpAnnualEntitlementRequ* entitlement_request;
        entitlement_request = new OpAnnualEntitlementRequ();
        entitlement_request->unmarshall(payload);
        if (entitlement_request->leaveYear() == leave_year) {
            return true;
        }
    }
    return false;
}

int OpsSqlDataSource::lastInboundId()
{
    int op_id = -1;
    QVariant result = _sda->execute("select max(in_op_id) from inbound_ops_queue");
    QVariantList list = result.value<QVariantList>();
    QVariant row = list.first();
    op_id = row.toMap().value("max(in_op_id)").toInt();
    return op_id;
}

int OpsSqlDataSource::lastOutboundId()
{
    int op_id = -1;
    QVariant result = _sda->execute("select max(out_op_id) from outbound_ops_queue");
    QVariantList list = result.value<QVariantList>();
    QVariant row = list.first();
    op_id = row.toMap().value("max(out_op_id)").toInt();
    return op_id;
}

void OpsSqlDataSource::deleteInboundOp(int op_id)
{
    qDebug() << QString("OOOO deleteInboundOp: in_op_id=%1").arg(op_id);

    QString delete_sql("DELETE from inbound_ops_queue WHERE in_op_id=%1");
    delete_sql = delete_sql.arg(op_id);
    qDebug() << "OOOO deleteInboundOp:" << delete_sql;
    _sda->execute(delete_sql);

    const QSqlError error = _sda->connection().lastError();
    qDebug() << "OOOO result=" << error;

}

void OpsSqlDataSource::deleteOutboundOp(int op_id)
{
    qDebug() << QString("OOOO deleteOutboundOp: out_op_id=%1").arg(op_id);

    QString delete_sql("DELETE from outbound_ops_queue WHERE out_op_id=%1");
    delete_sql = delete_sql.arg(op_id);
    qDebug() << "OOOO deleteOutboundOp:" << delete_sql;
    _sda->execute(delete_sql);

    const QSqlError error = _sda->connection().lastError();
    qDebug() << "OOOO result=" << error;

}

void OpsSqlDataSource::deleteOutboundOp(int local_request_id, int op_type)
{

    qDebug() << QString("OOOO deleteOutboundOp: local_request_id=%1 op_type=%2").arg(local_request_id).arg(op_type);

    QString delete_sql("DELETE from outbound_ops_queue WHERE local_request_id=%1 AND op_type=%2");
    delete_sql = delete_sql.arg(local_request_id).arg(op_type);
    qDebug() << "OOOO deleteOutboundOp:" << delete_sql;
    _sda->execute(delete_sql);

    const QSqlError error = _sda->connection().lastError();
    qDebug() << "OOOO result=" << error;
}

void OpsSqlDataSource::copyFileToDataFolder(const QString fileName)
{
    QString dataFolder = QDir::homePath();
    QString newFileName = dataFolder + "/" + fileName;
    QFile newFile(newFileName);
    newFile.open(QIODevice::ReadWrite);

    if (!newFile.exists()) {

        QString appFolder(QDir::homePath());
        appFolder.chop(4);
        QString originalFileName = appFolder + fileName;
        QFile originalFile(originalFileName);
        originalFile.open(QIODevice::ReadWrite);

        if (originalFile.exists()) {

            QFileInfo fileInfo(newFileName);
            QDir().mkpath(fileInfo.dir().path());

            if (!originalFile.copy(newFileName)) {
                qDebug() << "Failed to copy file to path: " << newFileName;
            }

        } else {
            qDebug() << "Failed to copy file data base file does not exists.";
        }
        qDebug() << "Copied Successfully!";
    }
}

void OpsSqlDataSource::setSource(const QString source)
{
    if (_source.compare(source) != 0) {
        // We copy the database file to the data folder so that we have both read and write access.
        copyFileToDataFolder(source);

        QDir home = QDir::home();
        QString filePath = home.absoluteFilePath("al.db");
        _source = filePath;

    }
}

QString OpsSqlDataSource::source()
{
    return _source;
}
