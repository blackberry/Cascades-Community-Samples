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

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Label>
#include <bb/cascades/AbstractPane>
#include "app.hpp"
#include "Counter.hpp"

using namespace bb::cascades;

/**
 * This application demonstrates:
 * -- how to use QThread to execute the logics which needs to be run in a thread other than main thread.
 * -- how to expose the new C++ Class to be  used QML view
 * --how to sync among threads including the main thread.
 *
 */


/**
 * App::App()
 *
 * Initial setup for this application:
 * --Register C++ Class (e.g.: Type) to be used in QML view
 *
 */
App::App()
{
    qmlRegisterType<Counter>("threads.counter", 1, 0, "Counter");

    QmlDocument* qml = QmlDocument::create("main.qml");
    qml->setContextProperty("cs", this);

    bb::cascades::AbstractPane* root =  qml->createRootNode<bb::cascades::AbstractPane>();
    Application::setScene(root);
}


