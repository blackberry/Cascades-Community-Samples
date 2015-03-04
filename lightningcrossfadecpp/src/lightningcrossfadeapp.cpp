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

#include <bb/cascades/Color>
#include <bb/cascades/Container>
#include <bb/cascades/DockLayout>
#include <bb/cascades/ImageView>
#include <bb/cascades/Page>
#include <bb/cascades/Slider>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/UIConfig>

using namespace bb::cascades;

LightningCrossfadeApp::LightningCrossfadeApp()
{

    // Create the main UI in a container with a StackLayout
    Container *contentContainer = new Container();

    // Get the UIConfig object to use resolution independent sizes
    UIConfig *ui = contentContainer->ui();
    contentContainer->setTopPadding(ui->du(2.0));
    contentContainer->setLeftPadding(ui->du(2.0));
    contentContainer->setRightPadding(ui->du(2.0));

    contentContainer->setLayout(StackLayout::create());
    contentContainer->setBackground(Color::fromARGB(0xfff8f8f8));

    // Inside the main UI container, create two containers
    // The first container contain the images
    Container *imageContainer = setUpImageContainer();
    imageContainer->setHorizontalAlignment(HorizontalAlignment::Center);

    // The second container contains the slider, which is
    // aligned to the center of the main UI container
    Container *sliderContainer = setUpSliderContainer(imageContainer);
    sliderContainer->setHorizontalAlignment(HorizontalAlignment::Center);

    // Add the two containers to the main UI container
    contentContainer->add(imageContainer);
    contentContainer->add(sliderContainer);

    // Create a page with the main UI as the content
    Page *page = new Page();
    page->setContent(contentContainer);

    // Create the application scene
    Application::instance()->setScene(page);
}

Container *LightningCrossfadeApp::setUpImageContainer()
{
    // Create a container that contains two images and
    // uses a dock layout to align the images
    Container* nightAndDayContainer = new Container();
    nightAndDayContainer->setLayout(new DockLayout());

    // Add the night image
    ImageView* night = ImageView::create("asset:///images/night.jpg");

    // Center the night image using alignment
    night->setHorizontalAlignment(HorizontalAlignment::Center);
    night->setVerticalAlignment(VerticalAlignment::Center);

    // Add the day image on top of the night image and
    // hide the day image by changing its opacity value to 0
    ImageView* day = ImageView::create("asset:///images/day.jpg").opacity(0);

    // Center the day image using alignment
    day->setHorizontalAlignment(HorizontalAlignment::Center);
    day->setVerticalAlignment(VerticalAlignment::Center);

    // Assign an object name to the day image so that it can
    // be found and attached to the slider later
    day->setObjectName("dayImage");

    // Add all the images to the Container
    nightAndDayContainer->add(night);
    nightAndDayContainer->add(day);

    return nightAndDayContainer;
}

Container *LightningCrossfadeApp::setUpSliderContainer(Container *imageContainer)
{
    // Create a container for the slider and sort the controls
    // in this container from left to right
    Container* sliderContainer = new Container();

    // Get the UIConfig object to use resolution independent sizes
    UIConfig *sliderui = sliderContainer->ui();

    sliderContainer->setLeftPadding(sliderui->du(1.0));
    sliderContainer->setRightPadding(sliderui->du(1.0));
    sliderContainer->setTopPadding(sliderui->du(4.0));
    sliderContainer->setBottomPadding(sliderui->du(2.0));

    sliderContainer->setLayout(StackLayout::create().orientation(LayoutOrientation::LeftToRight));

    // Create the slider that you see at the bottom of the screen
    // When it gets an onImmediateValueChanged() signal,
    // it opacity of the night image to make it disappear
    Slider* opacitySlider = Slider::create().leftMargin(sliderui->du(1.0)).rightMargin(sliderui->du(1.0));

    // Center the slider in the stack layout
    // and give it a positive space quota, which
    // makes the slider opacity value less than
    // the moon and sun icon images when they
    // are displayed on screens with different widths
    opacitySlider->setLayoutProperties(StackLayoutProperties::create().spaceQuota(1.0f));
    opacitySlider->setHorizontalAlignment(HorizontalAlignment::Fill);

    // Add a moon and a sun image on either side of slider
    ImageView* moon = ImageView::create("asset:///images/moon.png");
    moon->setVerticalAlignment(VerticalAlignment::Center);
    ImageView* sun = ImageView::create("asset:///images/sun.png");
    sun->setVerticalAlignment(VerticalAlignment::Center);

    // Attach the day image to the slider using the
    // object ID from the image Container
    ImageView *dayImage = imageContainer->findChild<ImageView*>("dayImage");

    // Connect the slider value directly to the
    // opacity property of the day image

    // Make sure to test the result  to detect any errors
    bool result;
    Q_UNUSED(result);

    result=connect(opacitySlider,
            SIGNAL(immediateValueChanged(float)),
            dayImage,
            SLOT(setOpacity(float)));

    Q_ASSERT(result);

    // Add the components to the slider container
    // (Remember that they will be stacked from left to right)
    sliderContainer->add(moon);
    sliderContainer->add(opacitySlider);
    sliderContainer->add(sun);

    return sliderContainer;
}
