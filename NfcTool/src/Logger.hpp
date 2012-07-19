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
#ifndef LOGGER_H
#define LOGGER_H

#include <QtCore/QString>
#include <QtCore/QDebug>
#include "alternatinglistdatamodel.hpp"

class Logger {

private:
	Logger();
	virtual ~Logger();
	static Logger* _instance;
	AlternatingListDataModel* _data_model;

public:
	static Logger* getInstance();
	void log(const QString& title, const QString& itemLabel,
			const QString& desc);
	void clearLog();
	AlternatingListDataModel* getDataModel();

};

#endif // ifndef LOGGER_H
