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
#include <bb/cascades/Application>
#include <bb/cascades/ForeignWindowControl>
#include <bb/cascades/Container>
#include <bb/cascades/StackLayout>
#include <bb/cascades/DockLayout>
#include <bb/cascades/LayoutProperties>
#include <bb/cascades/WindowProperty>
#include <bb/cascades/Button>
#include <bb/cascades/Page>

#include <camera/camera_api.h>
#include <screen/screen.h>
#include <bps/soundplayer.h>
#include <unistd.h>
#include "hellocameraapp.hpp"

using namespace bb::cascades;

// define this to debug the ForeignWindowControl race condition
#define BREAK_FWC
// define this to enable a workaround for the above race condition
#define WORKAROUND_FWC

HelloCameraApp::HelloCameraApp() :
        mCameraHandle(CAMERA_HANDLE_INVALID)
{
    qDebug() << "HelloCameraApp";

    // create our foreign window
    mViewfinderWindow = ForeignWindowControl::create()
        .windowId(QString("cameraViewfinder"));
    // Allow Cascades to update the native window's size, position, and visibility, but not the source-size.
    // Cascades may otherwise attempt to redefine the buffer source-size to match the window size, which would yield
    // undesirable results.  You can experiment with this if you want to see what I mean.
    mViewfinderWindow->setUpdatedProperties(WindowProperty::Position | WindowProperty::Size | WindowProperty::Visible);

    QObject::connect(mViewfinderWindow,
                     SIGNAL(windowAttached(screen_window_t, const QString &, const QString &)),
                     this, SLOT(onWindowAttached(screen_window_t, const QString &,const QString &)));

    // create a bunch of camera control buttons
    // NOTE: some of these buttons are not initially visible
    mStartFrontButton = Button::create("Front Camera")
        .onClicked(this, SLOT(onStartFront()));
    mStartRearButton = Button::create("Rear Camera")
        .onClicked(this, SLOT(onStartRear()));
    mStopButton = Button::create("Stop Camera")
        .onClicked(this, SLOT(onStopCamera()));
    mStopButton->setVisible(false);
    mTakePictureButton = Button::create("Take Picture")
        .onClicked(this, SLOT(onTakePicture()));
    mTakePictureButton->setVisible(false);

    // note that since saving pictures happens in a different thread,
    // we need to use a signal/slot in order to re-enable the 'take picture' button.
    QObject::connect(this,
                     SIGNAL(pictureSaved()), mTakePictureButton, SLOT(resetEnabled()));

    // using dock layout mainly.  the viewfinder foreign window sits in the center,
    // and the buttons live in their own container at the bottom.
    Container* container = Container::create()
        .layout(DockLayout::create())
        .add(Container::create()
            .horizontal(HorizontalAlignment::Center)
            .vertical(VerticalAlignment::Center)
            .add(mViewfinderWindow))
        .add(Container::create()
            .horizontal(HorizontalAlignment::Center)
            .vertical(VerticalAlignment::Bottom)
            .layout(StackLayout::create()
                .orientation(LayoutOrientation::LeftToRight))
            .add(mStartFrontButton)
            .add(mStartRearButton)
            .add(mTakePictureButton)
            .add(mStopButton));

   Application::instance()->setScene(Page::create().content(container));
}


HelloCameraApp::~HelloCameraApp()
{
    delete mViewfinderWindow;
}


void HelloCameraApp::onWindowAttached(screen_window_t win,
                                      const QString &group,
                                      const QString &id)
{
#ifdef BREAK_FWC
    // typically a value of 1ms will cause the window to only be visible the
    // first time you start it. on subsequent invocations, it does not become visible
    // unless you force a refresh of the cascades window.
    // a value of 10 ms seems to expose the problem every time.
    usleep(10000);
#endif
    qDebug() << "onWindowAttached: " << win << ", " << group << ", " << id;
    // set screen properties to mirror if this is the front-facing camera
    int i = (mCameraUnit == CAMERA_UNIT_FRONT);
    screen_set_window_property_iv(win, SCREEN_PROPERTY_MIRROR, &i);
    // put the viewfinder window behind the cascades window
    i = -1;
    screen_set_window_property_iv(win, SCREEN_PROPERTY_ZORDER, &i);
#ifdef WORKAROUND_FWC
    mViewfinderWindow->setVisible(false);
    mViewfinderWindow->setVisible(true);
#endif
}


int HelloCameraApp::createViewfinder(camera_unit_t cameraUnit,
                                     const QString &group,
                                     const QString &id)
{
    qDebug() << "createViewfinder";
    if (mCameraHandle != CAMERA_HANDLE_INVALID) {
        qDebug() << "camera already running";
        return EBUSY;
    }
    mCameraUnit = cameraUnit;
    if (camera_open(mCameraUnit,
                    CAMERA_MODE_RW | CAMERA_MODE_ROLL,
                    &mCameraHandle) != CAMERA_EOK) {
        qDebug() << "could not open camera";
        return EIO;
    }
    qDebug() << "camera opened";
    if (camera_set_photovf_property(mCameraHandle,
                                    CAMERA_IMGPROP_WIN_GROUPID, group.toStdString().c_str(),
                                    CAMERA_IMGPROP_WIN_ID, id.toStdString().c_str()) == CAMERA_EOK) {
        qDebug() << "viewfinder configured";
        if (camera_start_photo_viewfinder(mCameraHandle, NULL, NULL, NULL) == CAMERA_EOK) {
            qDebug() << "viewfinder started";
            // toggle button visibility...
            mStartFrontButton->setVisible(false);
            mStartRearButton->setVisible(false);
            mStopButton->setVisible(true);
            mTakePictureButton->setVisible(true);
            mTakePictureButton->setEnabled(true);
            return EOK;
        }
    }
    qDebug() << "couldn't start viewfinder";
    camera_close(mCameraHandle);
    mCameraHandle = CAMERA_HANDLE_INVALID;
    return EIO;
}


void HelloCameraApp::shutterCallback(camera_handle_t handle,
                                     void *arg)
{
    qDebug() << "shutterCallback";

    // THE CAMERA SERVICE DOES NOT PLAY SOUNDS WHEN PICTURES ARE TAKEN OR
    // VIDEOS ARE RECORDED.  IT IS THE APP DEVELOPER'S RESPONSIBILITY TO
    // PLAY AN AUDIBLE SHUTTER SOUND WHEN A PICTURE IS TAKEN AND WHEN VIDEO
    // RECORDING STARTS AND STOPS.  NOTE THAT WHILE YOU MAY CHOOSE TO MUTE
    // SUCH SOUNDS, YOU MUST ENSURE THAT YOUR APP ADHERES TO ALL LOCAL LAWS
    // OF REGIONS WHERE IT IS DISTRIBUTED.  FOR EXAMPLE, IT IS ILLEGAL TO
    // MUTE OR MODIFY THE SHUTTER SOUND OF A CAMERA APPLICATION IN JAPAN OR
    // KOREA.
    // TBD:
    //   RIM will be providing clarification of this policy as part of the
    //   NDK developer agreement and App World guidelines.  A link will
    //   be provided when the policy is publicly available.
    soundplayer_play_sound("event_camera_shutter");

    // this is just here to silence a compiler warning
    if (handle && arg) {}
}


void HelloCameraApp::stillCallback(camera_handle_t handle,
                                   camera_buffer_t *buf,
                                   void *arg)
{
    qDebug() << "stillCallback";
    HelloCameraApp* inst = (HelloCameraApp*)arg;
    if (buf->frametype == CAMERA_FRAMETYPE_JPEG) {
        qDebug() << "still image size: " << buf->framedesc.jpeg.bufsize;
        int fd;
        char filename[CAMERA_ROLL_NAMELEN];
        if (camera_roll_open_photo(handle,
                                   &fd,
                                   filename,
                                   sizeof(filename),
                                   CAMERA_ROLL_PHOTO_FMT_JPG) == CAMERA_EOK) {
            qDebug() << "saving " << filename;
            int index = 0;
            while(index < (int)buf->framedesc.jpeg.bufsize) {
                int rc = write(fd, &buf->framebuf[index], buf->framedesc.jpeg.bufsize-index);
                if (rc > 0) {
                    index += rc;
                } else if (rc == -1) {
                    if ((errno == EAGAIN) || (errno == EINTR)) continue;
                    qDebug() << "error saving: " << strerror(errno);
                    break;
                }
            }
            close(fd);
        }
    }
    qDebug() << "re-enabling button";
    emit inst->pictureSaved();
}


void HelloCameraApp::onStartFront()
{
    qDebug() << "onStartFront";
    if (mViewfinderWindow) {
        // create a window and see if we can catch the join
        if (createViewfinder(CAMERA_UNIT_FRONT,
                             mViewfinderWindow->windowGroup().toStdString().c_str(),
                             mViewfinderWindow->windowId().toStdString().c_str()) == EOK) {
        }
    }
}


void HelloCameraApp::onStartRear()
{
    qDebug() << "onStartRear";
    if (mViewfinderWindow) {
        // create a window and see if we can catch the join
        if (createViewfinder(CAMERA_UNIT_REAR,
                             mViewfinderWindow->windowGroup().toStdString().c_str(),
                             mViewfinderWindow->windowId().toStdString().c_str()) == EOK) {
        }
    }
}


void HelloCameraApp::onStopCamera()
{
    qDebug() << "onStopCamera";
    if (mCameraHandle != CAMERA_HANDLE_INVALID) {
        // closing the camera handle causes the viewfinder to stop which will in turn
        // cause it to detach from the foreign window
        camera_close(mCameraHandle);
        mCameraHandle = CAMERA_HANDLE_INVALID;
        // reset button visibility
        mTakePictureButton->setVisible(false);
        mStopButton->setVisible(false);
        mStartFrontButton->setVisible(true);
        mStartRearButton->setVisible(true);
    }
}


void HelloCameraApp::onTakePicture()
{
    qDebug() << "onTakePicture";
    if (mCameraHandle != CAMERA_HANDLE_INVALID) {
        camera_take_photo(mCameraHandle,
                          &shutterCallback,
                          NULL,
                          NULL,
                          &stillCallback,
                          (void*)this,
                          false);
        // disable take-picture button until we're ready to take a picture again
        mTakePictureButton->setEnabled(false);
    }
}
