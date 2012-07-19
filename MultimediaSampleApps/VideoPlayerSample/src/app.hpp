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


#ifndef APP_H
#define APP_H

#include <QObject>

/*!
 * @brief Application GUI object
 */
class App : public QObject
{
    Q_OBJECT

public:
    App();
    ~App();

	/**
	 * Manually set the position (x,y) and size (width, height) of the windowHandle
	 *
	 * NOTE: THIS IS A WORK AROUND FOR A CURRENT BUG IN THE "ForeignWindow" class. Ideally, you wouldn't need this extra method to manually
	 * reset the position and size of the window handle
	 */
	Q_INVOKABLE void setPositionAndSize(unsigned long windowHandle, float x, float y, float width, float height);

	/**
	 * Return the absolute path; concatenates the argument passed
	 */
	Q_INVOKABLE QString getAbsolutePath(QString localPath);
};

#endif // ifndef APP_H
