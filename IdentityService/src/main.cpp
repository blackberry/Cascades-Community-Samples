/**
   Copyright (c) 2013 BlackBerry Limited.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**/
#include <bb/cascades/Application>

#include <QLocale>
#include <QTranslator>
#include "applicationui.hpp"
#include <errno.h>
#include <bps/bps.h>
#include <ids.h>
#include <ids_blackberry_id.h>
#include <ids_blackberry_profile.h>

#include <Qt/qdeclarativedebug.h>

using namespace bb::cascades;

#include <QDebug>
#include <QString>

Q_DECL_EXPORT int main(int argc, char **argv) {
	/**
	 * Initialize the BPS and IDS libraries.
	 */
	bps_initialize();
	ids_initialize();

	Application app(argc, argv);

	// Create the Application UI object, this is where the main.qml file
	// is loaded and the application scene is set.
	new ApplicationUI(&app);

	// Enter the application main event loop.
	int toReturn = Application::exec();

    /**
     * Cleanup is good - m'kay?
     */
	ids_shutdown();
	bps_shutdown();

	return toReturn;
}
