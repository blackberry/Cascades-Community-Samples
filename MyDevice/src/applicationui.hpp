/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
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
#include <bb/device/SimCardInfo>
#include <bb/device/HardwareInfo>
#include <bb/device/BatteryInfo>
#include <bb/device/DisplayInfo>
#include <bb/cascades/DropDown>
#include <bb/system/phone/Phone>
#include <bb/system/phone/Line>

namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
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
    virtual ~ApplicationUI();
    bb::device::SimCardInfo *_sim;
    bb::device::HardwareInfo *_hw;
    bb::device::BatteryInfo *_bat;
    bb::device::DisplayInfo *_display;
    bb::system::phone::Phone *_phone;
    bb::system::phone::LineType::Type _line_type;
    bool _line_is_valid;
    QString _line_address;
    QString _line_description;
    QNetworkInterface _network_interface;
    QList<QNetworkInterface> _network_interfaces;
    bb::cascades::DropDown *_dd_net_interfaces;
    QList < bb::system::phone::Line > _lines;

    Q_INVOKABLE int getProcessorCount();
    Q_INVOKABLE QString getProcessorModels();
    Q_INVOKABLE QString getProcessorSpeeds();
    Q_INVOKABLE qreal getDisplayPhysicalHeight();
    Q_INVOKABLE qreal getDisplayPhysicalWidth();
    Q_INVOKABLE int getDisplayPixelHeight();
    Q_INVOKABLE int getDisplayPixelWidth();
    Q_INVOKABLE qreal getDisplayResolutionHeight();
    Q_INVOKABLE qreal getDisplayResolutionWidth();
    Q_INVOKABLE QString getNetworkInterfaceHwAddress();
    Q_INVOKABLE void selectNetworkInterfaceByName(QString name);
    Q_INVOKABLE QString getNetworkAddresses();
    Q_INVOKABLE bool isUp();
    Q_INVOKABLE bool isRunning();
    Q_INVOKABLE bool canBroadcast();
    Q_INVOKABLE bool isLoopback();
    Q_INVOKABLE bool isPointToPoint();
    Q_INVOKABLE bool canMulticast();
    Q_INVOKABLE QString getLineAddress();
    Q_INVOKABLE QString getLineDescription();
    Q_INVOKABLE QString getLineType();
    Q_INVOKABLE bool isLineValid();
    Q_INVOKABLE void selectLineById(QString id);

private slots:
    void onSystemLanguageChanged();
private:
    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;

    void populateNetworkInterfacesDropDown();
    void populatePhoneLinesDropDown();
};

#endif /* ApplicationUI_HPP_ */
