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
#include <QList>
#include <QDeclarativeError>
#include <iostream>

#include "SmartSignalsApp.hpp"

using namespace bb::cascades;

SmartSignalsApp::SmartSignalsApp() {
    QmlDocument *qml = QmlDocument::create("main.qml");
    if (qml->hasErrors()) {
    	QList<QDeclarativeError> errors = qml->errors();
    	std::cout << "Errors occurred parsing qml:" << std::endl;
    	std::cout << &errors << std::endl;
    } else {
		qml->setContextProperty("cs", this);

		AbstractPane *root = qml->createRootNode<AbstractPane>();
		Application::setScene(root);
    }
}
