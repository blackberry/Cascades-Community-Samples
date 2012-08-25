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

#ifndef SENSORSTHREAD_HPP_
#define SENSORSTHREAD_HPP_

#include <QThread>
#include <QMutex>
#include <QVariant>
#include <QWaitCondition>

class SensorsThread : public QObject
{
	Q_OBJECT
public:
    SensorsThread(QObject* parent = 0);
    virtual ~SensorsThread();

    /**
     * void start()
     *
     * start the sensor in its own thread.
     */
    Q_INVOKABLE void start();

signals:
    /*
     * Emits a signal from the thread indicating the current sensor values
     *
     * FixMe: int celciusTemperature,
     *          int illuminance
     *   those two parameters shall be "float", however, the current QML engine shipped with 10.0.6.386
     *   does not accept that type.
     */
    void pulse(int celciusTemperature,
               int illuminance,
               QVariant xyzMagnet,
               QVariant xyzAccel,
               QVariant xyzGyro);

    /*
     * Emits a signal when the thread starts and reports which sensors are available
     */
    void started(bool temperature, bool light, bool magnetometer, bool accelerometer, bool gyroscope);

    /*
     * Emits a signal when the thread is no longer available.
     */
    void finished();

public slots:

	/*
	 * the method is to be invoked after the executing thread is ready
	 */
	void startProcess();

private:
    /*
     * Called by run() to obtain bps sensor events and emit pulse signals.
     */
    void bpsEventLoop();

    /**
     *
     * void prepareThread(QThread* executeThread);
     *
     * helper function to prepare  the given thread.
     *
     * @param executeThread, the executing thread
     */
    void prepareThread(QThread* executeThread);

protected:
    QMutex m_mutex; //used to guard the m_quit flag.
    bool m_quit; // controls lifetime of bpsEventLoop() function

    QThread* m_executeThread; // the thread which actually check the sensors.
    QWaitCondition m_condition; //wait condition for pausing the run loop


};

#endif /* SENSORSTHREAD_HPP_ */
