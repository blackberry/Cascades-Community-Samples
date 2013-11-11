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

#ifndef PUBSUBCLIENT_HPP_
#define PUBSUBCLIENT_HPP_

#include <QObject>
#include <qdebug>
#include <QTime>

#include <MqttClient.h>

#define LOG(...)                             \
	do {                                     \
		fprintf(stderr, "YYYY " __VA_ARGS__);\
		fflush(stderr);                      \
	} while (0)

using mqtt_client::MqttClient;
using mqtt_client::MqttError;

class PubSubClient : public QObject
{
    Q_OBJECT

public:
	PubSubClient(QObject *obj);
	virtual ~PubSubClient();

	static void subscribeReceivedCallback(const uint8_t *topic, int tlength, const uint8_t *payload, int plength);
	static void disconnectReceivedCallback();
	static void subscribeAckCallback(uint16_t messageId);
	static void unsubscribeAckCallback(uint16_t messageId);
	static void publishCallback(const uint8_t *topic, ssize_t tlength, const uint8_t *payload, ssize_t plength, uint8_t retained);
	static void publishAckCallback(uint16_t messageId);

	void processSubscribeReceivedCallback(const uint8_t *topic, int tlength, const uint8_t *payload, int plength);
	void processDisconnectReceivedCallback();
	void processSubscribeAckCallback(uint16_t messageId);
	void processUnsubscribeAckCallback(uint16_t messageId);
	void processPublishCallback(const uint8_t *topic, ssize_t tlength, const uint8_t *payload, ssize_t plength, uint8_t retained);
	void processPublishAckCallback(uint16_t messageId);
	void logApiVersion();

public slots:
    void mqttConnect(const QString &hostname);
    void mqttDisconnect();
    void mqttPublish(const QString &topic, const QString &message);
    void mqttSubscribe(const QString &topic);
    void mqttUnSubscribe(const QString &topic);

signals:
	void logMessage(const QVariant &);
	void mqttConnected();
	void mqttDisconnected();

private:
	bool isMqttConnected();
	bool _mqttConnected;
	mqtt_client::MqttClient *_mqttClient;
};

#endif /* PUBSUBCLIENT_HPP_ */
