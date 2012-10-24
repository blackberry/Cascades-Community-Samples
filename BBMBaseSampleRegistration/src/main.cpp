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
#include <bb/cascades/NavigationPane>
#include <bb/cascades/Page>

#include <RegistrationHandler.hpp>

#include <QLocale>
#include <QTranslator>

/*
 * A base sample that registers with the BBM Social Platform.
 * Other samples will extend this to demonstrate specific features of the BBM SP.
 */

using namespace bb::cascades;

int main(int argc, char **argv)
{
    // this is where the server is started etc
    Application app(argc, argv);

    // create scene document from main.qml asset
    // set parent to created document to ensure it exists for the whole application lifetime
//    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(&app);
    // create root object for the UI
//    NavigationPane *root = qml->createRootObject<NavigationPane>();
    // set created root object as a scene
//    app.setScene(root);

    RegistrationHandler* page;
    page = new RegistrationHandler();
    app.setScene(page);

//	root->push(page);

    // we complete the transaction started in the app constructor and start the client event loop here
    return Application::exec();
    // when loop is exited the Application deletes the scene which deletes all its children (per qt rules for children)
}
