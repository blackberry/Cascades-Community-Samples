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
#include <bps/netstatus.h>

using namespace bb::cascades;

static AbstractPane *_root = 0;

QString getInterfaceType(netstatus_interface_type_t if_type, QString network_intf_name)
{
    switch (if_type) {
        case NETSTATUS_INTERFACE_TYPE_WIRED:
            return QString("WIRED");
        case NETSTATUS_INTERFACE_TYPE_WIFI:
            return QString("WIFI");
        case NETSTATUS_INTERFACE_TYPE_BLUETOOTH_DUN:
            return QString("BLUETOOTH DUN");
        case NETSTATUS_INTERFACE_TYPE_USB:
            return QString("USB");
        case NETSTATUS_INTERFACE_TYPE_VPN:
            return QString("VPN");
        case NETSTATUS_INTERFACE_TYPE_BB:
            return QString("BB");
        case NETSTATUS_INTERFACE_TYPE_CELLULAR:
            return QString("CELLULAR");
        case NETSTATUS_INTERFACE_TYPE_P2P:
            return QString("P2P");
    }

    if (network_intf_name.indexOf("lo") == 0) {
        return "LOOPBACK";
    }
    if (network_intf_name.indexOf("en") == 0) {
        return "ETHERNET";
    }
    if (network_intf_name.indexOf("cellular") == 0) {
        return "CELLULAR";
    }
    if (network_intf_name.indexOf("ste") == 0) {
        return "CELLULAR";
    }
    if (network_intf_name.indexOf("msm") == 0) {
        return "CELLULAR";
    }
    if (network_intf_name.indexOf("rndis") == 0) {
        return "USB";
    }
    if (network_intf_name.indexOf("ecm") == 0) {
        return "USB";
    }
    if (network_intf_name.indexOf("ncm") == 0) {
        return "USB";
    }
    if (network_intf_name.indexOf("smsc") == 0) {
        return "USB";
    }
    if (network_intf_name.indexOf("tiw") == 0) {
        return "WIFI";
    }
    if (network_intf_name.indexOf("bcm") == 0) {
        return "WIFI";
    }
    if (network_intf_name.indexOf("qca") == 0) {
        return "WIFI";
    }
    if (network_intf_name.indexOf("pgp") == 0) {
        return "WIFI";
    }
    if (network_intf_name.indexOf("vpn") == 0) {
        return "VPN";
    }
    if (network_intf_name.indexOf("openvpn") == 0) {
        return "VPN";
    }
    if (network_intf_name.indexOf("bptp") == 0) {
        return "P2P Logical";
    }
    if (network_intf_name.indexOf("ppp") == 0) {
        return "PPP Logical";
    }
    if (network_intf_name.indexOf("asix") == 0) {
        return "USB";
    }
    if (network_intf_name.indexOf("ipsec") == 0) {
        return "IPSEC";
    }
    if (network_intf_name.indexOf("bb") == 0) {
        return "LOCAL HTTP PROXY";
    }
    return QString("UNRECOGNISED");
}

QString getInterfaceIpStatus(netstatus_ip_status_t if_status)
{
    switch (if_status) {
        case NETSTATUS_IP_STATUS_ERROR_UNKNOWN:
            return QString("UNKNOWN");
        case NETSTATUS_IP_STATUS_OK:
            return QString("OK");
        case NETSTATUS_IP_STATUS_ERROR_NOT_CONNECTED:
            return QString("NOT CONNECTED");
        case NETSTATUS_IP_STATUS_ERROR_NOT_UP:
            return QString("NOT UP");
        case NETSTATUS_IP_STATUS_ERROR_NOT_CONFIGURED:
            return QString("NOT CONFIGURED");
        case NETSTATUS_IP_STATUS_ERROR_IP6_OFF:
            return QString("IP6 OFF");
        case NETSTATUS_IP_STATUS_ERROR_NO_IP_ADDRESS:
            return QString("NO IP ADDRESS");
        case NETSTATUS_IP_STATUS_ERROR_NO_IP6_ADDRESS:
            return QString("NO IP6 ADDRESS");
        case NETSTATUS_IP_STATUS_ERROR_NO_IP_GATEWAY:
            return QString("NO IP GATEWAY");
        case NETSTATUS_IP_STATUS_ERROR_NO_IP6_GATEWAY:
            return QString("NO IP6 GATEWAY");
        case NETSTATUS_IP_STATUS_ERROR_NO_NAME_SERVER:
            return QString("ERROR NO NAME SERVER");
        default:
            return QString("UNRECOGNISED");
    }
}

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
        QObject(app)
, _sim(new bb::device::SimCardInfo(this))
, _hw(new bb::device::HardwareInfo(this))
, _bat(new bb::device::BatteryInfo(this))
, _phone(new bb::system::phone::Phone(this))
, _display(new bb::device::DisplayInfo(this))
, _bps_interfaces_attr_map(new QMap<QString, QMap<QString, QVariant> >())
, _q_interfaces_attr_map(new QMap<QString, QNetworkInterface>())
, _selected_interface(new QMap <QString, QVariant>())
, _selected_if_name("")
, _selected_if_type("")
, _selected_ip_addresses("")
, _selected_link_addr("")
, _selected_name_servers("")
, _selected_ip_status("")
, _selected_if_is_up(false)
, _selected_if_is_running(false)
, _selected_if_can_broadcast(false)
, _selected_if_is_loopback(false)
, _selected_if_is_p2p(false)
, _selected_if_can_multicast(false)
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

    qml->setContextProperty("app", this);
    qml->setContextProperty("sim", _sim);
    qml->setContextProperty("hw", _hw);
    qml->setContextProperty("bat", _bat);
    qml->setContextProperty("disp", _display);

    _root = qml->createRootObject<AbstractPane>();

    populateNetworkInterfacesDropDown();
    populatePhoneLinesDropDown();

    // Set created root object as the application scene
    app->setScene(_root);

}

ApplicationUI::~ApplicationUI()
{
    delete _sim;
    delete _hw;
    delete _bat;
    delete _phone;
    delete _display;
    delete _bps_interfaces_attr_map;
    delete _q_interfaces_attr_map;
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
    bb::cascades::DropDown *dropDown = _root->findChild<bb::cascades::DropDown*>("networkInterfaces");

    // We'll augment the netstatus information from the QNetworkInterfaces list so we need it first
    buildQNetworkInterfacesAttributeMap();

    // use network interface details from BPS as the master list
    buildBpsNetworkInterfacesAttributeMap();


// Example content of the _bps_interfaces_attr_map
// _bps_interfaces_attr_map:  "bcm0" :
    //  QMap(("if_up", QVariant(bool, true) )
    //      ( "ip_addresses" ,  QVariant(QString, "172.20.119.102 fe80::96eb:cdff:fe3c:b74e%bcm0 ") )
    //      ( "ip_status" ,     QVariant(QString, "NETSTATUS_IP_STATUS_OK") )
    //      ( "link_addr" ,     QVariant(QString, "94:eb:cd:3c:b7:4e") )
    //      ( "name" ,          QVariant(QString, "bcm0") )
    //      ( "name_servers" ,  QVariant(QString, "194.168.8.100 194.168.4.100 ") )
    //      ( "type" ,  QVariant(QString, "NETSTATUS_INTERFACE_TYPE_WIFI") ) )

    QMapIterator<QString, QMap<QString, QVariant> > mi(*_bps_interfaces_attr_map);
    while (mi.hasNext()) {
        mi.next();
        dropDown->add(Option::create().text(mi.key()));
        qDebug() << "NNNN _bps_interfaces_attr_map: " << mi.key() << ": " << mi.value();
    }

    if (_bps_interfaces_attr_map->size() > 0) {
        dropDown->setSelectedIndex(0);
    }

}

void ApplicationUI::buildBpsNetworkInterfacesAttributeMap()
{
    // the size of the netstatus_interface_list_t array does not change hence we don't need to know the number of interfaces when allocating memory
    netstatus_interface_list_t* interface_list = (netstatus_interface_list_t*) malloc(sizeof(netstatus_interface_list_t));

    int rc = netstatus_get_interfaces(interface_list);

    qDebug() << "NNNN netstatus_get_interfaces: " << (rc == BPS_SUCCESS ? QString("SUCCESS") : QString("FAILED: %1").arg(errno));

    if (rc == BPS_SUCCESS) {
        int num_ifs = interface_list->num_interfaces;
        qDebug() << "XXXX device has " << num_ifs << " interfaces";
        for (int i = 0; i < num_ifs; i++) {
            netstatus_interface_details_t* details;
            QMap<QString, QVariant> if_details;
            int rc = netstatus_get_interface_details(interface_list->interfaces[i], &details);
            if (rc != BPS_SUCCESS) {
                /* handle error */
            } else {
                const char * name = netstatus_interface_get_name(details);
                qDebug() << "NNNN i/f name=" << name;
                if_details.insert(QString("name"), QVariant(name));

                netstatus_interface_type_t if_type = netstatus_interface_get_type(details);
                qDebug() << "NNNN i/f type=" << getInterfaceType(if_type,name);
                if_details.insert(QString("type"), QVariant(getInterfaceType(if_type,name)));

                int num_ips = netstatus_interface_get_num_ip_addresses(details);
                QString ip_addresses("");
                for (int i = 0; i < num_ips; i++) {
                    const char* ip = netstatus_interface_get_ip_address(details, i);
                    qDebug() << "    NNNN IP " << i << ":" << ip;
                    ip_addresses.append(ip).append(" ");
                }
                if_details.insert(QString("ip_addresses"), QVariant(ip_addresses));

                bool if_up = netstatus_interface_is_up(details);
                qDebug() << "NNNN up? " << if_up;
                if_details.insert(QString("if_up"), QVariant(if_up));

                netstatus_ip_status_t ip_status = netstatus_interface_get_ip_status(details);
                qDebug() << "NNNN IP status=" << getInterfaceIpStatus(ip_status);
                if_details.insert(QString("ip_status"), QVariant(getInterfaceIpStatus(ip_status)));

                const char* link_addr = netstatus_interface_get_link_address(details);
                qDebug() << "NNNN Link Address=" << link_addr;
                if_details.insert(QString("link_addr"), QVariant(link_addr));

                int num_name_servers = netstatus_interface_get_num_name_servers(details);
                QString name_servers("");
                qDebug() << "NNNN Number of name servers: " << num_name_servers;
                for (int i = 0; i < num_name_servers; i++) {
                    const char* name_server = netstatus_interface_get_name_server(details, i);
                    qDebug() << "    NNNN Name Server " << i << ":" << name_server;
                    name_servers.append(name_server).append(" ");
                }
                if_details.insert(QString("name_servers"), QVariant(name_servers));

                netstatus_free_interface_details(&details);

                _bps_interfaces_attr_map->insert(QString(name), if_details);
            }

        }

        netstatus_free_interfaces(interface_list);
    }

}

void ApplicationUI::buildQNetworkInterfacesAttributeMap()
{

    QString net_names("");

    QList <QNetworkInterface> network_interfaces = QNetworkInterface::allInterfaces();

    for (int i=0;i<network_interfaces.size();i++) {
        net_names.append(network_interfaces.at(i).name());
        net_names.append(QString("    "));
        _q_interfaces_attr_map->insert(network_interfaces.at(i).name(), network_interfaces.at(i));
    }

    qDebug() << "NNNN Qt network interface names: " << net_names;

    qDebug() << "NNNN Qt interface attributes:";

    QMapIterator<QString, QNetworkInterface > mi(*_q_interfaces_attr_map);
    while (mi.hasNext()) {
        mi.next();
        qDebug() << "NNNN _q_interfaces_attr_map: " << mi.key() << ": " << mi.value();
    }


}

QMap<QString, QVariant> ApplicationUI::getInterfaceAttributes(QString if_name)
{
    QMap<QString, QVariant> if_details = _bps_interfaces_attr_map->value(if_name);
    return if_details;
}

void ApplicationUI::selectNetworkInterfaceByName(QString if_name)
{
    qDebug() << "XXXX getting interface details by name " << if_name;

    // refresh data - we assume no new interfaces will materialise during the application's run time but that interface attributes may change
    buildQNetworkInterfacesAttributeMap();
    buildBpsNetworkInterfacesAttributeMap();

    QMap<QString, QVariant> if_details = _bps_interfaces_attr_map->value(if_name);
    _selected_if_name = if_name;
    _selected_if_type = if_details.value("type").toString();
    _selected_ip_addresses = if_details.value("ip_addresses").toString();
    _selected_link_addr = if_details.value("link_addr").toString();
    _selected_name_servers = if_details.value("name_servers").toString();
    _selected_ip_status = if_details.value("ip_status").toString();
    _selected_if_is_up = if_details.value("if_up").toBool();

    // and from the QNetworkInterface API

    _network_interface = _q_interfaces_attr_map->value(if_name);
    qDebug() << "NNNN selected QNetworkInterface:" << _network_interface;

    _selected_if_is_running = isRunning();

    _selected_if_can_broadcast = canBroadcast();
    _selected_if_is_loopback = isLoopback();
    _selected_if_is_p2p = isPointToPoint();
    _selected_if_can_multicast = canMulticast();

    emit signalSelectedIfTypeChanged();
    emit signalSelectedIpAddressesChanged();
    emit signalSelectedLinkAddrChanged();
    emit signalSelectedNameServersChanged();
    emit signalSelectedIpStatusChanged();
    emit signalSelectedIfIsUpChanged();
    emit signalSelectedIfIsRunningChanged();
    emit signalSelectedIfCanBroadcastChanged();
    emit signalSelectedIfIsLoopbackChanged();
    emit signalSelectedIfIsP2pChanged();
    emit signalSelectedIfCanMulticastChanged();

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
    return ((_network_interface.flags() & 0x01) == 0x01);
}

bool ApplicationUI::isRunning()
{
    return ((_network_interface.flags() & 0x02) == 0x02);
}

bool ApplicationUI::canBroadcast()
{
    return ((_network_interface.flags() & 0x04) == 0x04);
}

bool ApplicationUI::isLoopback()
{
    return ((_network_interface.flags() & 0x08) == 0x08);
}

bool ApplicationUI::isPointToPoint()
{
    return ((_network_interface.flags() & 0x10) == 0x10);
}

bool ApplicationUI::canMulticast()
{
    return ((_network_interface.flags() & 0x20) == 0x20);
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

QString ApplicationUI::getSelectedIfType() {
    return _selected_if_type;
}

QString ApplicationUI::getSelectedIpAddresses() {
    return _selected_ip_addresses;
}

QString ApplicationUI::getSelectedLinkAddr() {
    return _selected_link_addr;
}

QString ApplicationUI::getSelectedNameServers() {
    return _selected_name_servers;
}


QString ApplicationUI::getSelectedIpStatus() {
    return _selected_ip_status;
}


bool ApplicationUI::isSelectedIfUp() {
    return _selected_if_is_up;
}

bool ApplicationUI::isSelectedIfRunning() {
    return _selected_if_is_running;
}

bool ApplicationUI::canSelectedIfBroadcast() {
    return _selected_if_can_broadcast;
}

bool ApplicationUI::isSelectedIfLoopback() {
    return _selected_if_is_loopback;
}

bool ApplicationUI::isSelectedIfP2p() {
    return _selected_if_is_p2p;
}

bool ApplicationUI::canSelectedIfMulticast() {
    return _selected_if_can_multicast;
}
