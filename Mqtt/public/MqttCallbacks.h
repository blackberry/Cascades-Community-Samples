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

#ifndef MQTTCALLBACKS_H_
#define MQTTCALLBACKS_H_

#include <stdint.h>
#include <sys/types.h>

namespace mqtt_client {
typedef void (*mqttSubscription_cb) (const uint8_t *topic, ssize_t tlength, const uint8_t *payload, ssize_t length);
typedef void (*mqttDisconnected_cb) ();
typedef void (*mqttPublish_cb)      (const uint8_t *topic, ssize_t tlength, const uint8_t *payload, ssize_t plength, uint8_t retained);
typedef void (*mqttPubAck_cb)       (uint16_t messageId);
typedef void (*mqttPubRec_cb)       (uint16_t messageId);
typedef void (*mqttPubComp_cb)      (uint16_t messageId);
typedef void (*mqttSubAck_cb)       (uint16_t messageId);
typedef void (*mqttUnSubAck_cb)     (uint16_t messageId);

typedef struct {
	mqttSubscription_cb subscription;
	mqttDisconnected_cb disconnected;
	mqttPublish_cb      publish;
	mqttPubAck_cb       puback;
	mqttPubRec_cb       pubrec;
	mqttPubComp_cb      pubcomp;
	mqttSubAck_cb       suback;
	mqttUnSubAck_cb     unsuback;
} mqttCallbacks_t;
} /* namespace mqtt_client */

#endif /* MQTTCALLBACKS_H_ */
