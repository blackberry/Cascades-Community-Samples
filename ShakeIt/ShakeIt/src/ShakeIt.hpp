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


#ifndef SHAKEIT_HPP_
#define SHAKEIT_HPP_

#include <QObject>

class Color;

namespace bb { namespace cascades { class Application; }}

/**
 * The UI portion of our app.
 */
class ShakeIt : public QObject
{
    Q_OBJECT
public:
    ShakeIt(bb::cascades::Application *app);
    virtual ~ShakeIt() {}

private:
    Color* _color;
};


#endif
