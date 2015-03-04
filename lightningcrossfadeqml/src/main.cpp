/* Copyright (c) 2012, 2013, 2014, 2015 BlackBerry Limited.
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
#include <Qt/qdeclarativedebug.h>

using ::bb::cascades::Application;

Q_DECL_EXPORT int main(int argc, char **argv)
{
    // Instantiate the main application constructor
    Application app(argc, argv);

    // Create the application
    LightningCrossfadeApp mainApp;

    // Complete the transaction started in the main app
    // constructor and start the client event loop here
    // When the loop is exited, the application deletes the
    // scene, which deletes all of its children
    return Application::exec();
}

