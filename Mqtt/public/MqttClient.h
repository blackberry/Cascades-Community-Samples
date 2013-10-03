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

#ifndef MQTTCLIENT_H_
#define MQTTCLIENT_H_

#include "MqttError.h"
#include "MqttCallbacks.h"

namespace mqtt_client {

class MqttClientImpl;

class MqttClient {
private:
	MqttClientImpl *_mqttClientImpl;
public:
	MqttClient(const char *hostName = "localhost", const char *portNumber = "1883");
	virtual ~MqttClient();

	enum QosLevel {AtMostOnce, AtLeastOnce, ExactlyOnce, Undefined};

	void connect(const char *id, const char *willTopic, uint8_t willQos, uint8_t willRetain, const char *willMessage, MqttError &connectError);
	void connect(const uint8_t *id, ssize_t idLen, const uint8_t *willTopic, ssize_t willTopicLen, uint8_t willQos, uint8_t willRetain, const uint8_t *willMessage, ssize_t willMessageLen, MqttError &connectError);

	void connect(const char *id, MqttError &connectError);
	void connect(const uint8_t *id, ssize_t idLen, MqttError &connectError);

	void disconnect(MqttError &disconnectError);

	void publish(const char *topic, const char *payload, MqttError &publishError);
	void publish(const char *topic, const uint8_t *payload, ssize_t plength, MqttError &publishError);
	void publish(const char *topic, const uint8_t *payload, ssize_t plength, uint8_t retained, MqttError &publishError);
	void publish(const uint8_t *topic, ssize_t tlength, const uint8_t *payload, ssize_t plength, MqttError &publishError);
	void publish(const uint8_t *topic, ssize_t tlength, const uint8_t *payload, ssize_t plength, uint8_t retained, MqttError &publishError);

	void setCallbacks(mqttCallbacks_t callbacks);

	void subscribe(const char *topic, MqttError &subscribeError);
	void subscribe(const uint8_t *topic, ssize_t tlength, MqttError &subscribeError);

	void unSubscribe(const char *topic, MqttError &unSubscribeError);
	void unSubscribe(const uint8_t *topic, ssize_t tlength, MqttError &unSubscribeError);

	bool isConnected();

	void setHostname(const char *hostName);
	void setPortNumber(const char *portNumber);

	void setQos(const QosLevel qos);
	QosLevel getQos();

    void getApiVersion(char *version);

};

} /* namespace mqtt_client */
#endif /* MQTTCLIENT_H_ */
