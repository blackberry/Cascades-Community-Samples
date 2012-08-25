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

#ifndef COUNTER_HPP_
#define COUNTER_HPP_

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

/*
 * This class implements a counter  when will be invoked the thread is started, the
 * counting can be paused and resumed by calling the pause() method after the thread has started.
 *
 * During the counting, a counter (m_counter) is incremented based on the milliseconds defined by the
 * m_updateInterval member. When the counter reaches an increment defined by m_pulseInterval it
 * emits a pulse() signal with the value of the current.
 */
class Counter: public QObject
{
    Q_OBJECT
    Q_PROPERTY(int startValue WRITE setStartValue)
    Q_PROPERTY(unsigned int updateInterval READ getUpdateInterval WRITE setUpdateInterval)
    Q_PROPERTY(unsigned int pulseInterval READ getPulseInterval WRITE setPulseInterval)
    Q_PROPERTY(bool paused READ isPaused)
public:
    /*
     * Ctor & Dtor
     *
     * @param parent - the parent object
     * @param startval - the starting value of the counter
     * @param incinterval - time interval in milliseconds to increment
     * @param pulse - the interval (counter) at which to emit a signal that the value has changed
     */
    Counter(QObject* parent = 0);
    virtual ~Counter();

    /*
     * Pauses or resumes the thread based on the value parameter
     *
     * @param value - if true, thread is paused. if false thread is resumed
     */
    Q_INVOKABLE void pause(bool value);

    /*
     * Sets the start value for the counter. Can only be set
     * before the thread is started. Will not be changed even after
     * a running thread is paused.
     */
    void setStartValue(int value);

    /*
     * Setter and Getter for the updateinterval member
     */
    unsigned int getUpdateInterval();
    void setUpdateInterval(unsigned int value);

    /*
     * Setter and getter for the pulseinterval member
     */
    unsigned int getPulseInterval();
    void setPulseInterval(unsigned int value);


    /**
     * void Counter::start()
     *
     * start the execute thread to run the counter.
     */
    Q_INVOKABLE void start();


    /*
     * Returns whether the counting is paused or not
     */
    bool isPaused();

    /**
     * instruct the counter to stop
     */
    void stop();

signals:
    /*
     * Emits a signal from the thread indicating the current counter value
     */
    void pulse(int counter);

    /**
     * Emits a signal when the thread moves from a pause to a running state
     */
    void countingStarted();
    /*
     * Emits a signal when the executing thread is put in a paused state
     */
    void paused();


    /*
     * Emits a signal when the counter stopped running
     */
    void finished();

public slots:

	/**
	 * startCounter()
	 *
	 * This method shall be invoked when the executing thread has started
	 *
	 */
    void startCounter();

private:

    /**
     *
     * void prepareThread(QThread* executeThread);
     *
     * prepare to execute the logic of counter in the given thread.
     *
     * @param executeThread, the thread which will run the counter
     */
    void prepareThread(QThread* executeThread);

    bool m_pause;       //used to signal to the run loop to pause execution
    bool m_exit;        //used to signal the run loop to exit
    int m_counter;      //a counter that is incremented over the lifetime of the thread
    unsigned int m_updateinterval;  //the number of milli-seconds to wait before incrementing the counter
    unsigned int m_pulseinterval;   //interval at which a pulse signal is emitted
    QMutex m_mutex; //used to guard the m_pause and m_exit flags when changes are made outside of the run loop
    QWaitCondition m_condition; //wait condition for pausing the run loop

    QThread* m_executeThread; // the thread which actually execute the counting.
};

#endif /* COUNTER_HPP_ */
