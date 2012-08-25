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

#include <bps/sensor.h>
#include <errno.h>
#include <QDebug>
#include <QList>

#include "SensorsThread.hpp"

/**
 * SensorsThread
 *
 * In this class you will learn the following:
 * -- How to interface with BPS (Blackberry Platform Service) to read the following sensors:
 * 		temperature (not supported on Dev Alpha device)
 * 		light
 * 		magnometer
 * 		accelerometer
 * 		gyrometer
 */

namespace
{
/*
 * For the specified sensor, calls sensor_is_supported, sensor_set_rate, sensor_set_skip_duplicates and
 * sensor_request_events in its constructor and sensor_stop_events on destruction. Can be cast to bool
 * to determine if monitoring is enabled (i.e. the sensor is supported).
 */

class SensorMarshall
{
public:
    SensorMarshall(const sensor_type_t sensor,
                   const int rate,
                   const bool report_duplicates = false)
        : m_sensor(sensor)
    {
        m_supported = sensor_is_supported(m_sensor);
        if (m_supported)
        {
            if (BPS_SUCCESS != sensor_request_events(m_sensor))
            {
                qDebug() << "SensorMarshall::SensorMarshall(), sensor_request_events failed with errno " << errno;
                throw errno;
            }

            if (BPS_SUCCESS != sensor_set_rate(m_sensor, rate))
            {
                qDebug() << "SensorMarshall::SensorMarshall(), sensor_set_rate failed with errno " << errno;
                throw errno;
            }

            if (BPS_SUCCESS != sensor_set_skip_duplicates(m_sensor, report_duplicates))
            {
                qDebug() << "SensorMarshall::SensorMarshall(), sensor_set_skip_duplicates failed with errno " << errno;
                throw errno;
            }
        }
        else
        {
            qDebug() << "SensorMarshall::SensorMarshall(), " << sensor << " sensor not supported.";
        }
    }

    ~SensorMarshall()
    {
        if (m_supported)
        {
            sensor_stop_events(m_sensor);
        }
    }

    operator bool()
    {
        return m_supported;
    }

private:
    const sensor_type_t m_sensor;
    bool m_supported;
};

/*
 * Creates a QList from the data in the array argument.
 */
template <typename TQListItem, typename TArrayItem> QList<TQListItem> qlistFromArray(TArrayItem array[], const unsigned int len)
{
    QList<TQListItem> list;

    for (unsigned int i = 0; i < len; i++)
    {
        list.append(TQListItem(array[i]));
    }

    return list;
}

} // end anonymous namespace

SensorsThread::SensorsThread(QObject* parent)
    : QObject(parent)
    , m_quit(false)
	, m_executeThread(NULL)
{
    // void
}

SensorsThread::~SensorsThread()
{
    qDebug() << "SensorsThread dtor called. Will wait for thread run() to finish.";


    {
    	QMutexLocker locker(&m_mutex);
    	m_quit = true;
    	m_condition.wakeOne();
    }
    if(m_executeThread)
    	m_executeThread->wait();

    //no need to explicitly delete m_executeThread

    qDebug() << "SensorsThread dtor finished";

}
/**
 * void start()
 *
 * start the sensor in its own thread.
 */
void SensorsThread::start()
{
	if(m_executeThread!=NULL)
		return;

	m_executeThread = new QThread;

	prepareThread(m_executeThread);
	m_executeThread->start();

}

/*
 * the method is to be invoked after the executing thread is ready
 */
void SensorsThread::startProcess()
{
    qDebug() << "SensorsThread run() started";

    // initialize blackberry platform services for this thread
    bps_initialize();

    // go into the bps event loop
    try {
    	bpsEventLoop();
    }
    catch (int err) {
    	qDebug() << "SensorsThread::run(), bpsEventLoop threw error " << err;
    }
    catch (...) {
    	qDebug() << "SensorsThread::run(), bpsEventLoop threw an unknown exception";
    }

    // shutdown blackberry platform services for this thread
    bps_shutdown();

    qDebug() << "SensorsThread run() finished.";
}

/**
 * SensorsThread::bpsEventLoop()
 *
 * Start the sensors on the device and read from them in a loop until the app exits
 */
void SensorsThread::bpsEventLoop()
{
    static const int microseconds_sensor_rate = 100000; // 100ms
    static const int sensor_domain = sensor_get_domain();

    // try to start the sensors
    SensorMarshall temp(SENSOR_TYPE_TEMPERATURE, microseconds_sensor_rate);
    SensorMarshall light(SENSOR_TYPE_LIGHT, microseconds_sensor_rate);
    SensorMarshall magnet(SENSOR_TYPE_MAGNETOMETER, microseconds_sensor_rate);
    SensorMarshall accel(SENSOR_TYPE_ACCELEROMETER, microseconds_sensor_rate);
    SensorMarshall gyro(SENSOR_TYPE_GYROSCOPE, microseconds_sensor_rate);

    // alert slots that we have started monitoring sensors
    emit started(temp, light, magnet, accel, gyro);

    // prepare values
    float celciusTemperature = 0.0f;
    float illuminance = 0.0f;
    float xyzAccel[3] = { 0.0f, 0.0f, 0.0f };
    float xyzMagnet[3] = { 0.0f, 0.0f, 0.0f };
    float xyzGyro[3] = { 0.0f, 0.0f, 0.0f };

    // event loop, do it until m_quit is true
    forever
    {
    	QMutexLocker locker(&m_mutex);

		if (m_quit)
		{
			emit finished();
			return;
		}

        bps_event_t* event = NULL;
        bps_get_event(&event, 0);

        if (event)
        {
            int domain = bps_event_get_domain(event);
            unsigned int code = bps_event_get_code(event);

            if (domain == sensor_domain)
            {
                switch (code)
                {
                case SENSOR_TYPE_TEMPERATURE:
                    celciusTemperature = sensor_event_get_temperature(event);
                    break;
                case SENSOR_TYPE_LIGHT:
                    illuminance = sensor_event_get_illuminance(event);
                    break;
                case SENSOR_TYPE_MAGNETOMETER:
                    sensor_event_get_xyz(event, &xyzMagnet[0], &xyzMagnet[1], &xyzMagnet[2]);
                    break;
                case SENSOR_TYPE_ACCELEROMETER:
                    sensor_event_get_xyz(event, &xyzAccel[0], &xyzAccel[1], &xyzAccel[2]);
                    break;
                case SENSOR_TYPE_GYROSCOPE:
                    sensor_event_get_xyz(event, &xyzGyro[0], &xyzGyro[1], &xyzGyro[2]);
                    break;
                default:
                    qDebug() << "Received bps sensor event for unrequested sensor type " << code;
                    break;
                }
            }

            // alert slots about current sensor values
            emit pulse(celciusTemperature
                       , illuminance
                       , qlistFromArray<QVariant, float>(xyzMagnet, 3)
                       , qlistFromArray<QVariant, float>(xyzAccel, 3)
                       , qlistFromArray<QVariant, float>(xyzGyro, 3));
        }

        /**
         * Fixme: we are in a non-UI thread.
         * we have to refrain ourself from refreshing the UI too often.
         * if we signal UI thread too often
         * the app would crash at libqtdeclarative.so.4.8.
         *
         *
         */
        m_condition.wait(&m_mutex, 100/*ms*/); //wait briefly until it's time to update ui again

    }
}

/**
 *
 * void prepareThread(QThread* thread);
 *
 * prepare to execute the logic of checking sensor in the given thread.
 *
 */
void SensorsThread::prepareThread(QThread* executeThread)
{
	if(parent())
	{
		setParent(NULL); //a QObject with a parent will not be able to move another thread.
	}

	 // Connects the thread's started() signal
	// to the startProcess() slot, causing it to start checking sensor.
	connect(executeThread, SIGNAL(started()), this, SLOT(startProcess()));

	// Connect counter finished signal to trigger thread quit, then delete.
	connect(this, SIGNAL(finished()), executeThread, SLOT(quit()));
	connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));

	// Make sure the thread object is deleted after execution has finished.
	connect(executeThread, SIGNAL(finished()), executeThread, SLOT(deleteLater()));

	moveToThread(executeThread);

}
