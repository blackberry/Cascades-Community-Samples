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

using namespace bb::cascades;

namespace bb
{
    namespace cascades
    {
        class Container;
    }
}

/**
 * LightningCrossfade Description:
 *
 * This is basic Core-controls application for Cascades.
 *
 * Feature summary
 * - Adding an image to the UI
 * - layout of several UI elements
 * - Adding and reading a Slider
 * - Doing all above with C++ code
 */
class LightningCrossfadeApp: public QObject
{
    Q_OBJECT

public:
    LightningCrossfadeApp();

private:
    /**
     * This function creates a container with several images center aligned and
     * on top of each other.This will be the first Control in the Content Container.
     *
     * @return The Container containing the images.
     */
    Container *setUpImageContainer();

    /**
     * This function sets up the slider Container (a Slider control and two icon images).
     *
     * @param  imageContainer The container with the image to which we connect the slider.
     * @return The Container containing the slider and its icons.
     */
    Container *setUpSliderContainer(Container *imageContainer);

};

#endif // ifndef LIGHTNINGCROSSFADEAPP_H
