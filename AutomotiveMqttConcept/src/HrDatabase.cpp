/* Copyright (c) 2013 Research In Motion Limited.
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
#include "HrDatabase.hpp"
#include "HrDataContainer.hpp"
#include "Utilities.hpp"
#include <QDebug>
#include <bb/data/SqlDataAccess>
#include <QtSql/QtSql>
#include <sys/time.h>

HrDatabase* HrDatabase::_instance;

HrDatabase::HrDatabase() {
}

HrDatabase* HrDatabase::getInstance() {
	if (_instance == 0) {
		_instance = new HrDatabase;
	}
	return _instance;
}

bool HrDatabase::initDatabase() {
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("./data/hr_data.db");

	if (db.open() == false) {
		QSqlError err = db.lastError();
		Utilities::alert("Error opening connection to the database: " + err.text());
		qDebug() << "\nDatabase NOT opened.";
		return false;
	}

	QSqlQuery q(db);
	QString createSQL = "CREATE TABLE IF NOT EXISTS hr_sessions "
			"  (session_id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"   session_datetime INTEGER, "
			"   session_name VARCHAR);";
	if (q.exec(createSQL)) {
		qDebug() << "XXXX hr_sessions table created.";
	} else {
		QSqlError error = q.lastError();
		Utilities::alert("Create hr_sessions table error: " + error.text());
		return false;
	}

	createSQL = "CREATE TABLE IF NOT EXISTS hr_data "
			"  (session_data_id INTEGER PRIMARY KEY AUTOINCREMENT, "
			"  session_id INTEGER, "
			"  hrm_datetime INTEGER, "
			"  hr_measurement INTEGER);";
	if (q.exec(createSQL)) {
		qDebug() << "XXXX hr_data table created.";
	} else {
		QSqlError error = q.lastError();
		Utilities::alert("Create hr_data table error: " + error.text());
		return false;
	}

	_q_hr_session_delete = QSqlQuery(db);
	_q_hr_session_delete.prepare("delete from hr_sessions"
			"	where session_id = :the_session_id");

	return true;
}

bool HrDatabase::saveSession(QString session_name) {

	// Get the local DB connection
	QSqlDatabase db = QSqlDatabase::database();
	if (db.open() == false) {
		QSqlError err = db.lastError();
		Utilities::alert("Error opening connection to the database: " + err.text());
		qDebug() << "\nDatabase NOT opened.";
		return false;
	}

	int time_saved = (int) Utilities::getSysTimeSeconds();
	qDebug() << "XXXX session time saved=" << time_saved;
	qDebug() << "XXXX session session_name=" << session_name;


	QString insert_session = QString("insert into hr_sessions (session_datetime, session_name) values (%1, '%2')").arg(time_saved).arg(session_name);
	qDebug() << "XXXX insert session SQL=" << insert_session;

	QSqlQuery sql_insert_session(insert_session);

	// Execute the query and check the result
	bool success = false;
	if (sql_insert_session.exec()) {
		qDebug() << "XXXX inserted hr_sessions record OK";
		QSqlQuery id_query("SELECT seq FROM sqlite_sequence where name='hr_sessions'");
		qDebug() << "XXXX retrieving allocated session ID value";
		if (id_query.next()) {
			qDebug() << "XXXX got record which should contain allocated ID";
			int session_id = id_query.value(0).toInt();
			qDebug() << "XXXX session ID=" << session_id;
			HrDataContainer* hrdc = HrDataContainer::getInstance();
			QQueue<hr_notification_t> notifications = hrdc->getNotifications();
			qDebug() << "XXXX going to insert " << notifications.size() << " hr_data records";
			for (int i = 0; i < notifications.size(); ++i) {
				hr_notification_t this_notification = notifications.at(i);
				QString insert_data = QString("insert into hr_data (session_id, hrm_datetime, hr_measurement) values (%1, %2, %3)").arg(session_id).arg(this_notification.timestamp).arg(this_notification.hr_measurement);
				qDebug() << "XXXX insert data SQL=" << insert_data;
				QSqlQuery sql_insert_data(insert_data);
				if (sql_insert_data.exec()) {
					qDebug() << "XXXX inserted hr_data record " << (i + 1) << " of " << notifications.size() << " OK";
				} else {
					QSqlError err = sql_insert_data.lastError();
					Utilities::alert("INSERT hr_data error: " + err.text());
					return false;
				}
			}
		} else {
			qDebug() << "XXXX could not find record which contains allocated ID";
			QSqlError err = id_query.lastError();
			Utilities::alert("INSERT id_query error: " + err.text());
			return false;
		}
	} else {
		QSqlError err = sql_insert_session.lastError();
		Utilities::alert("INSERT hr_sessions error: " + err.text());
		return false;
	}
	db.close();
	success = true;
	return success;
}

QList<hr_session_t> HrDatabase::loadSavedSessions() {
	qDebug() << "XXXX HrDatabase::loadSavedSessions()";

	QList<hr_session_t> sessions;
	QSqlDatabase db = QSqlDatabase::database();
	if (db.open() == false) {
		QSqlError err = db.lastError();
		Utilities::alert("Error opening connection to the database: " + err.text());
		qDebug() << "\nDatabase NOT opened.";
		return sessions;
	}
	QSqlQuery sessions_query(
			"SELECT hr_sessions.session_id, session_datetime, session_name,hrm_datetime, hr_measurement FROM hr_sessions,hr_data where hr_sessions.session_id = hr_data.session_id order by hr_sessions.session_id desc",
			db);

	int last_session_id = 0;
	int last_session_datetime = 0;
	QVariant last_session_name = "";
	int session_id = 0;
	int session_datetime = 0;
	QVariant session_name = "";
	QQueue<QVariant> hr_measurements;
	int record_count = 0;

	while (sessions_query.next()) {
		record_count++;
		qDebug() << "XXXX HrDatabase::loadSavedSessions() : processing hr_sessions record #" << record_count;
		session_id = sessions_query.value(0).toInt();
		session_datetime = sessions_query.value(1).toInt();
		session_name = sessions_query.value(2);
		QVariant hr_measurement = QVariant(sessions_query.value(3));
		if (record_count == 1) {
			last_session_id = session_id;
		}
		qDebug() << "XXXX HrDatabase::loadSavedSessions() : session_id=" << session_id << ",session_datetime=" << session_datetime << ",session_name=" << session_name;
		qDebug() << "XXXX HrDatabase::loadSavedSessions() : last_session_id=" << last_session_id << ",session_id=" << session_id;
		if (session_id != last_session_id) {
			qDebug() << "XXXX HrDatabase::loadSavedSessions() : making hr_session_t item";
			hr_session_t session = { last_session_id, last_session_datetime, last_session_name, hr_measurements };
			sessions.append(session);
			qDebug() << "XXXX HrDatabase::loadSavedSessions() : appending hr_session_t item to sessions list";
			last_session_id = session_id;
			last_session_name = session_name;
			last_session_datetime = session_datetime;
			hr_measurements = QQueue<QVariant>();
		}
		hr_measurements.enqueue(hr_measurement);
	}
	// last session
	hr_session_t session = { last_session_id, last_session_datetime, last_session_name, hr_measurements };
	sessions.append(session);
	qDebug() << "XXXX HrDatabase::loadSavedSessions() : appended last session";
	last_session_id = session_id;
	last_session_name = session_name;
	last_session_datetime = session_datetime;
	hr_measurements = QQueue<QVariant>();

	_session_listing = sessions;

	return sessions;

}

bool HrDatabase::deleteSession(int session_id) {
	_q_hr_session_delete.bindValue(":the_session_id", session_id);
	if (_q_hr_session_delete.exec()) {
		return true;
	} else {
		QSqlError err = _q_hr_session_delete.lastError();
		Utilities::alert("DELETE hr_sessions error: " + err.text());
		return false;
	}
}
