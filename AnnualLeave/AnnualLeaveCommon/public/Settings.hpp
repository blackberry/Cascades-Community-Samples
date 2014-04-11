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

#ifndef SETTINGS_HPP_
#define SETTINGS_HPP_

#include <Common.hpp>

#include <QObject>
#include <QFileSystemWatcher>
#include <QSettings>
#include <errno.h>

class Q_DECL_EXPORT Settings
	: public QObject
{
	Q_OBJECT

public:
	virtual ~Settings();

	static Settings *getInstance(QObject *parent=NULL);
	QString getValueFor(const QString &objectName, const QString &defaultValue);
	void saveValueFor(const QString &objectName, const QString &inputValue);
	QVariant getQvariantValueFor(const QString &objectName, const QString &defaultValue);
	void saveQvariantValueFor(const QString &objectName, const QVariant &inputValue);
	QStringList allKeys();

signals:
	void settingsChanged();

private slots:
	void onSettingsChanged(const QString &path);

private:
	Settings(QObject *parent=NULL);

	static Settings* _instance;

	const QString _orgName;
	const QString _appName;
	const QString _dummyKey;

	QFileSystemWatcher *_settingsWatcher;
};

#endif /* SETTINGS_HPP_ */
