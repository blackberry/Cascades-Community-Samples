/*
 * mysqldatasource.hpp
 *
 *  Created on: Sep 5, 2013
 *      Author: Sridhar
 */

#ifndef MYSQLDATASOURCE_H_
#define MYSQLDATASOURCE_H_

#include <QObject>

#include <bb/data/DataSource>
#include <bb/data/SqlDataAccess>

using namespace bb::data;

class MySqlDataSource : public QObject {
	Q_OBJECT

    /**
     * The path to the SQL database.
     */
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)

    /**
     * The initial query that will be run on the database.
     */
    Q_PROPERTY(QString query READ query WRITE setQuery NOTIFY queryChanged)

public:
	MySqlDataSource(QObject *parent = 0);
	virtual ~MySqlDataSource();

	/*!
	 * @brief Convenience method to set the database source
	 *
	 * @param source  SQL database filename
	 */
	void setSource(const QString source);
	/*!
	 * @brief Convenience method to read the database source/name
	 */
	QString source();
	/*!
	 * @brief Convenience method to set the database query
	 *
	 * @param query SQL query to read the data records
	 */
	void setQuery(const QString query);
	/*!
	 * @brief Convenience method to read the database query statement
	 */
	QString query();
	/*!
	 * @brief Convenience method to load the queried data into data model
	 */
	Q_INVOKABLE void loadData();
	Q_INVOKABLE void addClient(const QString vFirstName, const QString vLastName, const QString vPhoneNumber, const QString vEmailAddress);
	Q_INVOKABLE void editClient(const QString vFirstName, const QString vLastName, const QString vClientID, const QString vPhoneNumber, const QString vEmailAddress);
	Q_INVOKABLE void deleteClient(int clientID);
	Q_INVOKABLE int  addFile(const QString vClientID, const QString vFileTitle, const QString vDateHeader, const QString vComment);
	Q_INVOKABLE void editFile(const QString vFileTitle, const QString vDateHeader, const QString vComment, const QString vFileID);
	Q_INVOKABLE void deleteFile(int fileID);

signals:
	void loadSQLData(const QString&);
	void sourceChanged(QString source);
	void queryChanged(QString query);
	void dataLoaded (const QVariant &data);

private slots:
	void onLoadData(const QString& sql);

private:
	void createDatabase();
	void copyFileToDataFolder(const QString fileName);
	void alert(const QString &message);

    QString mSource;
    QString mQuery;

	QVariantList modelList;
};

#endif /* MYSQLDATASOURCE_H_ */
