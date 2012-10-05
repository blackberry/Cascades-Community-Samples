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

#include <QObject>
#include <bb/cascades/CustomControl>

#ifndef TIMER_HPP_
#define TIMER_HPP_

class Timer  : public bb::cascades::CustomControl {

	Q_OBJECT
	Q_PROPERTY(
			bool active
			READ isActive
			NOTIFY activeChanged)
	Q_PROPERTY(
			int interval
			READ interval
			WRITE setInterval
	        NOTIFY intervalChanged)

public:
    explicit Timer(QObject* parent = 0);
    bool isActive();
    void setInterval(int mSec);
    int interval();

public slots:
    void start();
    void stop();

signals:
        void timeout();
        void intervalChanged();
        void activeChanged();
private:
    QTimer* _timer;
};

#endif /* TIMER_HPP_ */
