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

#ifndef GUI_INDICATOR_HPP_
#define GUI_INDICATOR_HPP_

#include <Common.hpp>

#include <QObject>
#include <errno.h>

class Q_DECL_EXPORT GuiIndicator
: public QObject
{
    Q_OBJECT

public:
    virtual ~GuiIndicator();
    static GuiIndicator *getInstance(QObject *parent=NULL);

    // Ops API will call this method when it has placed an operation in the inbound queue and wants the running GUI to pick it up
    // This will cause an "indicator file" with a file name derived from the current time to be created in the "indicator director".
    // The GUI will use QFileWatcher to watch for files appearing in this directory and when a new one does, use this as a trigger to processing the inbound queue
    void indicateWaitingInboundOperation();

    QString indicatorDirectoryName();

    void createIndicatorDirIfNecessary();

    void clearIndications();

    signals:
    void signalInboundOperation();

private:
    GuiIndicator(QObject *parent=NULL);

    static GuiIndicator* _instance;

    QString _indicator_dir_name;
    QDir _indicator_dir;

    QFileSystemWatcher *_indicatorWatcher;

private slots:
    void onIndication(const QString& path);

};

#endif /* GUI_INDICATOR_HPP_ */
