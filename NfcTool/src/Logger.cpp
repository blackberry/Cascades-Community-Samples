/* Copyright (c) 2012 Research In Motion Limited.
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
#include "Logger.hpp"

Logger* Logger::_instance;
QString colour_codes[2];

Logger::Logger() {
	qDebug() << "XXXX Constructing Logger";
	_data_model = new AlternatingListDataModel;
	colour_codes << "#00ff00" << "#00ffff";
	current_colour = 0;
	qDebug() << "XXXX Done constructing Logger";
}

Logger::~Logger() {
	qDebug() << "XXXX SendVcard destructor";
	_instance = 0;
}

Logger* Logger::getInstance() {
	if (_instance == 0) {
		_instance = new Logger;
	}
	return _instance;
}

AlternatingListDataModel* Logger::getDataModel() {
	return _data_model;
}

void Logger::log(const QString& itemLabel, const QString& desc) {
	if (_data_model != 0) {
		_data_model->append(colour_codes[current_colour], itemLabel, desc);
		switch (current_colour) {
		case 0:
			current_colour = 1;
			break;
		case 1:
			current_colour = 0;
			break;
		}
	}
}

void Logger::clearLog() {
	if (_data_model != 0) {
		_data_model->reset();
	}
}
