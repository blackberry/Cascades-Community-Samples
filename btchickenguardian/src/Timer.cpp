/* Copyright (c) 2013 BlackBerry Limited.
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

#include "Timer.hpp"

Timer::Timer(QObject *parent)
	: bb::cascades::CustomControl((bb::cascades::Container *) parent)
	, _timer(new QTimer(this))
{
    connect(_timer, SIGNAL(timeout()), this, SIGNAL(timeout()));
    setVisible(false);
}

Timer::~Timer() {}

bool Timer::isActive() {
    return _timer->isActive();
}

int Timer::timeLimit() {
    return _timer->interval();
}

void Timer::setTimeLimit(int mSec) {
    if (_timer->interval() == mSec)
        return;

    _timer->setInterval(mSec);
    emit timeLimitChanged();
}

void Timer::start() {
    if (_timer->isActive())
        return;

    _timer->start();
    emit activeChanged();
}

void Timer::stop() {
    if (!_timer->isActive())
        return;

    _timer->stop();
    emit activeChanged();
}
