/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
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

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include "BtleAdvertData.hpp"

#include <QObject>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/Application>
#include <bb/cascades/LocaleHandler>
#include <btapi/btdevice.h>
#include <btapi/btle.h>
#include <errno.h>
#include <bb/cascades/GroupDataModel>

using namespace bb::cascades;

namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
    }
}

class QTranslator;

class ApplicationUI : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bb::cascades::DataModel* model READ model CONSTANT)

public:
    ApplicationUI();
    virtual ~ApplicationUI();

    void emitMessageForUi(const QVariant &text);
	void parseBeaconData(const char *data, int len, int8_t rssi, const char *bdaddr);
	QString btEventName(int event);

private slots:
    void onSystemLanguageChanged();
    void initBluetooth();
    void deinitBluetooth();

public slots:
    void onStartListening();
    void onStopListening();

signals:
    void message(const QVariant &text);

private:
    bb::cascades::DataModel* model() const;

    QTranslator* _pTranslator;
    bb::cascades::LocaleHandler* _pLocaleHandler;
    QmlDocument *_qml;
    AbstractPane *_root;
    QObject *_mainPage;
    GroupDataModel *_model;
};

#endif /* ApplicationUI_HPP_ */
