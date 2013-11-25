/*
 * Copyright (c) 2013 BlackBerry Limited.
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

#include "Color.hpp"

#include <bb/system/InvokeManager.hpp>
#include <bb/system/InvokeRequest.hpp>

#include <stdio.h>

Color::Color(QObject* parent)
    : QObject(parent)
    , _step1Color(Red)
    , _step1Seconds(1)
    , _step2Color(None)
    , _step2Seconds(0)
    , _step3Color(None)
    , _step3Seconds(0) {

    _invokeManager = new bb::system::InvokeManager(this);
}

Color::~Color() {
}

int Color::step1Color() const {
    return _step1Color;
}

void Color::setStep1Color(int value) {
    _step1Color = value;
}

int Color::step1Seconds() const {
    return _step1Seconds;
}

void Color::setStep1Seconds(int value) {
    _step1Seconds = value;
}

int Color::step2Color() const {
    return _step2Color;
}

void Color::setStep2Color(int value) {
    _step2Color = value;
}

int Color::step2Seconds() const {
    return _step2Seconds;
}

void Color::setStep2Seconds(int value) {
    _step2Seconds = value;
}

int Color::step3Color() const {
    return _step3Color;
}

void Color::setStep3Color(int value) {
    _step3Color = value;
}

int Color::step3Seconds() const {
    return _step3Seconds;
}

void Color::setStep3Seconds(int value) {
    _step3Seconds = value;
}

/**
 * Encode the data into a format that both the UI and the headless portion can
 * understand.
 */
void Color::encodeData(int color, int seconds, QString& data) {
    if (color >= Red && color <= Blue && seconds > 0) {

        if (data.length() > 0) {
            data.append(",");
        }

        switch (color) {
        case Red:
            data.append("red");
            break;
        case Green:
            data.append("green");
            break;
        case Blue:
            data.append("blue");
            break;
        }
        data.append(",");
        data.append(QString::number(seconds));
    }
}

/**
 * Send off the new instructions to the headless portion.  We do this through
 * an invoke action.  We defined the invoke action in the bar-descriptor.xml
 */
void Color::done() {
    QString data;
    encodeData(_step1Color, _step1Seconds, data);
    encodeData(_step2Color, _step2Seconds, data);
    encodeData(_step3Color, _step3Seconds, data);

    // Send the data to the headless component
    bb::system::InvokeRequest request;
    request.setTarget("com.example.ShakeItService");
    request.setAction("SetBlinkSteps");
    request.setData(data.toAscii());
    (void)_invokeManager->invoke(request);
}

/**
 * Let the headless portion know that the user has cleared all instructions.
 */
void Color::clear() {
    bb::system::InvokeRequest request;
    request.setTarget("com.example.ShakeItService");
    request.setAction("ClearBlinkSteps");
    (void)_invokeManager->invoke(request);
}
