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

#include "WeatherService.hpp"

/**
 * WeatherService
 *
 * In this class you will learn the following:
 * -- Interface with a SOAP C++ library
 */

/**
 * WeatherService::WeatherService(QObject* parent)
 *
 * Initialize member variables and connect the SOAP response signal to our onServiceResponse() slot
 */
WeatherService::WeatherService(QObject* parent)
    : QObject(parent)
    , m_temperature("")
    , m_description("")
    , m_error("")
    , m_succeeded(false)
{
    connect(&m_soap, SIGNAL(responseReady()), SLOT(onServiceResponse()));
}

WeatherService::~WeatherService()
{
	// void
}

/**
 * WeatherService::getTemperature()
 *
 * Return QString, the currently set temperature for a selected city
 */
QString WeatherService::getTemperature() const
{
    return m_temperature;
}

/**
 * WeatherService::getDescription()
 *
 * Return QString, the currently set description
 */
QString WeatherService::getDescription() const
{
    return m_description;
}

/**
 * WeatherService::getSucceeded()
 *
 * Return bool, whether the SOAP query was successful or not
 */
bool WeatherService::getSucceeded() const
{
    return m_succeeded;
}

/**
 * WeatherService::getError()
 *
 * Return QString, the error if the SOAP query wasn't successful
 */
QString WeatherService::getError() const
{
    return m_error;
}

/**
 * WeatherService::getWeather(QString zipCode)
 *
 * Construct and submit a SOAP request for a specific location
 */
void WeatherService::getWeather(QString zipCode)
{
    m_succeeded = false;
    m_soap.setAction("http://ws.cdyne.com/WeatherWS/GetCityWeatherByZIP");
    m_soap.setHost("wsf.cdyne.com");

    QtSoapMessage request;
    request.setMethod(QtSoapQName("GetCityWeatherByZIP", "http://ws.cdyne.com/WeatherWS/"));
    request.addMethodArgument("ZIP", "http://ws.cdyne.com/WeatherWS/", zipCode);

    // Submit the method request to the web service.
    m_soap.submitRequest(request, "/WeatherWS/Weather.asmx");
}

/**
 * WeatherService::onServiceResponse()
 *
 * SLOT
 * Called when the SOAP interface returns a response
 * Retrieves the SOAP result, sets member variables and
 * emits a signal indicating the request is complete
 */
void WeatherService::onServiceResponse()
{
    // Get the response, check for error.
    const QtSoapMessage& resp = m_soap.getResponse();
    if (resp.isFault())
    {
        m_error = "Query failed: " + resp.faultString().value().toString();
        qDebug() << m_error;
        emit complete();
        return;
    }

    // Extract the return value from this method response, check for
    // errors.
    const QtSoapType& res = resp.returnValue();
    if (!res.isValid())
    {
        m_error = "Query failed: invalid return value";
        qDebug() << m_error;
        emit complete();
        return;
    }

    qDebug() << "soap response: " << resp.toXmlString(0);

    if (QString::compare("true", res["Success"].toString(), Qt::CaseInsensitive) == 0)
    {
        m_succeeded = true;
        m_error = "";
        m_temperature = res["Temperature"].toString() + " F";
        m_description = res["Description"].toString();
    }
    else
    {
        m_succeeded = false;
        m_error = res["ResponseText"].toString();
    }
    emit complete();
}
