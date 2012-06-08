/* Copyright (c) 2012 Research In Motion Limited.
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
#ifndef About_h
#define About_h

#include <QtCore/QObject>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Control>
#include <bb/cascades/Container>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

class About : public QObject
{
    Q_OBJECT
	Q_PROPERTY(QString appVersion READ appVersion WRITE setAppVersion NOTIFY detectAppVersionChanged)

public:
	About();
    virtual ~About();

    QString appVersion() const;
	void setAppVersion(QString appVersion);

private:
    QString _appVersion;
    QmlDocument *_qml;
    AbstractPane *_root;

public slots:
	void show();

private slots:

signals:
	void detectAppVersionChanged();

};

#endif // ifndef About_h
