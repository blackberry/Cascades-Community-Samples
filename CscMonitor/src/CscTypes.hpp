/* Copyright (c) 2013 Research In Motion Limited.
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

#ifndef CSCTYPES_HPP_
#define CSCTYPES_HPP_
#include <QQueue>
#include <QVariant>
#include <stdint.h>

/*
 * Either the Wheel Revolution Data (containing the Cumulative Wheel Revolutions and Last Wheel Event Time fields) or the Crank Revolution Data
 * (containing the Cumulative Crank Revolutions and Last Crank Event Time fields) or both shall be present in the CSC Measurement characteristic.
 */

typedef struct {
	unsigned int timestamp;
	bool wheel_revolution_data_present;
	bool crank_revolution_data_present;
	uint32_t cumulative_wheel_revolutions;
	uint16_t last_wheel_event_time;
	uint32_t cumulative_crank_revolutions;
	uint16_t last_crank_event_time;
} csc_notification_t;

typedef struct {
	unsigned int timestamp;
	bool wheel_revolution_data_present;
	bool crank_revolution_data_present;
	uint16_t wheel_revolutions_per_minute;
	uint16_t crank_revolutions_per_minute;
} csc_values_t;

typedef struct {
	unsigned int session_id;
	unsigned int timestamp;
	QVariant session_name;
	QQueue<QVariant> csc_measurements;
} csc_session_t;

#endif /* CSCTYPES_HPP_ */
