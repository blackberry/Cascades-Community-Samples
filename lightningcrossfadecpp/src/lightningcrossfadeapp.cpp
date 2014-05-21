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
#include "lightningcrossfadeapp.h"

#include <bb/cascades/Color>
#include <bb/cascades/Container>
#include <bb/cascades/DockLayout>
#include <bb/cascades/ImageView>
#include <bb/cascades/Page>
#include <bb/cascades/Slider>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>

using namespace bb::cascades;

LightningCrossfadeApp::LightningCrossfadeApp()
{
    // The UI is set up using a couple of different Containers set to handle
    // layouts of their children in different ways (stack and dock layouts).
    // First off we have the main content Container, where all other components will be added.
    Container *contentContainer = new Container();
    contentContainer->setLayout(StackLayout::create());
    contentContainer->setTopPadding(20.0f);
    contentContainer->setBackground(Color::fromARGB(0xfff8f8f8));

    // Inside the content Container we have two component Containers, the first one contain images.
    Container *imageContainer = setUpImageContainer();
    imageContainer->setHorizontalAlignment(HorizontalAlignment::Center);

    // Then set up the Slider Container and align it to the center of the content Container as well.
    Container *sliderContainer = setUpSliderContainer(imageContainer);
    sliderContainer->setHorizontalAlignment(HorizontalAlignment::Center);

    // Add the two containers to the content container.
    contentContainer->add(imageContainer);
    contentContainer->add(sliderContainer);

    // We create a page with the control as content.
    Page *page = new Page();
    page->setContent(contentContainer);

    // Create the application scene and we are done.
    Application::instance()->setScene(page);
}

Container *LightningCrossfadeApp::setUpImageContainer()
{
    // In this function a Container containing two images are set up.
    // A dock layout is used to align the images.
    Container* nightAndDayContainer = new Container();
    nightAndDayContainer->setLayout(new DockLayout());

    // The first image is the night image, taken at the same
    // position as the day one, but further from the viewer.
    ImageView* night = ImageView::create("asset:///images/night.jpg");

    // Center it using alignment.
    night->setHorizontalAlignment(HorizontalAlignment::Center);
    night->setVerticalAlignment(VerticalAlignment::Center);

    // Now, we will overlay the day image on top of the night image.
    // Since the day image is on top of the night one, we can show the
    // night image with changing the opacity value of this image.
    ImageView* day = ImageView::create("asset:///images/day.jpg").opacity(0);

    // Center it using alignment.
    day->setHorizontalAlignment(HorizontalAlignment::Center);
    day->setVerticalAlignment(VerticalAlignment::Center);

    // The day image is attached to the slider later. We need to be able to 
    // find the object at a later point in time so an object name is assigned.
    day->setObjectName("dayImage");

    // Now add all the images to the Container and the alignment will be
    // automatically done by the layout system.
    nightAndDayContainer->add(night);
    nightAndDayContainer->add(day);

    return nightAndDayContainer;
}

Container *LightningCrossfadeApp::setUpSliderContainer(Container *imageContainer)
{
    // Create the Slider Container with the controls set to sort themselves from
    // left to right rather then top to bottom (which is the default for a StackLayout).
    Container* sliderContainer = Container::create().left(20.0f).right(20.0f).top(25.0f).bottom(
            25.0f);
    sliderContainer->setLayout(StackLayout::create().orientation(LayoutOrientation::LeftToRight));

    // This is the Slider you see at the bottom of the page when it get's a onImmediateValueChanged
    // event it changes the image with id night's opacity to that value, margins for space.
    Slider* opacitySlider = Slider::create().leftMargin(20.0f).rightMargin(20.0f);

    // Center the slider in the stack layout. We have given a positive space quota which
    // makes the slider opacity value less than the moon and sun icon images when
    // laying out on screens of different widths.
    opacitySlider->setLayoutProperties(StackLayoutProperties::create().spaceQuota(1.0f));
    opacitySlider->setHorizontalAlignment(HorizontalAlignment::Fill);

    // A moon and sun icon image is used to illustrate that time of day, both
    // aligned to the center in a vertical direction to line up with the slider.
    ImageView* moon = ImageView::create("asset:///images/moon.png");
    moon->setVerticalAlignment(VerticalAlignment::Center);
    ImageView* sun = ImageView::create("asset:///images/sun.png");
    sun->setVerticalAlignment(VerticalAlignment::Center);

    // The day image will be attached to a slider so we need to get the corresponding
    // object from the image Container.
    ImageView *dayImage = imageContainer->findChild<ImageView*>("dayImage");

    // Connect the Slider value directly to the opacity property of the day image view.
    connect(opacitySlider, SIGNAL(immediateValueChanged(float)), dayImage, SLOT(setOpacity(float)));

    // Add the components to the slider container (remember that they will be stacked from left to right).
    sliderContainer->add(moon);
    sliderContainer->add(opacitySlider);
    sliderContainer->add(sun);

    return sliderContainer;
}
