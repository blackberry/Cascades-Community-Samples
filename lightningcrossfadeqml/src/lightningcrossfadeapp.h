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

#ifndef LIGHTNINGCROSSFADEAPP_H
#define LIGHTNINGCROSSFADEAPP_H

#include <bb/cascades/Application>

/**
 * LightningCrossfade Description:
 *
 * This is basic Core-controls application for Cascades.
 *
 * Feature summary
 * - Adding an image to the UI
 * - layout of several UI elements
 * - Adding and reading a slider
 * - Doing all above with QML
 */
class LightningCrossfadeApp: public QObject
{
    Q_OBJECT

public:
    // This is our constructor that sets up the recipe.
    LightningCrossfadeApp();

};

#endif // ifndef LIGHTNINGCROSSFADEAPP_H
