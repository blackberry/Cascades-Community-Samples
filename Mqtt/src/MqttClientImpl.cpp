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

#include "MqttClientImpl.h"

namespace mqtt_client {

static char* API_VERSION = "0.1.0\0";

MqttClientImpl::MqttClientImpl(const char *hostName, const char *portNumber)
	: _sockfd(-1)
	, _nextMsgId(0)
	, _lastActivity(0)
	, _inboundEventThread(0)
	, _processingEvents(true)
	, _connected(false)
	, _remoteDisconnectedUs(false)
	, _qosLevel(MQTT_QOS_AT_MOST_ONCE)
	, _disconnectInProgress(false)
{
	_hostNameSize = (sizeof(_hostName)/sizeof(_hostName[0]));
	memset(_hostName, '\0', _hostNameSize);
	strncpy(_hostName, hostName, _hostNameSize-1);

	_portNumberSize = (sizeof(_portNumber)/sizeof(_portNumber[0]));
	memset(_portNumber, '\0', _portNumberSize);
	strncpy(_portNumber, portNumber, _portNumberSize-1);

	_mqttCallbacks.subscription = NULL;
	_mqttCallbacks.disconnected = NULL;
	_mqttCallbacks.publish      = NULL;
	_mqttCallbacks.puback       = NULL;
	_mqttCallbacks.pubrec       = NULL;
	_mqttCallbacks.pubcomp      = NULL;
	_mqttCallbacks.suback       = NULL;
	_mqttCallbacks.unsuback     = NULL;

	pthread_mutex_init(&_outboundSocketMutex, NULL);
}

MqttClientImpl::~MqttClientImpl()
{
	_mqttCallbacks.subscription = NULL;
	_mqttCallbacks.disconnected = NULL;
	_mqttCallbacks.publish      = NULL;
	_mqttCallbacks.puback       = NULL;
	_mqttCallbacks.pubrec       = NULL;
	_mqttCallbacks.pubcomp      = NULL;
	_mqttCallbacks.suback       = NULL;
	_mqttCallbacks.unsuback     = NULL;

	MqttError discError("Destructor");
	if (_sockfd != -1) {
		disconnect(discError);
	}
	pthread_mutex_destroy(&_outboundSocketMutex);
}

void MqttClientImpl::setCallbacks(mqttCallbacks_t callbacks) {
	_mqttCallbacks.subscription = callbacks.subscription;
	_mqttCallbacks.disconnected = callbacks.disconnected;
	_mqttCallbacks.publish      = callbacks.publish;
	_mqttCallbacks.puback       = callbacks.puback;
	_mqttCallbacks.pubrec       = callbacks.pubrec;
	_mqttCallbacks.pubcomp      = callbacks.pubcomp;
	_mqttCallbacks.suback       = callbacks.suback;
	_mqttCallbacks.unsuback     = callbacks.unsuback;
}

void MqttClientImpl::connect(const uint8_t *id, ssize_t idLen,
		                     const uint8_t *willTopic, ssize_t willTopicLen, uint8_t willQos,
		                     uint8_t willRetain, const uint8_t *willMessage, ssize_t willMessageLen,
		                     MqttError &connectError)
{
	LOG("In connect()\n");

	struct addrinfo hints, *currentAddressInfo, *addressInfo;
	int error;
	int mySocket;
    int flags;

	if (isConnected()) {
    	connectError.set("Already connected");
		return;
	}
	LOG("Creating Pipe\n");

    pipe(_pfd);

    flags = fcntl(_pfd[0], F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(_pfd[0], F_SETFL, flags);

    flags = fcntl(_pfd[1], F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(_pfd[1], F_SETFL, flags);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	error = ::getaddrinfo(_hostName, _portNumber, &hints, &addressInfo);
	if (error) {
    	connectError.set("Error resolving hostname");
    	return;
	}
	mySocket = -1;

	for (currentAddressInfo = addressInfo;
		 currentAddressInfo;
		 currentAddressInfo = currentAddressInfo->ai_next) {

		mySocket = ::socket(currentAddressInfo->ai_family,
	    		            currentAddressInfo->ai_socktype,
	    		            currentAddressInfo->ai_protocol);
	    if (mySocket < 0) {
	    	connectError.set("Error opening socket");
	        continue;
	    }
	    if (::connect(mySocket, currentAddressInfo->ai_addr, currentAddressInfo->ai_addrlen) < 0) {
	    	connectError.set("Error connecting to server");
	        ::close(mySocket);
	        mySocket = -1;
	        continue;
	    }
	    break;
	}
	::freeaddrinfo(addressInfo);

	if (mySocket < 0) {
    	return;
	}
	_sockfd = mySocket;
    _nextMsgId = 1;

    uint8_t d[] = {0x00,0x06,'M','Q','I','s','d','p',MQTT_PROTOCOL_VERSION};
    ssize_t length = 0;

    int j;
    for (j = 0; j < (int)(sizeof(d)/sizeof(d[0])); j++) {
       _outBuffer[length++] = d[j];
    }

    if (willTopic) {
       _outBuffer[length++] = 0x06 | (willQos << 3) | (willRetain << 5);
    } else {
       _outBuffer[length++] = 0x02;
    }

    _outBuffer[length++] = 0;
    _outBuffer[length++] = (KEEPALIVE/1000);
    length = writeBytes(id, idLen, _outBuffer, length);

    if (willTopic) {
       length = writeBytes(willTopic, willTopicLen, _outBuffer, length);
       length = writeBytes(willMessage, willMessageLen, _outBuffer, length);
    }

    write(MQTT_CONNECT, _outBuffer, length);
    ssize_t len = readPacket();

    if (len == 4 && _inBuffer[3] == 0) {
       _lastActivity = currentMillis();
    }

    _connected = true;
    _remoteDisconnectedUs = false;
    _disconnectInProgress = false;

    connectError.set("Connected", MqttError::PASS_CODE);

    int policy;
    struct sched_param param;

    pthread_attr_init(&_attr);
    pthread_attr_setdetachstate(&_attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setinheritsched(&_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_getschedparam(pthread_self(), &policy, &param);
    param.sched_priority=12;
    pthread_attr_setschedparam(&_attr, &param);
    pthread_attr_setschedpolicy(&_attr, SCHED_RR);
    pthread_create(&_inboundEventThread, &_attr, &inboundEventThread, this);
}

void MqttClientImpl::connect(const uint8_t *id, ssize_t idLen, MqttError &connectError)
{
	connect(id, idLen, NULL, 0, 0, 0, NULL, 0, connectError);
}

void MqttClientImpl::connect(const char *id, MqttError &connectError)
{
	connect(id, NULL, 0, 0, NULL, connectError);
}

void MqttClientImpl::connect(const char *id,
		                     const char *willTopic, uint8_t willQos,
		                     uint8_t willRetain, const char *willMessage,
		                     MqttError &connectError)
{
	connect((uint8_t *)id, ((id == NULL) ? 0 : strlen(id)),
			(uint8_t *)willTopic, ((willTopic == NULL) ? 0 : strlen(willTopic)),
			willQos, willRetain,
			(uint8_t *)willMessage, ((willMessage == NULL) ? 0 : strlen(willMessage)),
			connectError);
}

void *MqttClientImpl::inboundEventThread(void *arg)
{
	((MqttClientImpl *)arg)->processInboundEvents();
	return (NULL);
}

void MqttClientImpl::processInboundEvents()
{
	LOGT("In processInvoundEvents()\n");

	_processingEvents = true;

	fd_set readFileDescriptorSet;
	struct timeval timeVal;
	int numFileDescriptors = 0;
	int rc = 0;
	ssize_t packetLength = 0;
	int flags = 0;

	while (_processingEvents) {
		LOGT("Processing events\n");

		_uint64 t = currentMillis();

		if (t - _lastActivity > KEEPALIVE) {
			write(MQTT_PINGREQ, NULL, 0);
			_lastActivity = t;
		}
		FD_ZERO(&readFileDescriptorSet);
	    FD_SET(_sockfd, &readFileDescriptorSet);
	    FD_SET(_pfd[0], &readFileDescriptorSet);
	    numFileDescriptors = (_sockfd > _pfd[0]) ? _sockfd : _pfd[0];
	    timeVal.tv_sec = KEEPALIVE_SECONDS;
		timeVal.tv_usec = 0;
		rc = select(numFileDescriptors+1, &readFileDescriptorSet, NULL, NULL, &timeVal);
		packetLength = 0;

		LOGT("select rc=%d\n", rc);

		if (rc > 0) {
			LOGT("select() filedescriptor ready rc=%d\n", rc);
			/*
			 * Bit of a hack since we don't seem to have pselect() :-(
			 */
		    if (FD_ISSET(_pfd[0], &readFileDescriptorSet)) {
				LOGT("select() data on pipe -- terminate\n");
		        for (;;) {
			    	char ch;
		            if (read(_pfd[0], &ch, 1) == -1) {
		                if (errno == EAGAIN) {
		                	break;
		                }
		            }
		        }
		    }
	        if(FD_ISSET(_sockfd, &readFileDescriptorSet)) {
				LOGT("select() data on socket -- read it\n");
				LOGT("About to read packet\n");
	    		packetLength = readPacket();
				LOGT("Length read = %d\n", packetLength);
	        }
		} else if (rc == 0) {
			LOGT("select() timer popped\n");
			continue;
		} else if (rc < 0) {
			LOGT("select() error rc=%d, errno=%d\n", rc, errno);
			continue;
		}

        if (packetLength > 0) {
        	uint8_t packetType = _inBuffer[0] & 0xF0;
        	if (packetType == MQTT_PUBLISH) {
				LOGT("Got MQTT_PUBLISH\n");

				ssize_t topicOffset = payloadOffsetInPacket(_inBuffer, packetLength);
				ssize_t topicLength = (_inBuffer[topicOffset] << 8) + _inBuffer[topicOffset+1];
				ssize_t payloadLength = packetLength-(topicOffset + STRING_LENGTH_SIZE)-topicLength;

				uint8_t *topic = _inBuffer + topicOffset + STRING_LENGTH_SIZE;
				uint8_t *payload = topic + topicLength;

				LOGT("Packet length=%d\n", packetLength);
				LOGT("Topic length=%d\n", topicLength);
				LOGT("Payload Length=%d\n", payloadLength);

	    		_lastActivity = currentMillis();

	    		if (_mqttCallbacks.subscription) _mqttCallbacks.subscription(topic, topicLength, payload, payloadLength);

        	} else if (packetType == MQTT_PUBACK) {
				LOGT("Got MQTT_PUBACK\n");
				uint16_t messageId = (_inBuffer[2] << 8) + _inBuffer[3];

				LOGT("Packet length=%d\n", packetLength);
				LOGT("Message Id=%d\n", (messageId));

	    		_lastActivity = currentMillis();

	    		if (_mqttCallbacks.puback) _mqttCallbacks.puback(messageId);

        	} else if (packetType == MQTT_PUBREC) {
				LOGT("Got MQTT_PUBREC\n");
				uint16_t messageId = (_inBuffer[2] << 8) + _inBuffer[3];

				LOGT("Packet length=%d\n", packetLength);
				LOGT("Message Id=%d\n", (messageId));

    			ssize_t length = 2;
				_outBuffer[0] = _inBuffer[2];
				_outBuffer[1] = _inBuffer[3];

				uint8_t header = MQTT_PUBREL;
				header |= (MQTT_QOS_AT_LEAST_ONCE << 1);
				write(header, _outBuffer, length);

	    		_lastActivity = currentMillis();

	    		if (_mqttCallbacks.pubrec) _mqttCallbacks.pubrec(messageId);

        	} else if (packetType == MQTT_PUBCOMP) {
				LOGT("Got MQTT_PUBCOMP\n");
				uint16_t messageId = (_inBuffer[2] << 8) + _inBuffer[3];

				LOGT("Packet length=%d\n", packetLength);
				LOGT("Message Id=%d\n", (messageId));

	    		_lastActivity = currentMillis();

	    		if (_mqttCallbacks.pubcomp) _mqttCallbacks.pubcomp(messageId);

        	} else if (packetType == MQTT_PINGREQ) {
				LOGT("Got MQTT_PINGREQ\n");
    			write(MQTT_PINGRESP, NULL, 0);
	    		_lastActivity = currentMillis();

        	} else if (packetType == MQTT_PINGRESP) {
				LOGT("Got MQTT_PINGRESP\n");
	    		_lastActivity = currentMillis();

        	} else if (packetType == MQTT_SUBACK) {
				LOGT("Got MQTT_SUBACK\n");

				ssize_t msgIdOffset = payloadOffsetInPacket(_inBuffer, packetLength);
				uint16_t messageId = (_inBuffer[msgIdOffset] << 8) + _inBuffer[msgIdOffset+1];

				LOGT("Packet length=%d\n", packetLength);
				LOGT("Message Id=%d\n", (messageId));

	    		if (_mqttCallbacks.suback) _mqttCallbacks.suback(messageId);

	    		_lastActivity = currentMillis();

        	} else if (packetType == MQTT_UNSUBACK) {
				LOGT("Got MQTT_UNSUBACK\n");

				ssize_t msgIdOffset = payloadOffsetInPacket(_inBuffer, packetLength);
				uint16_t messageId = (_inBuffer[msgIdOffset] << 8) + _inBuffer[msgIdOffset+1];

				LOGT("Packet length=%d\n", packetLength);
				LOGT("Message Id=%d\n", messageId);

	    		if (_mqttCallbacks.unsuback) _mqttCallbacks.unsuback(messageId);

	    		_lastActivity = currentMillis();

        	} else {
				LOGT("_inBuffer[0]=%x\n", _inBuffer[0]);
	    		_lastActivity = currentMillis();
			}
        } else if (packetLength == 0) {
        	if (!_disconnectInProgress) {
            	LOGT("Hmm looks like remote end has disconnected\n");
            	_processingEvents = false;
            	_remoteDisconnectedUs = true;
            	_connected = false;
        		if (_mqttCallbacks.disconnected) _mqttCallbacks.disconnected();
        	} else {
            	LOGT("Looks like a disconnect is in progress - let it work its way through\n");
        	}
        }
	}

	LOGT("Thread ending\n");

	/*
	 * Flush the read side of pipe and close it
	 */
    LOGT("Flushing read side of pipe\n");
	for (;;) {
    	char ch;
        LOGT("Attempting read\n");
        if (read(_pfd[0], &ch, 1) == -1) {
            LOGT("Read got -1\n");
            fprintf(stderr,"errno=%d\n",errno);
            if (errno == EAGAIN) {
            	break;
            }
        }
    }
	::close(_pfd[0]);

	LOGT("Thread exiting\n");
}

ssize_t MqttClientImpl::writeString(const char *text, uint8_t *buf, ssize_t pos)
{
	return (writeBytes((uint8_t *)text, ((text == NULL) ? 0 : strlen(text)), buf, pos));
}

ssize_t MqttClientImpl::writeBytes(const uint8_t *data, ssize_t dataLength, uint8_t *buf, ssize_t pos)
{
	if (dataLength > ((1<<(STRING_LENGTH_SIZE*8))-1))  return -1;
	if ((pos+STRING_LENGTH_SIZE+dataLength) > MAX_PACKET_SIZE) return -2;

	ssize_t i = 0;
	pos += 2;
	while (i < dataLength) buf[pos++] = data[i++];
	buf[pos-i-2] = ( i >> 8 ) & 0xff;
	buf[pos-i-1] = i & 0xff;

	return pos;
}

int MqttClientImpl::write(const uint8_t header, const uint8_t *buf, ssize_t length)
{
	uint8_t encodedLength[MAX_ENCODED_LENGTH_SIZE];
	uint8_t digit;
	int digitIndex = 0;

	TRACE("XXXX >>> %x /", header);

	ssize_t workLength = length;
	do {
		digit = workLength % 128;
		workLength /= 128;
		if ( workLength > 0 ) {
			digit |= 0x80;
		}
		encodedLength[digitIndex++] = digit;
		TRACE(" %x", digit);
	} while ( workLength > 0 );

	TRACE(" /");
	for (int i = 0; i < length; i++) TRACE(" %x", buf[i]);
	TRACE("\n");

	pthread_mutex_lock(&_outboundSocketMutex);
	::send(_sockfd, &header, sizeof(header), 0);
	::send(_sockfd, encodedLength, digitIndex, 0);
	::send(_sockfd, buf, length, 0);
	pthread_mutex_unlock(&_outboundSocketMutex);

	return 0;
}

ssize_t MqttClientImpl::readPacket() {
	ssize_t len = 0;
	uint8_t multiplier = 1;
	ssize_t length = 0;
	ssize_t readLength;
	uint8_t readData;

	TRACE("XXXX <<<");
	readLength = readByte(&readData);
	if (readLength == 0) {
		TRACE("\n");
		return len;
	}
	_inBuffer[len++] = readData;
	TRACE(" %x", _inBuffer[len-1]);

	do {
		readLength = readByte(&readData);
		_inBuffer[len++] = readData;
		TRACE(" %x", _inBuffer[len-1]);
		length += (readData & 127) * multiplier;
		multiplier *= 128;
	} while ((readData & 128) != 0);

	bool overrun = false;

	for (int i = 0; i<length; i++) {
		if (len < MAX_PACKET_SIZE) {
			readLength = readByte(&readData);
			_inBuffer[len++] = readData;
			TRACE(" %x", _inBuffer[len-1]);
		} else {
			overrun = true;
			readLength = readByte(&readData);
		}
	}

	TRACE("\n");
	return (overrun ? 0 : len);
}

ssize_t MqttClientImpl::readByte(uint8_t *data) {
	ssize_t length;
	length = ::recv(_sockfd, data, sizeof(uint8_t), 0);
	return length;
}

void MqttClientImpl::disconnect(MqttError &disconnectError)
{
	int rc;

	LOG("In disconnect()\n");

	if (!_remoteDisconnectedUs) {
		LOG("Normal disconnection\n");
		if (!isConnected()) {
			LOG("Not connected\n");
	    	disconnectError.set("Error disconnecting since not already connected");
	    	return;
		}
		_processingEvents = false;
		_disconnectInProgress = true;

		/*
		 * Bit of a hack since we don't have pselect() :-(
		 * write to a looped back pipe being select()ed by thread
		 */
		LOG("Writing to pipe\n");
		ssize_t rc = ::write(_pfd[1], "x", 1);
		LOG("Written to pipe rc=%d\n", rc);

		LOG("Waiting until thread ends\n");
		rc = pthread_join(_inboundEventThread, NULL);
		LOG("pthread_join rc = %d\n", rc);

		write(MQTT_DISCONNECT, NULL, 0);

	} else {
		LOG("Remote disconnected us\n");
		LOG("Waiting until thread ends\n");
		rc = pthread_join(_inboundEventThread, NULL);
		LOG("pthread_join rc = %d\n", rc);
	}
	LOG("Thread ended\n");
	if (::close(_pfd[1]) < 0) {
    	disconnectError.set("Error closing write side of pipe");
    }
	if (::close(_sockfd) < 0) {
    	disconnectError.set("Error disconnecting from remote server");
    }
    disconnectError.set("Disconnected", MqttError::PASS_CODE);
    _sockfd = -1;
    _connected = false;
    _remoteDisconnectedUs = false;
	LOG("Exiting disconnect()\n");
}

_uint64 MqttClientImpl::currentMillis()
{
	_uint64 millis = 0;
	struct timeb time;

	if (!ftime(&time)) {
		millis = (_uint64)time.time * (_uint64)1000 + (_uint64)time.millitm;
	}
	return millis;
}

void MqttClientImpl::publish(const char *topic,
		                     const char *payload,
		                     MqttError &publishError)
{
   publish(topic,(uint8_t *)payload, ((payload == NULL) ? 0 : strlen(payload)), publishError);
}

void MqttClientImpl::publish(const char *topic,
		                     const uint8_t *payload,
		                     ssize_t plength,
		                     MqttError &publishError)
{
   publish(topic, payload, plength, 0, publishError);
}

void MqttClientImpl::publish(const char *topic,
		                     const uint8_t *payload, ssize_t plength,
		                     uint8_t retained,
		                     MqttError &publishError)
{
	publish((uint8_t *)topic, ((topic == NULL) ? 0 : strlen(topic)), payload, plength, retained, publishError);
}

void MqttClientImpl::publish(const uint8_t *topic, ssize_t tlength,
		                     const uint8_t *payload, ssize_t plength,
		                     MqttError &publishError)
{
	publish(topic, tlength, payload, plength, 0, publishError);
}

void MqttClientImpl::publish(const uint8_t *topic, ssize_t tlength,
		                     const uint8_t *payload, ssize_t plength,
		                     uint8_t retained,
		                     MqttError &publishError)
{
	ssize_t length = writeBytes(topic, tlength, _outBuffer, 0);
	if (length < 0) {
		publishError.set("Failure to write topic to buffer");
		return;
	}
	int i;
	for (i=0; i<plength; i++) {
		_outBuffer[length++] = payload[i];
	}
	uint8_t header = MQTT_PUBLISH;
	if (retained != 0) {
		header |= 1;
	}
	if (_qosLevel != MQTT_QOS_AT_MOST_ONCE) {
		header |= _qosLevel;
		_nextMsgId++;
		uint8_t messageId[2];
		messageId[0] = _nextMsgId >> 8;
		messageId[1] = _nextMsgId - (messageId[0]<<8);
		length = writeBytes(messageId, sizeof(messageId), _outBuffer, length);

		if (length < 0) {
			publishError.set("Failure to write message Id to buffer");
			return;
		}
	}
	write(header, _outBuffer, length);

	if (_mqttCallbacks.publish) _mqttCallbacks.publish(topic, tlength, payload, plength, retained);

	publishError.set("Data published", MqttError::PASS_CODE);
}

void MqttClientImpl::subscribe(const char *topic, MqttError &subscribeError)
{
    subscribe((uint8_t *)topic, ((topic == NULL) ? 0 : strlen(topic)), subscribeError);
}

void MqttClientImpl::unSubscribe(const char *topic, MqttError &unSubscribeError)
{
    unSubscribe((uint8_t *)topic, ((topic == NULL) ? 0 : strlen(topic)), unSubscribeError);
}

void MqttClientImpl::subscribe(const uint8_t *topic, ssize_t tlength, MqttError &subscribeError)
{
	ssize_t length = 2;
    if (isConnected()) {
		_nextMsgId++;
		_outBuffer[0] = _nextMsgId >> 8;
		_outBuffer[1] = _nextMsgId - (_outBuffer[0]<<8);
		length = writeBytes(topic, tlength, _outBuffer, length);
		if (length < 0) {
			subscribeError.set("Failure to write data to socket");
			return;
		}
		_outBuffer[length++] = 0;
		write(MQTT_SUBSCRIBE, _outBuffer, length);
    }
    subscribeError.set("Subscribed to topic", MqttError::PASS_CODE);
}

void MqttClientImpl::unSubscribe(const uint8_t *topic, ssize_t tlength, MqttError &unSubscribeError)
{
	ssize_t length = 2;
    if (isConnected()) {
		_nextMsgId++;
		_outBuffer[0] = _nextMsgId >> 8;
		_outBuffer[1] = _nextMsgId - (_outBuffer[0]<<8);
		length = writeBytes(topic, tlength, _outBuffer, length);
		if (length < 0) {
			unSubscribeError.set("Failure to write data to socket");
			return;
		}
		write(MQTT_UNSUBSCRIBE, _outBuffer, length);
    }
    unSubscribeError.set("Unsubscribed from topic", MqttError::PASS_CODE);
}

bool MqttClientImpl::isConnected()
{
	return _connected;
}

void MqttClientImpl::setHostname(const char*hostName)
{
	strncpy(_hostName, hostName, _hostNameSize-1);
}

void MqttClientImpl::setPortNumber(const char *portNumber)
{
	strncpy(_portNumber, portNumber, _portNumberSize-1);
}

void MqttClientImpl::setQos(const MqttClient::QosLevel qos)
{
	switch (qos) {
		case MqttClient::AtMostOnce:
			_qosLevel = MQTT_QOS_AT_MOST_ONCE;
			break;
		case MqttClient::AtLeastOnce:
			_qosLevel = MQTT_QOS_AT_LEAST_ONCE;
			break;
		case MqttClient::ExactlyOnce:
			_qosLevel = MQTT_QOS_EXACTLY_ONCE;
			break;
		default:
			_qosLevel = MQTT_QOS_AT_MOST_ONCE;
			break;
	}
}

MqttClient::QosLevel MqttClientImpl::getQos()
{
	MqttClient::QosLevel qosLevel;

	switch (_qosLevel) {
		case MQTT_QOS_AT_MOST_ONCE:
			qosLevel = MqttClient::AtMostOnce;
			break;
		case MQTT_QOS_AT_LEAST_ONCE:
			qosLevel = MqttClient::AtLeastOnce;
			break;
		case MQTT_QOS_EXACTLY_ONCE:
			qosLevel = MqttClient::ExactlyOnce;
			break;
		default:
			qosLevel = MqttClient::Undefined;
			break;
	}
	return qosLevel;
}

ssize_t MqttClientImpl::payloadOffsetInPacket(const uint8_t *buffer, ssize_t packetLength)
{
	uint8_t data;
	ssize_t maxPayloadOffset = (packetLength < (MAX_ENCODED_LENGTH_SIZE+1)) ? packetLength : (MAX_ENCODED_LENGTH_SIZE+1) ;
	ssize_t payloadOffset = 1;
	do {
		data = buffer[payloadOffset++];
	} while (((data & 128) != 0) && (payloadOffset < maxPayloadOffset));
	return payloadOffset;
}

void MqttClientImpl::getApiVersion(char *version) {
    if (API_VERSION != NULL && version != NULL) {
        strcpy(version,API_VERSION);
    } else {
    }
}


} /* namespace mqtt_client */
