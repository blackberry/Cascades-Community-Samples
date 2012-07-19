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
*
* Note: The video file packaged is in this application titled
* "BB10DevAlpha.wmv" in the "assets" folder is NOT under the Apache 2.0 license.
* Only Research In Motion Limited has the distribution rights for this video file.
*/

#include "app.hpp"

#include <bb/cascades/Application>

#include <QLocale>
#include <QTranslator>

#include <bb/multimedia/MediaPlayer>
#include <bb/multimedia/MediaError>

using ::bb::cascades::Application;

int main(int argc, char **argv)
{
	// code for enabling the respective types to be referenced from QML
	qmlRegisterType<bb::multimedia::MediaPlayer>("bb.multimedia", 1, 0, "MediaPlayer");
	qmlRegisterUncreatableType<bb::multimedia::MediaError>("bb.multimedia", 1, 0, "MediaError", "");
	qRegisterMetaType<bb::multimedia::MediaError::Code>("bb::multimedia::MediaError::Code");

    //-- this is where the server is started etc
    Application app(argc, argv);
    
    //-- localization support
    QTranslator translator;
    QString locale_string = QLocale().name();
    QString filename = QString( "VideoPlayerSample_%1" ).arg( locale_string );
    if (translator.load(filename, "app/native/qm")) {
        app.installTranslator( &translator );
    }
    
    App mainApp;
    
    //-- we complete the transaction started in the app constructor and start the client event loop here
    return Application::exec();
    //-- when loop is exited the Application deletes the scene which deletes all its children (per qt rules for children)
}
