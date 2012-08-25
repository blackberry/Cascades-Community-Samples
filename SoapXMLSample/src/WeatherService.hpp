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

#ifndef WEATHERSERVICE_HPP_
#define WEATHERSERVICE_HPP_

#include <QtCore/QObject>
#include "qtsoap/qtsoap.h"

class WeatherService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString temperature READ getTemperature)
    Q_PROPERTY(QString description READ getDescription)
    Q_PROPERTY(bool succeeded READ getSucceeded)
    Q_PROPERTY(QString error READ getError)

public:
    WeatherService(QObject* parent = 0);
    virtual ~WeatherService();

    /*
     * Retrieves the temperature from the previous request
     */
    QString getTemperature() const;

    /*
     * Retrieves the description from the previous request
     */
    QString getDescription() const;

    /*
     * Returns whether the previous request succeeded or failed
     */
    bool getSucceeded() const;

    /*
     * Retrieves the error string from the previous request if one occurred
     * Check getSucceeded() first
     */
    QString getError() const;

public slots:
	/*
	 * Retrieves the weather information for the specified zip code (US only)
	 */
    void getWeather(QString zipCode);

    /*
     * Handler for SOAP request response
     */
    void onServiceResponse();

signals:
	/*
	 * The complete signal is emitted when the request to the webservice is done
	 */
    void complete();

private:
    QString m_temperature;	//temperature in F
    QString m_description;	//description of the weather
    QString m_error;		//error string if one occurred
    bool m_succeeded;		//flag to determine whether request succeeded
    QtSoapHttpTransport m_soap;	//soap transport
};

#endif /* WEATHERSERVICE_HPP_ */
