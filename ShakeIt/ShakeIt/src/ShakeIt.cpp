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


#include "ShakeIt.hpp"
#include "Color.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

ShakeIt::ShakeIt(bb::cascades::Application *app)
: QObject(app)
{
    // Color will act as the model for our qml.  It communicates instructions
    // to the headless portion of the app.
    _color = new Color(this);

    QmlDocument *qml = QmlDocument::create("asset:///main.qml")
                                    .property("_color", _color)
                                    .parent(this);

    AbstractPane *root = qml->createRootObject<AbstractPane>();
    app->setScene(root);
}

