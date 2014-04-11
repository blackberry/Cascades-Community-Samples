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

#include <GuiIndicator.hpp>
#include <common.hpp>

GuiIndicator* GuiIndicator::_instance;

GuiIndicator::GuiIndicator(QObject *parent) :
        QObject(parent), _indicatorWatcher(new QFileSystemWatcher(this))
{
    _indicator_dir_name = QDir::homePath().append(INDICATOR_DIR);
    qDebug() << "CCCC indicator directory: " << _indicator_dir_name;
    createIndicatorDirIfNecessary();

    _indicatorWatcher->addPath(indicatorDirectoryName());

    if (!QObject::connect(_indicatorWatcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(onIndication(const QString&)))) {
        qWarning() << "CCCC indicatorWatcher - connect failed " << strerror(errno) << endl;
    }

}

GuiIndicator::~GuiIndicator()
{
}

GuiIndicator *GuiIndicator::getInstance(QObject *parent)
{
    if (_instance == 0) {
        _instance = new GuiIndicator(parent);
    }
    return _instance;
}

void GuiIndicator::indicateWaitingInboundOperation()
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QString indicator_file_name("");

    indicator_file_name = _indicator_dir_name + "/" + dateTime.toString("yyyy_MM_dd_hh_mm_ss_zzz") + ".txt";
    qDebug() << "CCCC creating indicator file " << indicator_file_name;
    QFile *ind_file = new QFile(indicator_file_name);
    ind_file->open(QIODevice::WriteOnly | QIODevice::Text);
    delete (ind_file);
}

QString GuiIndicator::indicatorDirectoryName()
{
    return _indicator_dir_name;
}

void GuiIndicator::createIndicatorDirIfNecessary()
{
    _indicator_dir = QDir(_indicator_dir_name);
    if (!_indicator_dir.exists()) {
        qDebug() << "CCCC " << _indicator_dir_name << " does not exist";
        int rc = _indicator_dir.mkpath(_indicator_dir_name);
        if (rc == 0) {
            qDebug() << "CCCC Error creating indicator_dir directory: " << rc;
        }
    }
}

void GuiIndicator::clearIndications()
{
    qDebug() << "CCCC clearIndications()";
    QStringList ind_files = _indicator_dir.entryList();
    int file_count = ind_files.size();
    qDebug() << "CCCC clearIndications() has found " << file_count << " files in the indicator directory " << _indicator_dir.absolutePath();
    for (int i = 0; i < file_count; i++) {
        QString file_name = ind_files.at(i);
        if (file_name.compare("..") != 0) {
            _indicator_dir.remove(file_name);
        }
    }
}

void GuiIndicator::onIndication(const QString& path)
{
    emit signalInboundOperation();
}
