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
#include "lightningcrossfadeapp.h"

#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

LightningCrossfadeApp::LightningCrossfadeApp()
{
    // Create and load the QML file, using build patterns.
    QmlDocument *qml = QmlDocument::create("asset:///lightningcrossfade.qml");
    qml->setContextProperty("cs", this);

    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // Set the application scene
    Application::instance()->setScene(root);
}
