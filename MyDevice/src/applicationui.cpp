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

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/Container>
#include <bb/cascades/DropDown>
#include <bb/device/SimCardInfo>
#include <bb/system/phone/Phone>
#include <bb/system/phone/Line>
#include <bb/system/phone/LineType>

using namespace bb::cascades;

static AbstractPane *_root = 0;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
        QObject(app), _sim(new bb::device::SimCardInfo(this)), _hw(new bb::device::HardwareInfo(this)), _bat(new bb::device::BatteryInfo(this)), _phone(new bb::system::phone::Phone(this)), _display(new bb::device::DisplayInfo(this))
{

    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    // Create root object for the UI
    // Create root object for the UI
    _root = qml->createRootObject<AbstractPane>();

    _network_interfaces = QNetworkInterface::allInterfaces();

    populateNetworkInterfacesDropDown();
    populatePhoneLinesDropDown();

    qml->setContextProperty("app", this);
    qml->setContextProperty("sim", _sim);
    qml->setContextProperty("hw", _hw);
    qml->setContextProperty("bat", _bat);
    qml->setContextProperty("disp", _display);

    // Set created root object as the application scene
    app->setScene(_root);

}

ApplicationUI::~ApplicationUI()
{
    delete _sim;
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("MyDevice_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

int ApplicationUI::getProcessorCount()
{
    return _hw->processorCount();
}

QString ApplicationUI::getProcessorModels()
{
    QString models;
    for (int i = 0; i < _hw->processorCount(); i++) {
        models.append(QString("%1").arg(i));
        models.append(":");
        models.append(QString("%1").arg(_hw->processorModel(i)));
        if (i < (_hw->processorCount() - 1)) {
            models.append("\n");
        }
        qDebug() << "XXXX models:" << models;
    }
    return models;
}

QString ApplicationUI::getProcessorSpeeds()
{
    QString speeds("");
    for (int i = 0; i < _hw->processorCount(); i++) {
        speeds.append(QString("%1").arg(i));
        speeds.append(":");
        speeds.append(QString("%1").arg(_hw->processorSpeed(i)));
        if (i < (_hw->processorCount() - 1)) {
            speeds.append("\n");
        }
        qDebug() << "XXXX speeds:" << speeds;
    }
    return speeds;
}

qreal ApplicationUI::getDisplayPhysicalHeight()
{
    return _display->physicalSize().height();
}

qreal ApplicationUI::getDisplayPhysicalWidth()
{
    return _display->physicalSize().width();
}

int ApplicationUI::getDisplayPixelHeight()
{
    return _display->pixelSize().height();
}

int ApplicationUI::getDisplayPixelWidth()
{
    return _display->pixelSize().width();
}

qreal ApplicationUI::getDisplayResolutionHeight()
{
    return _display->resolution().height();
}

qreal ApplicationUI::getDisplayResolutionWidth()
{
    return _display->resolution().width();
}

void ApplicationUI::populateNetworkInterfacesDropDown()
{
    QString net_names("");

    bb::cascades::DropDown *dropDown = _root->findChild<bb::cascades::DropDown*>("networkInterfaces");

    int count = 0;

    QList<QNetworkInterface>::iterator i;
    for (i = _network_interfaces.begin(); i != _network_interfaces.end(); ++i) {
        net_names.append(i->name());
        net_names.append(QString("    "));
        if (i->isValid()) {
            dropDown->add(Option::create().text(i->name()));
        }
        count++;
    }

    if (count > 0) {
        _network_interface = i->interfaceFromIndex(0);
        dropDown->setSelectedIndex(0);
    }

    qDebug() << "XXXX network interface names: " << net_names;

}

QString ApplicationUI::getNetworkInterfaceHwAddress()
{
    qDebug() << "XXXX network interface hw address " << _network_interface.hardwareAddress();
    return _network_interface.hardwareAddress();
}

void ApplicationUI::selectNetworkInterfaceByName(QString name)
{
    qDebug() << "XXXX getting QNetworkInterface by name " << name;
    _network_interface = QNetworkInterface::interfaceFromName(name);
}

QString ApplicationUI::getNetworkAddresses()
{
    QString addresses("");
    QList<QNetworkAddressEntry> address_list = _network_interface.addressEntries();
    QList<QNetworkAddressEntry>::iterator i;

    int count = 0;
    int size = address_list.size();

    for (i = address_list.begin(); i != address_list.end(); ++i) {
        qDebug() << QString("XXXX acquiring IP addresses").arg(count + 1);
        addresses.append(QString("%1").arg((count + 1)));
        addresses.append(":");
        addresses.append(QString("%1").arg(i->ip().toString()));
        if (count < (size - 1)) {
            addresses.append("\n");
        }
        count++;
    }
    qDebug() << "XXXX network addresses: " << addresses;
    return addresses;
}

bool ApplicationUI::isUp()
{
    return ((_network_interface.flags() && 0x01) == 0x01);
}

bool ApplicationUI::isRunning()
{
    return ((_network_interface.flags() && 0x02) == 0x02);
}

bool ApplicationUI::canBroadcast()
{
    return ((_network_interface.flags() && 0x04) == 0x04);
}

bool ApplicationUI::isLoopback()
{
    return ((_network_interface.flags() && 0x08) == 0x08);
}

bool ApplicationUI::isPointToPoint()
{
    return ((_network_interface.flags() && 0x10) == 0x10);
}

bool ApplicationUI::canMulticast()
{
    return ((_network_interface.flags() && 0x20) == 0x20);
}

void ApplicationUI::populatePhoneLinesDropDown()
{

    qDebug() << "XXXX populatePhoneLinesDropDown";

    QString phone_lines("");

    bb::cascades::DropDown *dropDown = _root->findChild<bb::cascades::DropDown*>("phoneLines");

    qDebug() << "XXXX populatePhoneLinesDropDown: dropDown=" << *dropDown;

    if (dropDown) {
        int count = 0;
        qDebug() << "XXXX populatePhoneLinesDropDown: getting phone lines map";
        QMap<QString, bb::system::phone::Line> line_map = _phone->lines();

        qDebug() << "XXXX populatePhoneLinesDropDown: phone lines map empty? " << line_map.empty();
        _lines = line_map.values();

        qDebug() << "XXXX populatePhoneLinesDropDown: phone lines list empty? " << _lines.empty();
        QList<bb::system::phone::Line>::iterator i;
        for (i = _lines.begin(); i != _lines.end(); ++i) {
            qDebug() << "XXXX populatePhoneLinesDropDown: processing Line " << i->id();
            phone_lines.append(i->id());
            phone_lines.append(QString("    "));
            if (i->isValid()) {
                qDebug() << "XXXX populatePhoneLinesDropDown: adding Line to drop down " << i->id();
                dropDown->add(Option::create().text(i->id()));
                qDebug() << "XXXX populatePhoneLinesDropDown: done adding Line to drop down " << i->id();
                count++;
            }
        }

        qDebug() << "XXXX populatePhoneLinesDropDown: done processing lines list ";

        if (count > 0) {
            qDebug() << "XXXX populatePhoneLinesDropDown: storing first Line";

            _line_type = _lines.first().type();
            _line_address = _lines.first().address();
            _line_description = _lines.first().description();
            _line_is_valid = _lines.first().isValid();

            dropDown->setSelectedIndex(0);
        }
    } else {
        qDebug() << "Couldn't find phone lines dropdown control";
    }

    qDebug() << "XXXX phone_lines: " << phone_lines;

}

QString ApplicationUI::getLineAddress()
{
    qDebug() << "XXXX getLineAddress";
    return _line_address;
}

QString ApplicationUI::getLineDescription()
{
    qDebug() << "XXXX getLineDescription: line description is " << _line_description;
    return _line_description;
}

QString ApplicationUI::getLineType()
{

    qDebug() << "XXXX getLineType";

    switch (_line_type) {
        case bb::system::phone::LineType::Invalid:
            return QString("Invalid");
        case bb::system::phone::LineType::Cellular:
            return QString("Cellular");
        case bb::system::phone::LineType::MVS:
            return QString("MVS");
        case bb::system::phone::LineType::VideoChat:
            return QString("VideoChat");
        case bb::system::phone::LineType::SecuVOICE:
            return QString("SecuVOICE");
        default:
            return QString("Unknown");
    }

}

bool ApplicationUI::isLineValid()
{
    return _line_is_valid;
}

void ApplicationUI::selectLineById(QString id)
{
    qDebug() << "XXXX selectLineById: " << id;
    int line_count = _lines.size();
    for (int i = 0; i < line_count; i++) {
        bb::system::phone::Line this_line = _lines.at(i);
        qDebug() << "XXXX line_id=" << this_line.id();
        if (this_line.id().compare(id) == 0) {
            qDebug() << "XXXX selectLineById: line with id=" << id << " is item " << i;
            _line_type = this_line.type();
            _line_address = this_line.address();
            _line_description = this_line.description();
            _line_is_valid = this_line.isValid();
            return;
        }
    }
    qDebug() << "XXXX selectLineById: Could not find Line with id=" << id;
}
