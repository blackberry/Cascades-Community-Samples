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

#ifndef APP_H
#define APP_H

#include <QtCore/QObject>
#include <QMap>
#include <bb/cascades/AbstractPane>

class App : public QObject
{
    Q_OBJECT

public:
    App();

    /*
     * Invoked by the QML to load an image from a url
     * @param url - the url to load
     * @param id - the index of the container responsible for displaying the image
     */
    Q_INVOKABLE void loadImage(QString const& url, int id);

public slots:

    /*
     * Handler for photo image retrieval
     */
    void onImageLoaded(QString url, bool success, QString response);

signals:

	 /**
	  * Emit signal when finish using a data from the image thread.
	  */
     void finishUpdateImage();

private:
    bb::cascades::AbstractPane* m_root;
    QMap<QString, int> m_imageMap; //maps the url to an id number in qml
};

#endif // ifndef APP_H
