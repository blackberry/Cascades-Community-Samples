/*
* Copyright (c) 2012 Brian Scheirer
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

#include "app.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

App::App()
{
    QmlDocument *qml = QmlDocument::create("main.qml");
    //-- setContextProperty expose C++ object in QML as an variable
    //-- uncomment next line to introduce 'this' object to QML name space as an 'app' variable
    //qml->setContextProperty("app", this);
    
    AbstractPane *root = qml->createRootNode<AbstractPane>();
    Application::setScene(root);
}
