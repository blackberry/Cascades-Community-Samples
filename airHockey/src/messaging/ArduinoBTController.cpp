/* Copyright (c) 2012, 2013 BlackBerry Limited.
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

#include "ArduinoBTController.hpp"

#include <QtCore>

#include <btapi/btdevice.h>
#include <btapi/btspp.h>

#include <errno.h>
#include <iostream>


#define SPP_SERVICE_UUID "00001101-0000-1000-8000-00805F9B34FB"

void BTControllerCallbackArduino(const int event, const char *bt_addr,
        const char *event_data) {

    qDebug() << "BTControllerCallbackArduino";

    qDebug() << "event" << event;
    qDebug() << "bt_addr" << bt_addr;
    qDebug() << "event_data" << event_data;

}

void ArduinoBTController::sendMyMessage(const char *msg) {
    write(m_sppDataThread.getFD(), msg, strlen(msg));
}

void BTControllerSPPCallbackArduino(long param, int fd) {
    printf("BTControllerSPPCallbackArduino\n");
    qDebug() << "BTControllerSPPCallbackArduino param" << param << "fd" << fd;
}

ArduinoBTController::~ArduinoBTController()
{
    int closeFD = m_sppDataThread.getFD();
    m_sppDataThread.resetFD();
    bt_spp_close(closeFD);

    int errorCode;
    errorCode = bt_spp_deinit();
    qDebug() << "bt_spp_deinit" << errorCode;
}


ArduinoBTController::ArduinoBTController(QObject* parent)
    : QObject(parent)
{
    // Initialize the bt device and SPP library APIs.
    int errorCode;

    errorCode = bt_device_init(BTControllerCallbackArduino);
    errorCode = bt_spp_init();

    bt_remote_device_t **remote_device_array;
    bt_remote_device_t *next_remote_device = 0;

    // Retrieve and show all devices.
    remote_device_array = bt_disc_retrieve_devices(BT_DISCOVERY_ALL, 0);
    if (remote_device_array) {
        for (int i = 0; (next_remote_device = remote_device_array[i]); ++i) {
            QVariantMap map;
            char buffer[128];
            const int bufferSize = sizeof(buffer);

            bt_rdev_get_friendly_name(next_remote_device, buffer, bufferSize);
            map["deviceName"] = QString::fromLatin1(buffer);
            bt_rdev_get_addr(next_remote_device, buffer);
            map["deviceAddress"] = QString::fromLatin1(buffer);
            if ( (strcmp(buffer, "00:12:09:12:02:05") == 0) or  (strcmp(buffer, "20:13:06:18:04:04") == 0) ) { //this are my devicesis my device's mac adress
                fprintf(stderr, "Found the BT device");
                int rssi = 0;
                bool ok = false;
                QString m_rssi;
                const QString unknown = tr("Unknown");
                bt_remote_device_t *remote_device = bt_rdev_get_device(buffer);

                ok = (bt_rdev_get_rssi(remote_device, &rssi) == 0);
                m_rssi = (ok ? QString::number(rssi) : unknown);

                const int fd = bt_spp_open(buffer, (char *) SPP_SERVICE_UUID,
                        false);

                if (fd >= 0) {

                    fflush(stdout);
                    if (fd >= 0) {
                        //starting the reciever/thread, I could use this to get data from a sensor
                            m_sppDataThread.init(fd,false);
                            m_sppDataThread.start();

                            //the shield has the bad habit of disconnecting if the app idles to long, so I'm sending a heartbeatsignal
                            m_keepAliveThread.init(fd);
                            m_keepAliveThread.start();

                            if (m_sppDataThread.active()) {
                                char buffer[80] = "HELLO"; //it is nice to say hello!
                                write(m_sppDataThread.getFD(), buffer, strlen(buffer));
                            }
                        }
                    } else {
                        qDebug() << "spp_open fail errno =" << QString::number(errno);
                    }
                }

            map["deviceClass"] = QString::number(
                    bt_rdev_get_device_class(next_remote_device,
                            BT_COD_DEVICECLASS));
            map["deviceType"] = tr("Bluetooth Devices  Paired");

        }
        bt_rdev_free_array(remote_device_array);
    }

    // Initialize the btdevice and SPP library APIs.
    bt_device_init(BTControllerCallbackArduino);
    bt_spp_init();
}
