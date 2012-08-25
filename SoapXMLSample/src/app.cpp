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
#include <bb/cascades/DropDown>
#include <QXmlStreamReader>
#include <QDomDocument>
#include "WeatherService.hpp"
#include "app.hpp"

using namespace bb::cascades;

/**
 * app
 *
 * In this class you will learn the following:
 * -- Register C++ classes for use in QML documents
 */

App::App()
{
    //add WeatherService class as qml type
    qmlRegisterType<WeatherService>("wsf.cdyne.com.WeatherWS", 1, 0, "WeatherService");

    QmlDocument* qml = QmlDocument::create("main.qml");
    qml->setContextProperty("cs", this);

    AbstractPane* root = qml->createRootNode<AbstractPane>();
    Application::setScene(root);
}

