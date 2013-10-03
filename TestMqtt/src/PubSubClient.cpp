/* Copyright (c) 2013 BlackBerry Limited.
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

#include "PubSubClient.hpp"

static PubSubClient *pubSubClientInstance = 0;
static char * api_version = 0;

PubSubClient::PubSubClient(QObject *obj) :
        QObject(obj), _mqttConnected(false), _mqttClient(new MqttClient())
{
    qDebug() << "Constructing PubSubClient";

    api_version = (char*) calloc(10,1);
    if (api_version != NULL) {
        _mqttClient->getApiVersion(api_version);
        qDebug() << QString("API VERSION %1").arg(QString(api_version));
    } else {
        qDebug() << "XXXX allocated api_version buffer is NULL";
    }


    pubSubClientInstance = this;
    mqtt_client::mqttCallbacks_t mqttCallbacks = { subscribeReceivedCallback, /* subscription */
    disconnectReceivedCallback, /* disconnected */
    publishCallback, /* publish      */
    publishAckCallback, /* puback       */
    NULL, /* pubrec       */
    NULL, /* pubcomp      */
    subscribeAckCallback, /* suback       */
    unsubscribeAckCallback /* unsuback     */
    };
    _mqttClient->setCallbacks(mqttCallbacks);
    _mqttClient->setQos(MqttClient::AtMostOnce);
}

void PubSubClient::logApiVersion() {

    emit logMessage(QString("Using MQTT API version %1").arg(QString(api_version)));

}

void PubSubClient::publishCallback(const uint8_t *topic, ssize_t tlength, const uint8_t *payload, ssize_t plength, uint8_t retained)
{
    LOG("In Publish Callback\n");
    if (pubSubClientInstance) {
        LOG("calling processPublishCallback()\n");
        pubSubClientInstance->processPublishCallback(topic, tlength, payload, plength, retained);
    }
}

void PubSubClient::processPublishCallback(const uint8_t *topic, ssize_t tlength, const uint8_t *payload, ssize_t plength, uint8_t retained)
{
    (void) topic; //suppress warning message
    (void) tlength; //suppress warning message
    (void) payload; //suppress warning message
    (void) plength; //suppress warning message
    (void) retained; //suppress warning message

    qDebug() << "AAAA in processPublishCallback()";
    emit logMessage(QString("Data published"));
}

void PubSubClient::publishAckCallback(uint16_t messageId)
{
    LOG("In Publish Ack Callback\n");
    LOG("Message Id=%d\n", messageId);
    if (pubSubClientInstance) {
        LOG("calling processPublishAckCallback()\n");
        pubSubClientInstance->processPublishAckCallback(messageId);
    }
}

void PubSubClient::processPublishAckCallback(uint16_t messageId)
{
    qDebug() << "AAAA in processPublishAckCallback() messageId=" << messageId;
    emit logMessage(QString("Publish acknowledged"));
}

void PubSubClient::subscribeAckCallback(uint16_t messageId)
{
    LOG("In Subscribe Ack Callback\n");
    LOG("Message Id=%d\n", messageId);
    if (pubSubClientInstance) {
        LOG("calling processSubscribeAckCallback()\n");
        pubSubClientInstance->processSubscribeAckCallback(messageId);
    }
}

void PubSubClient::processSubscribeAckCallback(uint16_t messageId)
{
    qDebug() << "AAAA in processSubscribeAckCallback() messageId=" << messageId;
    emit logMessage(QString("Subscription acknowledged"));
}

void PubSubClient::unsubscribeAckCallback(uint16_t messageId)
{
    LOG("In Unsubscribe Ack Callback\n");
    LOG("Message Id=%d\n", messageId);
    if (pubSubClientInstance) {
        LOG("calling processUnsubscribeAckCallback()\n");
        pubSubClientInstance->processUnsubscribeAckCallback(messageId);
    }
}

void PubSubClient::processUnsubscribeAckCallback(uint16_t messageId)
{
    qDebug() << "AAAA in processUnsubscribeAckCallback() messageId=" << messageId;
    emit logMessage(QString("Unsubscribe acknowledged"));
}

PubSubClient::~PubSubClient()
{
}

void PubSubClient::mqttConnect(const QString &hostname)
{
    qDebug() << "AAAA in mqttConnect()";

    qsrand(QTime::currentTime().msec());

    _mqttClient->setHostname(hostname.toLatin1().constData());

    MqttError connectError("");
    _mqttClient->connect(QString("MyId-").append(QString::number(qrand() % 30000)).toLatin1().data(), connectError);
    if (connectError.getCode() == MqttError::PASS_CODE) {
        emit logMessage(QString("Connected to %1").arg(hostname));
        emit mqttConnected();
    } else {
        emit logMessage(QString("Connection error: %1").arg(connectError.getDescription().data()));
    }
}

void PubSubClient::mqttDisconnect()
{
    qDebug() << "AAAA in mqttDisconnect()";
    MqttError disconnectError("");
    qDebug() << "AAAA calling disconnect";
    _mqttClient->disconnect(disconnectError);
    qDebug() << "AAAA done calling disconnect";
    if (disconnectError.getCode() == MqttError::PASS_CODE) {
        emit logMessage("Disconnected");
    } else {
        emit logMessage(QString("Disconnection error: %1").arg(disconnectError.getDescription().data()));
    }
    _mqttConnected = false;
    emit mqttDisconnected();
    qDebug() << "AAAA exiting mqttDisconnect";
}

void PubSubClient::mqttPublish(const QString &topic, const QString &message)
{
    qDebug() << "AAAA in mqttPublish()";
    MqttError publishError("");
    _mqttClient->publish(topic.toLatin1().constData(), message.toLatin1().constData(), publishError);
    if (publishError.getCode() == MqttError::PASS_CODE) {
        emit logMessage(QString("Publishing: %1 : %2").arg(topic).arg(message));
    } else {
        emit logMessage(QString("Publishing error: %1").arg(publishError.getDescription().data()));
    }
}

void PubSubClient::mqttSubscribe(const QString &topic)
{
    qDebug() << "AAAA in mqttSubscribe()";
    MqttError subscribeError("");
    _mqttClient->subscribe(topic.toLatin1().constData(), subscribeError);
    if (subscribeError.getCode() == MqttError::PASS_CODE) {
        emit logMessage(QString("Subscribing to: %1").arg(topic));
    } else {
        emit logMessage(QString("Error subscribing: %1").arg(subscribeError.getDescription().data()));
    }
}

void PubSubClient::mqttUnSubscribe(const QString &topic)
{
    qDebug() << "AAAA in mqttUnSubscribe()";
    MqttError unSubscribeError("");
    _mqttClient->unSubscribe(topic.toLatin1().constData(), unSubscribeError);
    if (unSubscribeError.getCode() == MqttError::PASS_CODE) {
        emit logMessage(QString("Unsubscribing from: %1").arg(topic));
    } else {
        emit logMessage(QString("Error unsubscribing: %1").arg(unSubscribeError.getDescription().data()));
    }
}

void PubSubClient::subscribeReceivedCallback(const uint8_t *topic, int tlength, const uint8_t *payload, int plength)
{
    LOG("In Subscribe Callback\n");
    LOG("topic length=%d\n", tlength);
    LOG("payload length=%d\n", plength);
    if (pubSubClientInstance) {
        LOG("calling processSubscribeReceivedCallback()\n");
        pubSubClientInstance->processSubscribeReceivedCallback(topic, tlength, payload, plength);
    }
}

void PubSubClient::disconnectReceivedCallback()
{
    LOG("In Disconnect Callback\n");
    if (pubSubClientInstance) {
        LOG("calling processDisconnectReceivedCallback()\n");
        pubSubClientInstance->processDisconnectReceivedCallback();
    }
}

void PubSubClient::processSubscribeReceivedCallback(const uint8_t *topic, int tlength, const uint8_t *payload, int plength)
{
    QString topicString = QByteArray::fromRawData(reinterpret_cast<const char *>(topic), tlength);
    QString payloadData = QByteArray::fromRawData(reinterpret_cast<const char *>(payload), plength);

    qDebug() << "AAAA in processSubscribeReceivedCallback() topic=" << topicString;
    qDebug() << "AAAA in processSubscribeReceivedCallback() payloadData=" << payloadData;

    emit logMessage(QString("Received: %1 : %2").arg(topicString).arg(payloadData));
}

void PubSubClient::processDisconnectReceivedCallback()
{
    qDebug() << "AAAA in processDisconnectReceivedCallback()";
    emit logMessage("Disconnected by server");
    mqttDisconnect();
}

bool PubSubClient::isMqttConnected()
{
    return _mqttConnected;
}
