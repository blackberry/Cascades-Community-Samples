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

#include <Settings.hpp>

Settings* Settings::_instance;

Settings::Settings(QObject *parent)
	: QObject(parent)
	, _orgName("BlackBerry Ltd")
	, _appName("AnnualLeave Settings")
	, _dummyKey("DummyKey")
	, _settingsWatcher(new QFileSystemWatcher(this))
{
	QSettings qSettings(_orgName, _appName);

	/*
	 * If application is newly installed the QSettings file will not exist
	 * so we need to check for this case and force creation of the file before we
	 * can use QFileSystemWatcher.
	 */
	if (qSettings.value(_dummyKey).isNull()) {
		saveValueFor(_dummyKey, QDateTime::currentDateTime().toString(Qt::ISODate));
		qSettings.sync();

		qDebug() << "CCCC Settings::Settings() - created" << qSettings.fileName() << endl;
	}

	_settingsWatcher->addPath(qSettings.fileName());

	if (!QObject::connect(_settingsWatcher, SIGNAL(fileChanged(const QString&)),
                                      this, SLOT(onSettingsChanged(const QString&)))) {
		qWarning() << "CCCC Settings::Settings() - connect failed " << strerror(errno) << endl;
	}
}

Settings::~Settings() {}

Settings *Settings::getInstance(QObject *parent)
{
    if (_instance == 0) {
        _instance = new Settings(parent);
    }
    return _instance;
}

QString Settings::getValueFor(const QString &objectName, const QString &defaultValue)
{
	QSettings qSettings(_orgName, _appName);

	if (qSettings.value(objectName).isNull()) {
		return defaultValue;
	}

	return qSettings.value(objectName).toString();
}

QVariant Settings::getQvariantValueFor(const QString &objectName, const QString &defaultValue)
{
	QSettings qSettings(_orgName, _appName);

	if (qSettings.value(objectName).isNull()) {
		return QVariant(defaultValue);
	}

	return qSettings.value(objectName);
}

void Settings::saveValueFor(const QString &objectName, const QString &inputValue)
{
	QSettings qSettings(_orgName, _appName);

	qSettings.setValue(objectName, QVariant(inputValue.trimmed()));
	qSettings.sync();
}

void Settings::saveQvariantValueFor(const QString &objectName, const QVariant &inputValue)
{
	QSettings qSettings(_orgName, _appName);

	qSettings.setValue(objectName, inputValue);
	qSettings.sync();
}

void Settings::onSettingsChanged(const QString &path)
{
	Q_UNUSED(path)
	qDebug() << "CCCC Settings object has noticed that QSettings data has been modified";
	emit settingsChanged();
}

QStringList Settings::allKeys()
{
    QSettings qSettings(_orgName, _appName);
    return qSettings.allKeys();
}

