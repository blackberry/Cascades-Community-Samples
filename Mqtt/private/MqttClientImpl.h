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

#ifndef MQTTCLIENTIMPL_H_
#define MQTTCLIENTIMPL_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#include "MqttClient.h"

#define MAX_PACKET_SIZE (1024)
#define MAX_ENCODED_LENGTH_SIZE (4)
#define STRING_LENGTH_SIZE (2)
#define REQUEST_LENGTH (1)
#define KEEPALIVE_SECONDS (15)
#define KEEPALIVE (15*1000) // max value = 255000

#define MQTT_PROTOCOL_VERSION       (3)
#define MQTT_CONNECT           (1 << 4)  // Client request to connect to Server
#define MQTT_CONNACK           (2 << 4)  // Connect Acknowledgment
#define MQTT_PUBLISH           (3 << 4)  // Publish message
#define MQTT_PUBACK            (4 << 4)  // Publish Acknowledgment
#define MQTT_PUBREC            (5 << 4)  // Publish Received (assured delivery part 1)
#define MQTT_PUBREL            (6 << 4)  // Publish Release (assured delivery part 2)
#define MQTT_PUBCOMP           (7 << 4)  // Publish Complete (assured delivery part 3)
#define MQTT_SUBSCRIBE         (8 << 4)  // Client Subscribe request
#define MQTT_SUBACK            (9 << 4)  // Subscribe Acknowledgment
#define MQTT_UNSUBSCRIBE      (10 << 4)  // Client Unsubscribe request
#define MQTT_UNSUBACK         (11 << 4)  // Unsubscribe Acknowledgment
#define MQTT_PINGREQ          (12 << 4)  // PING Request
#define MQTT_PINGRESP         (13 << 4)  // PING Response
#define MQTT_DISCONNECT       (14 << 4)  // Client is Disconnecting
#define MQTT_Reserved         (15 << 4)  // Reserved

#define MQTT_DUP_FLAG          (1 << 3)  // DUP flag

#define MQTT_QOS_AT_MOST_ONCE  (0 << 1)  // QoS = 0b00000000
#define MQTT_QOS_AT_LEAST_ONCE (1 << 1)  // QoS = 0b00000010
#define MQTT_QOS_EXACTLY_ONCE  (2 << 1)  // QoS = 0b00000100
#define MQTT_QOS_RESERVED      (3 << 1)  // QoS = 0b00000110 RESERVED

#define MQTT_RETAIN_FLAG            (1)  // RETAIN flag

#define LOG(...)                             \
	do {                                     \
		fprintf(stderr, "XXXX " __VA_ARGS__);\
		fflush(stderr);                      \
	} while (0)

#define LOGT(...)                            \
	do {                                     \
		fprintf(stderr, "TTTT " __VA_ARGS__);\
		fflush(stderr);                      \
	} while (0)

#define TRACE(...)                           \
	do {                                     \
		fprintf(stderr, __VA_ARGS__);        \
		fflush(stderr);                      \
	} while (0)

namespace mqtt_client {

class MqttClientImpl {

	char _hostName[256];
	int _hostNameSize;
	char _portNumber[10];
	int _portNumberSize;
    int _sockfd;
    uint8_t _inBuffer[MAX_PACKET_SIZE];
    uint8_t _outBuffer[MAX_PACKET_SIZE];
    uint16_t _nextMsgId;
    _uint64 _lastActivity;
    pthread_t _inboundEventThread;
    pthread_attr_t _attr;
    bool _processingEvents;
    bool _connected;
	pthread_mutex_t _outboundSocketMutex;
	mqttCallbacks_t _mqttCallbacks;
	int _pfd[2];
	bool _remoteDisconnectedUs;
	uint8_t _qosLevel;
	bool _disconnectInProgress;

	ssize_t writeString(const char *text, uint8_t *buf, ssize_t pos);
    ssize_t writeBytes(const uint8_t *data, ssize_t dataLength, uint8_t *buf, ssize_t pos);
    int write(uint8_t header, const uint8_t *buf, ssize_t length);
    ssize_t readPacket();
    ssize_t readByte(uint8_t *data);
    _uint64 currentMillis();
    void processInboundEvents();

    static void *inboundEventThread(void *arg);

public:
	MqttClientImpl(const char *hostName, const char *portNumber);
	virtual ~MqttClientImpl();

	void connect(const char *id, const char *willTopic, uint8_t willQos, uint8_t willRetain, const char *willMessage, MqttError &connectError);
	void connect(const char *id, MqttError &connectError);
	void connect(const uint8_t *id, ssize_t idLen, const uint8_t *willTopic, ssize_t willTopicLen, uint8_t willQos, uint8_t willRetain, const uint8_t *willMessage, ssize_t willMessageLen, MqttError &connectError);
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
	ssize_t payloadOffsetInPacket(const uint8_t *buffer, ssize_t packetLength);

	void setHostname(const char *hostName);
	void setPortNumber(const char *portNumber);

	void setQos(MqttClient::QosLevel qos);
	MqttClient::QosLevel getQos();
    void getApiVersion(char *version);
};

} /* namespace mqtt_client */

#endif /* MQTTCLIENTIMPL_H_ */
