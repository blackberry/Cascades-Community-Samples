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
#include "SessionLogger.hpp"
#include <sys/time.h>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>

SessionLogger* SessionLogger::_instance;

SessionLogger::SessionLogger() {
}

SessionLogger::~SessionLogger() {
	_instance = 0;
}

SessionLogger* SessionLogger::getInstance() {
	if (_instance == 0) {
		_instance = new SessionLogger;
	}
	return _instance;
}

void SessionLogger::newSession() {
	qDebug() << "XXXX newSession";
	QDateTime dateTime = QDateTime::currentDateTime();
	QString dateTimeString = dateTime.toString("yyyy_MM_dd_hh_mm_ss_zzz");
	QString workingDir = QDir::currentPath();
	QString homeDir = QDir::homePath();
	QString rootDir = QDir::rootPath();

	qDebug() << "XXXX working dir: " << workingDir;
	qDebug() << "XXXX home dir   : " << homeDir;
	qDebug() << "XXXX root dir   : " << rootDir;

	bool shared_dir_ok=true;

	QString session_logs_dir_name = workingDir + "/shared/misc/CSC_session_logs";
	QDir session_dir(session_logs_dir_name);

	if (!session_dir.exists()) {
		qDebug() << "XXXX " << session_logs_dir_name << " does not exist";
		int rc = session_dir.mkpath(session_logs_dir_name);
		if (rc == 0) {
			qDebug() << "XXXX Error creating session logs directory: " << rc;
			shared_dir_ok = false;
		}
	}
	if (shared_dir_ok) {
		_session_log_name = session_logs_dir_name + "/session_";
	} else {
		_session_log_name = "data/session_";
	}
	_session_log_name.append(dateTimeString);
	_session_log_name.append(".txt");
	qDebug() << "XXXX newSession: log file name:" << _session_log_name;
	_session_log = new QFile(_session_log_name);
	qDebug() << "XXXX newSession: opening file";
	int file_rc = _session_log->open(QIODevice::WriteOnly | QIODevice::Text);
	qDebug() << "XXXX file open result=" << file_rc;
	qDebug() << "XXXX newSession: writing to file";
}

void SessionLogger::closeSession() {
	qDebug() << "XXXX closeSession";
	qDebug() << "XXXX closing session log";
	if (_session_log != 0) {
		_session_log->flush();
		_session_log->close();
		delete _session_log;
		_session_log = 0;
	}
}

void SessionLogger::writeSessionLog(QString message) {
	if (_session_log != 0) {
		message = message + "\n";
		_session_log->write(message.toAscii());
		_session_log->flush();
	}
}
