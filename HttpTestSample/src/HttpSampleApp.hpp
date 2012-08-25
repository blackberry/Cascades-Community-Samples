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

#ifndef HTTPSAMPLEAPP_H
#define HTTPSAMPLEAPP_H

#include <QtCore/QObject>

class HttpSampleApp : public QObject
{
    Q_OBJECT
public:
    HttpSampleApp();

    /*
     * Invokable functions that we can call from QML.
     * This function saves the body of the http post message typed by the user
     */
    Q_INVOKABLE void setPostBody(QString body)
    {
        m_postBody = body;
    }

    /*
     * Invokable functions that we can call from QML.
     * This function retrieves the body of the http post message
     */
    Q_INVOKABLE QString getPostBody() const
    {
        return m_postBody;
    }

    /*
     * Invokable functions that we can call from QML.
     * This function checks if http is being used
     */
    Q_INVOKABLE bool isUsingHttp() const;

    /*
     * Invokable functions that we can call from QML.
     * This function checks if https is being used
     */
    Q_INVOKABLE bool isUsingHttps() const;

    /*
     * Invokable functions that we can call from QML.
     * This function toggles the https flag depending on the value parameter
     *
     * @param value - true to turn on https, false to disable https and use http
     */
    Q_INVOKABLE void useHttps(bool value);

private:
    QString m_postBody; //stores the post body for page transitions
};

#endif // ifndef HTTPSAMPLEAPP_H
