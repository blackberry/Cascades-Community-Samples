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

#ifndef OpsSqlDataSource_HPP_
#define OpsSqlDataSource_HPP_

#include <QObject>

#include <bb/data/DataSource>
#include <bb/data/SqlDataAccess>

using namespace bb::data;

class Q_DECL_EXPORT OpsSqlDataSource : public QObject {
	Q_OBJECT

public:

	virtual ~OpsSqlDataSource();

	static OpsSqlDataSource* getInstance(QObject *parent);

	void setSource(const QString source);
	QString source();

	// retrieve all waiting operations objects from the inbound queue
	QVariantList inboundQueueOps();
	// retrieve all waiting operations objects from the outbound queue
	QVariantList outboundQueueOps();

	int storeInbound(int op_type, int local_request_id, QByteArray payload);
	int storeOutbound(int op_type, int local_request_id, QByteArray payload);

	int lastInboundId();
	int lastOutboundId();

	void deleteInboundOp(int op_id);
	void deleteOutboundOp(int op_id);

	void deleteOutboundOp(int local_request_id, int op_type);

	void reset();
	void createDatabase();

	bool annualEntitelementRequExists(int leave_year);

private:

	OpsSqlDataSource(QObject *parent = 0);
	bool databaseExists();
	void copyFileToDataFolder(const QString fileName);

	static OpsSqlDataSource* _instance;
	static SqlDataAccess* _sda;
    QString _source;

};

#endif /* OpsSqlDataSource_HPP_ */
