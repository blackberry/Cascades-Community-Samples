/* Copyright (c) 2013 BlackBerry.
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
#include <bb/cascades/advertisement/Banner>
#include <bb/platform/PaymentManager>
#include <bb/platform/PaymentConnectionMode>

#include <QLocale>
#include <QTranslator>
#include <Qt/qdeclarativedebug.h>

#include "FreemiumSampleApp.hpp"

using namespace bb::cascades;


Q_DECL_EXPORT int main(int argc, char **argv)
{
    // this is where the server is started etc
    Application app(argc, argv);

    // Register our advertising and payment classes so they can be accessed from QML
    qmlRegisterType<bb::cascades::advertisement::Banner>("bb.cascades.advertisement", 1, 0, "Banner");

    // localization support
    QTranslator translator;
    QString locale_string = QLocale().name();
    QString filename = QString( "FreemiumSampleApp_%1" ).arg( locale_string );
    if (translator.load(filename, "app/native/qm")) {
        app.installTranslator( &translator );
    }

    new FreemiumSampleApp(&app);

    // we complete the transaction started in the app constructor and start the client event loop here
    return Application::exec();
    // when loop is exited the Application deletes the scene which deletes all its children (per qt rules for children)
}

