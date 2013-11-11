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

#ifndef HRTYPES_HPP_
#define HRTYPES_HPP_
#include <QQueue>
#include <QVariant>
#include <stdint.h>

// for humans only! (uint8_t for heart rates <= 255. This app is not designed for HR sensors attached to humming birds or small, frightened rodents!
typedef struct {
	unsigned int timestamp;
	bool hr_data_format_8;
	bool sensor_contact_detected;
	bool sensor_contact_feature_supported;
	bool energy_expended_feature_supported;
	bool rr_interval_data_present;
	uint8_t hr_measurement;
	uint16_t energy_expended;
	uint8_t num_rr_intervals;
	uint16_t* rr_intervals;
} hr_notification_t;

typedef struct {
	unsigned int session_id;
	unsigned int timestamp;
	QVariant session_name;
	QQueue<QVariant> hr_measurements;
} hr_session_t;


#endif /* HRTYPES_HPP_ */
