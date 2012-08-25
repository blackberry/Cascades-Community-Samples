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
#include <bb/cascades/AbstractPane>

#include "app.hpp"
#include "SensorsThread.hpp"

using namespace bb::cascades;

/**
 * App
 *
 * In this class you will learn the following:
 * -- Register C++ classes for use in QML documents
 */

App::App()
{
    // register the SensorsThread so that it is available in qml
    qmlRegisterType<SensorsThread>("threads.sensors", 1, 0, "Sensors");

    QmlDocument* qml = QmlDocument::create("main.qml");
    qml->setContextProperty("cs", this);

    AbstractPane* root = qml->createRootNode<AbstractPane>();
    Application::setScene(root);
}

