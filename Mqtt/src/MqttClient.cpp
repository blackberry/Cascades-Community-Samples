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

#include "MqttClient.h"
#include "MqttClientImpl.h"

namespace mqtt_client {

MqttClient::MqttClient(const char *hostName, const char *portNumber)
	: _mqttClientImpl(new MqttClientImpl(hostName, portNumber))
{
}

MqttClient::~MqttClient() {
	delete _mqttClientImpl;
}

void MqttClient::connect(const char *id, const char *willTopic, uint8_t willQos, uint8_t willRetain, const char *willMessage, MqttError &connectError)
{
	_mqttClientImpl->connect(id, willTopic, willQos, willRetain, willMessage, connectError);
}

void MqttClient::connect(const char *id, MqttError &connectError)
{
	_mqttClientImpl->connect(id, connectError);
}

void MqttClient::connect(const uint8_t *id, ssize_t idLen, const uint8_t *willTopic, ssize_t willTopicLen, uint8_t willQos, uint8_t willRetain, const uint8_t *willMessage, ssize_t willMessageLen, MqttError &connectError)
{
	_mqttClientImpl->connect(id, idLen, willTopic, willTopicLen, willQos, willRetain, willMessage, willMessageLen, connectError);
}

void MqttClient::connect(const uint8_t *id, ssize_t idLen, MqttError &connectError)
{
	_mqttClientImpl->connect(id, idLen, connectError);
}

void MqttClient::disconnect(MqttError &disconnectError)
{
	_mqttClientImpl->disconnect(disconnectError);
}

void MqttClient::publish(const char *topic, const char *payload, MqttError &publishError)
{
	_mqttClientImpl->publish(topic, payload, publishError);
}

void MqttClient::publish(const char *topic, const uint8_t *payload, ssize_t plength, MqttError &publishError)
{
	_mqttClientImpl->publish(topic, payload, plength, publishError);
}

void MqttClient::publish(const char *topic, const uint8_t *payload, ssize_t plength, uint8_t retained, MqttError &publishError)
{
	_mqttClientImpl->publish(topic, payload, plength, retained, publishError);
}

void MqttClient::publish(const uint8_t *topic, ssize_t tlength, const uint8_t *payload, ssize_t plength, MqttError &publishError)
{
	_mqttClientImpl->publish(topic, tlength, payload, plength, publishError);
}

void MqttClient::publish(const uint8_t *topic, ssize_t tlength, const uint8_t *payload, ssize_t plength, uint8_t retained, MqttError &publishError)
{
	_mqttClientImpl->publish(topic, tlength, payload, plength, retained, publishError);
}

void MqttClient::setCallbacks(mqttCallbacks_t callbacks)
{
	_mqttClientImpl->setCallbacks(callbacks);
}

void MqttClient::subscribe(const char *topic, MqttError &subscribeError)
{
	_mqttClientImpl->subscribe(topic, subscribeError);
}

void MqttClient::subscribe(const uint8_t *topic, ssize_t tlength, MqttError &subscribeError)
{
	_mqttClientImpl->subscribe(topic, tlength, subscribeError);
}

void MqttClient::unSubscribe(const char *topic, MqttError &unSubscribeError)
{
	_mqttClientImpl->unSubscribe(topic, unSubscribeError);
}

void MqttClient::unSubscribe(const uint8_t *topic, ssize_t tlength, MqttError &unSubscribeError)
{
	_mqttClientImpl->unSubscribe(topic, tlength, unSubscribeError);
}

bool MqttClient::isConnected()
{
	return (_mqttClientImpl->isConnected());
}

void MqttClient::setHostname(const char *hostName)
{
	_mqttClientImpl->setHostname(hostName);
}

void MqttClient::setPortNumber(const char *portNumber)
{
	_mqttClientImpl->setPortNumber(portNumber);
}

void MqttClient::setQos(const QosLevel qos)
{
	_mqttClientImpl->setQos(qos);
}

MqttClient::QosLevel MqttClient::getQos()
{
	return (_mqttClientImpl->getQos());
}

void MqttClient::getApiVersion(char *version) {
    _mqttClientImpl->getApiVersion(version);
}

} /* namespace mqtt_client */
