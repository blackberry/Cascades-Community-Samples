/* Copyright (c) 2015 BlackBerry Ltd.
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
 */

//Application Icon Source:  https://www.iconfinder.com/icons/6308/fusion-icon_icon#size=128
//Email icon source:  https://www.iconfinder.com/icons/143823/email_envelope_letter_mail_icon#size=128
//P2P icon source:  https://www.iconfinder.com/icons/73061/base_chartreuse_map_marker_pin_push_right_icon#size=128  Author: http://www.icons-land.com/
//Phone icon source:  https://www.iconfinder.com/icons/386738/call_phone_viber_icon#size=128
//SMS icon source: https://www.iconfinder.com/icons/328069/message_phone_send_sms_icon#size=128

#include "applicationui.hpp"

#include <bb/cascades/Application>

#include <QLocale>
#include <QTranslator>

#include <Qt/qdeclarativedebug.h>

using namespace bb::cascades;

Q_DECL_EXPORT int main(int argc, char **argv)
{
    Application app(argc, argv);

    // Create the Application UI object, this is where the main.qml file
    // is loaded and the application scene is set.
    ApplicationUI appui;

    // Enter the application main event loop.
    return Application::exec();
}
