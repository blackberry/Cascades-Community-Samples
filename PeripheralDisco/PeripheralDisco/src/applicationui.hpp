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
    }
}

class QTranslator;

/**
 * UI (and serial comms) responsible class.
 */
class ApplicationUI : public QObject
{
    Q_OBJECT
public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI() { }

    /**
     * Know if serial is currently possible. Only going to be possible if host mode
     * supported - and a serial device is plugged in.
     */
    Q_PROPERTY(bool serialPossible READ serialPossible NOTIFY serialPossibleChanged);
    /**
     * Know if we are connected to a serial device.
     */
    Q_PROPERTY(bool serialConnected READ serialConnected NOTIFY serialConnectedChanged);

    /**
     * Open (the first) known serial device.
     */
    Q_INVOKABLE void openSerial();
    /**
     * Close the open serial device.
     */
    Q_INVOKABLE void closeSerial();
    /**
     * Write a value to the serial device.
     */
    Q_INVOKABLE void writeSerial(float);

signals:
	/**
	 * Fired when data is available to write to the log.
	 */
	void log(QString newLogMessage);
	/**
	 * Fired when serial possible has changed.
	 */
	void serialPossibleChanged(bool newVal);
	/**
	 * Fired when serial connected has changed.
	 */
	void serialConnectedChanged(bool newVal);

public slots:
	/**
	 * Going to receive connect events.
	 */
	void onPeripheralConnected(int peripheralId,PeripheralDetail details);
	/**
	 * Going to receive disconnect events.
	 */
	void onPeripheralDisconnected(int peripheralId,PeripheralDetail details);

private slots:
    void onSystemLanguageChanged();

    /**
     * Going to receive when data is available from the serial device.
     */
    void dataAvailable(int fd);

private:
    /**
     * Log a message (via emit)
     */
    void addToLog(QString val);

    bool serialPossible() {
    	return _serialPossible;
    }
    bool serialConnected() {
    	return serialFd != -1;
    }

    /**
     * Check if serial is possible with the current devices.
     */
    void checkSerial();

    /**
     * Pop-up a nice crisp message - with honey.
     */
    void toast(QString message);

    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;

    bool _serialPossible;
    bool _serialConnected;

    PeripheralOracle * peripheralOracle;

    /**
     * File descriptor of the serial device. Setting to -1 to indicate not open.
     */
    int serialFd;

    /**
     * Store the byte that was last written as a coordinate to the serial device.
     * Setting to 256 to indicate a new connection / nothing written.
     */
    int lastSerialWrite;
};

#endif /* ApplicationUI_HPP_ */
