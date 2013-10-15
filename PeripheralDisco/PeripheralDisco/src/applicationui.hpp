/**
 * Copyright (c) 2013 BlackBerry Limited.
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

#include <QObject>

#include <stdio.h>

#include "PeripheralOracle.hpp"
#include "PeripheralDetail.hpp"

namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
        class TextArea;
        class TextField;
    }
}

class QTranslator;

/*!
 * @brief Application object
 *
 *
 */

class ApplicationUI : public QObject
{
    Q_OBJECT
public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI() { }

    Q_PROPERTY(bool serialPossible READ serialPossible NOTIFY serialPossibleChanged);
    Q_PROPERTY(bool serialConnected READ serialConnected NOTIFY serialConnectedChanged);

    Q_INVOKABLE void openSerial();
    Q_INVOKABLE void closeSerial();
    Q_INVOKABLE void writeSerial(float);

signals:
	void log(QString newLogMessage);
	void serialPossibleChanged(bool newVal);
	void serialConnectedChanged(bool newVal);

public slots:
	void onPeripheralConnected(int peripheralId,PeripheralDetail details);
	void onPeripheralDisconnected(int peripheralId,PeripheralDetail details);

private slots:
    void onSystemLanguageChanged();
    void dataAvailable(int fd);

private:
    void addToLog(QString val);

    bool serialPossible() {
    	return _serialPossible;
    }
    bool serialConnected() {
    	return serialFd != -1;
    }
    void checkSerial();

    void toast(QString message);

    QTranslator* m_pTranslator;
    bb::cascades::TextArea * logTextArea;
    bb::cascades::TextField * logTextField;
    bb::cascades::LocaleHandler* m_pLocaleHandler;

    bool _serialPossible;
    bool _serialConnected;

    PeripheralOracle * peripheralOracle;

    int serialFd;
    int lastSerialWrite;
};

#endif /* ApplicationUI_HPP_ */
