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
#include <bb/ApplicationInfo>
#include <bb/system/SystemToast>

#include <QLocale>
#include <QTranslator>
#include "applicationui.hpp"
#include <Flurry.h>

// include JS Debugger / CS Profiler enabler
// this feature is enabled by default in the debug build only
#include <Qt/qdeclarativedebug.h>

using namespace bb::cascades;


void myMessageOutput(QtMsgType type, const char* msg){
	fprintf(stdout, "%s\n", msg);
	fflush(stdout);
}

Q_DECL_EXPORT int main(int argc, char **argv) {

	// Only perform debug logging if running a debug build
#if !defined(QT_NO_DEBUG)
	Flurry::Analytics::SetDebugLogEnabled(true);
#endif

	// Read and log the version of the app being used
	Flurry::Analytics::SetAppVersion(bb::ApplicationInfo().version());

	//Set our session API Key (replace this value with your own key once you're all set up)
	Flurry::Analytics::StartSession("7DV4Z2GNKZ5M9S5VHBVV");

	Application app(argc, argv);
    qInstallMsgHandler(myMessageOutput);

	// localization support
	QTranslator translator;
	QString locale_string = QLocale().name();
	QString filename = QString("FlurryAnalyticsSample_%1").arg(locale_string);
	if (translator.load(filename, "app/native/qm")) {
		app.installTranslator(&translator);
	}

	new ApplicationUI(&app);

	// we complete the transaction started in the app constructor and start the client event loop here
	return Application::exec();
	// when loop is exited the Application deletes the scene which deletes all its children (per qt rules for children)
}
