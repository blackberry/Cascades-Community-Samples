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

#ifndef COLOR_HPP_
#define COLOR_HPP_

#include <QObject>

namespace bb { namespace system { class InvokeManager; } }

/**
 * Color acts as the back-end model to our QML.  As drop-down values are
 * changed they are captured here.  When the user decides they want to submit
 * the new blink instructions, we send an invoke to the headless part of the
 * app.
 */
class Color : public QObject {
    Q_OBJECT

    Q_PROPERTY(int step1Color READ step1Color WRITE setStep1Color);
    Q_PROPERTY(int step1Seconds READ step1Seconds WRITE setStep1Seconds);

    Q_PROPERTY(int step2Color READ step2Color WRITE setStep2Color);
    Q_PROPERTY(int step2Seconds READ step2Seconds WRITE setStep2Seconds);

    Q_PROPERTY(int step3Color READ step3Color WRITE setStep3Color);
    Q_PROPERTY(int step3Seconds READ step3Seconds WRITE setStep3Seconds);

public:
    Color(QObject *parent=0);
    virtual ~Color();

    /**
     * Called from QML when the user is done providing blink instructions.
     */
    Q_INVOKABLE void done();

    /**
     * Called from QML when the user has cleared the instructions.
     */
    Q_INVOKABLE void clear();

    // The rest of these accessors are bound to the QML drop-downs.

    int step1Color() const;
    void setStep1Color(int value);
    int step1Seconds() const;
    void setStep1Seconds(int value);

    int step2Color() const;
    void setStep2Color(int value);
    int step2Seconds() const;
    void setStep2Seconds(int value);

    int step3Color() const;
    void setStep3Color(int value);
    int step3Seconds() const;
    void setStep3Seconds(int value);


    static const int None = -1;
    static const int Red = 0;
    static const int Green = 1;
    static const int Blue = 2;

private:
    bb::system::InvokeManager* _invokeManager;

    int _step1Color;
    int _step1Seconds;

    int _step2Color;
    int _step2Seconds;

    int _step3Color;
    int _step3Seconds;

    void encodeData(int color, int seconds, QString& data);
};

#endif /* COLOR_HPP_ */
