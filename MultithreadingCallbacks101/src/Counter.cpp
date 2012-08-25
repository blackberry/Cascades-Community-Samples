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

#include "Counter.hpp"
#include <QDebug>

/**
 * This class implements a Counter which will be able to run in its own thread.
 *
 */

/**
 * Constructor
 *
 */
Counter::Counter(QObject* parent)
    : QObject(parent)
    , m_pause(true)
    , m_exit(false)
    , m_counter(0)
    , m_updateinterval(1000)
    , m_pulseinterval(1)
    , m_executeThread(NULL)
{
}

/**
 * Destructor
 *
 */
Counter::~Counter()
{
	stop();//stop the execute thread if it was running.
    m_mutex.unlock();
    if(m_executeThread)
    {
    	m_executeThread->wait();
    	delete m_executeThread;
    }
}


/**
 * Counter::pause(bool value)
 *
 * This method will be called by other threads (ex.: main thread) to instruct this counter thread
 * to change its state to be
 * "paused" or "started"
 *
 */
void Counter::pause(bool value)
 {
	QMutexLocker locker(&m_mutex);

	if (m_pause != value) {
		//change the value while we own the lock and
		//wake up the run loop thread. The run loop thread
		//will remain blocked until the mutex lock is released
		//when the method exits.
		m_pause = value;
		m_condition.wakeOne();
	}
}

/**
 * void Counter::start()
 *
 * start the execute thread to run the counter.
 */
void Counter::start()
{
	if(m_executeThread!=NULL)
		return;

	m_executeThread = new QThread;

	prepareThread(m_executeThread);
	m_executeThread->start();
}

/**
 * void Counter::stop()
 *
 * instruct the counter to stop
 */
void Counter::stop()
{
	QMutexLocker locker(&m_mutex);

	if (m_exit != true) {
		//change the value while we own the lock and
		//wake up the counting loop. The counting thread
		//will remain blocked until the mutex lock is released
		//when the method exits.
		m_exit = true;
		m_condition.wakeOne();
	}
}

/**
 * Counter::startCounter()
 *
 * This is the method which will be executed when a counter thread starts
 */
void Counter::startCounter()
{
	emit countingStarted();
    forever
    {
        m_mutex.lock();
        m_counter++;
        if (m_counter % m_pulseinterval == 0)
        {
            //send a signal when the counter has reached the pulse interval
            emit pulse(m_counter);
        }

        if (m_pause)
        {
            //if the pause flag has been set, send a paused signal and wait
            //QCondition.Wait() will release the mutex lock and wait
            //until it is woken up through wakeone() or wakeall().
            emit paused();
            m_condition.wait(&m_mutex);

            //the thread has been woken up through wakeone() or wakeall(), and the lock
            //has been re-acquired. send a started signal
            emit countingStarted();

        }
        else if (m_exit)
        {
            //if the exit flag has been set, then unlock the mutex and exit terminate
            //the run loop
            m_mutex.unlock();
            qDebug() << "stop counting: " << QThread::currentThreadId();

            emit finished();
            return;
        }
        else
        {
            //here the run loop releases the mutex and waits until the wait time has expired
            //or the thread is woken with wakeone() or wakeall(). It re-acquires the lock after
            //it has awoken before continuing.
            m_condition.wait(&m_mutex, m_updateinterval); //wait briefly until it's time to increment counter
        }
        m_mutex.unlock();
    }
}

/**
 * Counter::setStartValue(int value)
 *
 * set the starting value of the counter
 */
void Counter::setStartValue(int value)
{
	QMutexLocker locker(&m_mutex);

	m_counter = value;

}

/**
 * Counter::getUpdateInterval()
 *
 * retrieve the interval value of updating the counter value at UI
 */
unsigned int Counter::getUpdateInterval()
{
	QMutexLocker locker(&m_mutex);
    return m_updateinterval;
}

/**
 * Counter::setUpdateInterval(unsigned int value)
 *
 * set the interval value of updating the counter value at UI
 */
void Counter::setUpdateInterval(unsigned int value)
{
	QMutexLocker locker(&m_mutex);

    m_updateinterval = value;
}

/**
 * Counter::getPulseInterval()
 *
 * retrieve the current pulse interval value
 */
unsigned int Counter::getPulseInterval()
{
	QMutexLocker locker(&m_mutex);
    return m_pulseinterval;
}

/**
 * Counter::setPulseInterval(unsigned int value)
 *
 * set the pulse interval value
 */
void Counter::setPulseInterval(unsigned int value)
{
	QMutexLocker locker(&m_mutex);
    m_pulseinterval = value;
    if (m_pulseinterval == 0)
    {
    	m_pulseinterval = 1;
    }
}


/**
 *
 * void prepareThread(QThread* thread);
 *
 * prepare to execute the logic of counter in the given thread.
 *
 */
void Counter::prepareThread(QThread* executeThread)
{
	if(parent())
	{
		setParent(NULL); //a QObject with a parent will not be able to move another thread.
	}

	 // Connects the thread's started() signal
	// to the startCounter() slot, causing it to start counting.
	connect(executeThread, SIGNAL(started()), this, SLOT(startCounter()));

	// Connect counter finished signal to trigger thread quit, then delete.
	connect(this, SIGNAL(finished()), executeThread, SLOT(quit()));
	connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));

	// Make sure the thread object is deleted after execution has finished.
	connect(executeThread, SIGNAL(finished()), executeThread, SLOT(deleteLater()));

	moveToThread(executeThread);

}

/**
 * Counter::isPaused()
 *
 * check if the counting thread is in paused state
 */
bool Counter::isPaused()
{
    QMutexLocker locker(&m_mutex);
    return m_pause;
}
