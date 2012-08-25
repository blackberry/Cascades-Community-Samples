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

#ifndef LOADIMAGE_HPP_
#define LOADIMAGE_HPP_

#include <QThread>
#include <QImage>
#include <QByteArray>

/*
 * This class retrieves an image from the web, then converts the binary
 * data into a QImage before sending a complete signal
 */
class LoadImage : public QObject
{
    Q_OBJECT
public:
    /*
     * Ctor & Dtor
     */
    LoadImage(QObject* parent = 0);
    virtual ~LoadImage();

    /*
     * Loads an image from the specified url
     */
    void load(QString imageUrl);

    /*
     * Returns the QImage
     */
    QImage getImage() const
    {
        return m_image;
    }

public slots:
    /*
     * Response handler for the network operation
     */
    void onReplyFinished();

    /**
     *  This is the method to be execute in the thread which constructs the QImage
     *  Object from the network response.
     *
     */
    void startProcess();


signals:
    /*
     * Signal emitted after downloading and processing is complete.
     */
    void complete(QString imageUrl, bool success, QString response);

    /*
     * Signal emitted after this object and its thread is no longer needed.
     */
    void finishThread();

protected:

    /**
     * void startThread();
     *
     * start the executing thread
     */
    void startThread();

    /**
     *
     * void prepareThread(QThread* executeThread);
     *
     * a helper function to prepare the executing thread.
     *
     * @param executeThread, the thread which needs to be initialized.
     */
    void prepareThread(QThread* executeThread);


protected:
    QString m_imageUrl;
    QByteArray m_data;
    QImage m_image;
    QThread* m_executeThread; // the thread which actually execute the logic of converting image data.
};

#endif /* LOADIMAGE_HPP_ */
