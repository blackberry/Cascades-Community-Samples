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
*
* Note: The video file packaged is in this application titled
* "BB10DevAlpha.wmv" in the "assets" folder is NOT under the Apache 2.0 license.
* Only Research In Motion Limited has the distribution rights for this video file.
*/

#include "app.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <screen/screen.h>

#include <bb/cascades/ForeignWindow>

#include <stdio.h>

using namespace bb::cascades;

App::App()
{

    QmlDocument *qml = QmlDocument::create("main.qml");
    //-- setContextProperty expose C++ object in QML as an variable
    //-- uncomment next line to introduce 'this' object to QML name space as an 'app' variable
    qml->setContextProperty("app", this);
    
    AbstractPane *root = qml->createRootNode<AbstractPane>();
    Application::setScene(root);
}

App::~App() {}

void App::setPositionAndSize(unsigned long windowHandle, float x, float y, float width, float height) {

	screen_window_t win = (screen_window_t)windowHandle;

	//prepare the array for position parameter
	int positionParam[2];
	positionParam[0] = x;
	positionParam[1] = y;

	int result = -1;
	result = screen_set_window_property_iv(win, SCREEN_PROPERTY_POSITION, positionParam);

	if ( result != 0) {
		//TODO: Error handling
		fprintf(stderr,"ERROR : screen_set_window_property_iv : SCREEN_PROPERTY_POSITION = %d \n", result );
	} //if

	//prepare the array for size parameter
	int sizeParam[2];
	sizeParam[0] = width;
	sizeParam[1] = height;

	result = -1;
	result = screen_set_window_property_iv(win, SCREEN_PROPERTY_SIZE, sizeParam);

	if ( result != 0) {
		//TODO: Error handling
		fprintf(stderr,"ERROR : screen_set_window_property_iv : SCREEN_PROPERTY_SIZE = %d \n", result );
	} //if
}

QString App::getAbsolutePath(QString localPath) {
	return QDir::currentPath() + localPath;

}
