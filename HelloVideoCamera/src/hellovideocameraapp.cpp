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
#include <bb/cascades/ForeignWindow>
#include <bb/cascades/Container>
#include <bb/cascades/StackLayout>
#include <bb/cascades/DockLayout>
#include <bb/cascades/DockLayoutProperties>
#include <bb/cascades/Button>
#include <bb/cascades/Label>
#include <bb/cascades/Page>

#include <camera/camera_api.h>
#include <screen/screen.h>
#include <bps/soundplayer.h>
#include <unistd.h>
#include <libgen.h>
#include "hellovideocameraapp.hpp"

using namespace bb::cascades;

HelloVideoCameraApp::HelloVideoCameraApp() :
        mCameraHandle(CAMERA_HANDLE_INVALID),
        mVideoFileDescriptor(-1)
{
    qDebug() << "HelloVideoCameraApp";

    // create our foreign window
    // Using .id() in the builder is equivalent to mViewfinderWindow->setWindowId()
    mViewfinderWindow = ForeignWindow::create()
        .id(QString("cameraViewfinder"));
    // NOTE that there is a bug in ForeignWindow in 10.0.6 whereby the
    // SCREEN_PROPERTY_SOURCE_SIZE is updated when windows are attached.
    // We don't want this to happen, so we are disabling WindowFrameUpdates.
    // What this means is that if the ForeignWindow geometry is changed, then
    // the underlying screen window properties are not automatically updated to
    // match.  You will have to manually do so by listening for controlFrameChanged
    // signals.  This is outside of the scope of this sample.
    mViewfinderWindow->setWindowFrameUpdateEnabled(false);

    QObject::connect(mViewfinderWindow,
                     SIGNAL(windowAttached(unsigned long,
                                           const QString &, const QString &)),
                     this,
                     SLOT(onWindowAttached(unsigned long,
                          const QString &,const QString &)));

    // NOTE that there is a bug in ForeignWindow in 10.0.6 whereby
    // when a window is detached, it's windowHandle is not reset to 0.
    // We need to connect a detach handler to implement a workaround.
    QObject::connect(mViewfinderWindow,
                     SIGNAL(windowDetached(unsigned long,
                                           const QString &, const QString &)),
                     this,
                     SLOT(onWindowDetached(unsigned long,
                          const QString &,const QString &)));

    // create a bunch of camera control buttons
    // NOTE: some of these buttons are not initially visible
    mStartFrontButton = Button::create("Front Camera");
    mStartRearButton = Button::create("Rear Camera");
    mStopButton = Button::create("Stop Camera");
    mStopButton->setVisible(false);
    mStartStopButton = Button::create("Record Start");
    mStartStopButton->setVisible(false);
    // connect actions to the buttons
    QObject::connect(mStartFrontButton,
                     SIGNAL(clicked()), this, SLOT(onStartFront()));
    QObject::connect(mStartRearButton,
                     SIGNAL(clicked()), this, SLOT(onStartRear()));
    QObject::connect(mStopButton,
                     SIGNAL(clicked()), this, SLOT(onStopCamera()));
    QObject::connect(mStartStopButton,
                     SIGNAL(clicked()), this, SLOT(onStartStopRecording()));
    mStatusLabel = Label::create("filename");
    mStatusLabel->setVisible(false);

    // using dock layout mainly.  the viewfinder foreign window sits in the center,
    // and the buttons live in their own container at the bottom.
    // a single text label sits at the top of the screen to report recording status.
    Container* container = Container::create()
        .layout(DockLayout::create())
        .add(Container::create()
            .layoutProperties(DockLayoutProperties::create()
                .horizontal(HorizontalAlignment::Center)
                .vertical(VerticalAlignment::Center))
            .add(mViewfinderWindow))
        .add(Container::create()
            .layoutProperties(DockLayoutProperties::create()
                .horizontal(HorizontalAlignment::Left)
                .vertical(VerticalAlignment::Top))
            .add(mStatusLabel))
        .add(Container::create()
            .layoutProperties(DockLayoutProperties::create()
                .horizontal(HorizontalAlignment::Center)
                .vertical(VerticalAlignment::Bottom))
            .layout(StackLayout::create()
                .direction(LayoutDirection::LeftToRight))
            .add(mStartFrontButton)
            .add(mStartRearButton)
            .add(mStartStopButton)
            .add(mStopButton));

   Application::setScene(Page::create().content(container));
}


HelloVideoCameraApp::~HelloVideoCameraApp()
{
    delete mViewfinderWindow;
}


void HelloVideoCameraApp::onWindowAttached(unsigned long handle,
                                           const QString &group,
                                           const QString &id)
{
    qDebug() << "onWindowAttached: " << handle << ", " << group << ", " << id;
    screen_window_t win = (screen_window_t)handle;
    // set screen properties to mirror if this is the front-facing camera
    int i = (mCameraUnit == CAMERA_UNIT_FRONT);
    screen_set_window_property_iv(win, SCREEN_PROPERTY_MIRROR, &i);
    // put the viewfinder window behind the cascades window
    i = -1;
    screen_set_window_property_iv(win, SCREEN_PROPERTY_ZORDER, &i);
    // make the window visible.  by default, the camera creates an invisible
    // viewfinder, so that the user can decide when and where to place it
    i = 1;
    screen_set_window_property_iv(win, SCREEN_PROPERTY_VISIBLE, &i);
    // There is a bug in ForeignWindow in 10.0.6 which defers window context
    // flushing until some future UI update.  As a result, the window will
    // not actually be visible until someone flushes the context.  This is
    // fixed in the next release.  For now, we will just manually flush the
    // window context.
    screen_context_t ctx;
    screen_get_window_property_pv(win, SCREEN_PROPERTY_CONTEXT, (void**)&ctx);
    screen_flush_context(ctx, 0);
}


void HelloVideoCameraApp::onWindowDetached(unsigned long handle,
                                           const QString &group,
                                           const QString &id)
{
    qDebug() << "onWindowDetached: " << handle << ", " << group << ", " << id;
    // There is a bug in ForeignWindow in 10.0.6 whereby the windowHandle is not
    // reset to 0 when a detach event happens.  We must forcefully zero it here
    // in order for a re-attach to work again in the future.
    mViewfinderWindow->setWindowHandle(0);
}



int HelloVideoCameraApp::createViewfinder(camera_unit_t cameraUnit,
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
    // configure viewfinder properties so our ForeignWindow can find the resulting screen window
    if (camera_set_videovf_property(mCameraHandle,
                                    CAMERA_IMGPROP_WIN_GROUPID, group.toStdString().c_str(),
                                    CAMERA_IMGPROP_WIN_ID, id.toStdString().c_str()) == CAMERA_EOK) {
        qDebug() << "viewfinder configured";
        if (camera_start_video_viewfinder(mCameraHandle, NULL, NULL, NULL) == CAMERA_EOK) {
            qDebug() << "viewfinder started";
            // toggle button visibility...
            mStartFrontButton->setVisible(false);
            mStartRearButton->setVisible(false);
            mStopButton->setVisible(true);
            mStartStopButton->setText("Start Recording");
            mStartStopButton->setVisible(true);
            mStartStopButton->setEnabled(true);
            return EOK;
        }
    }
    qDebug() << "couldn't start viewfinder";
    camera_close(mCameraHandle);
    mCameraHandle = CAMERA_HANDLE_INVALID;
    return EIO;
}


void HelloVideoCameraApp::onStartFront()
{
    qDebug() << "onStartFront";
    if (mViewfinderWindow) {
        if (createViewfinder(CAMERA_UNIT_FRONT,
                             mViewfinderWindow->windowGroup().toStdString().c_str(),
                             mViewfinderWindow->windowId().toStdString().c_str()) == EOK) {
            qDebug() << "created viewfinder";
        }
    }
}


void HelloVideoCameraApp::onStartRear()
{
    qDebug() << "onStartRear";
    if (mViewfinderWindow) {
        if (createViewfinder(CAMERA_UNIT_REAR,
                             mViewfinderWindow->windowGroup().toStdString().c_str(),
                             mViewfinderWindow->windowId().toStdString().c_str()) == EOK) {
            qDebug() << "created viewfinder";
        }
    }
}


void HelloVideoCameraApp::onStopCamera()
{
    qDebug() << "onStopCamera";
    if (mCameraHandle != CAMERA_HANDLE_INVALID) {
        // NOTE that closing the camera causes the viewfinder to stop.
        // When the viewfinder stops, it's window is destroyed and the
        // ForeignWindow object will emit a windowDetached signal.
        camera_close(mCameraHandle);
        mCameraHandle = CAMERA_HANDLE_INVALID;
        // reset button visibility
        mStartStopButton->setVisible(false);
        mStopButton->setVisible(false);
        mStartFrontButton->setVisible(true);
        mStartRearButton->setVisible(true);

    }
}


void HelloVideoCameraApp::onStartStopRecording()
{
    qDebug() << "onStartStopRecording";
    if (mCameraHandle != CAMERA_HANDLE_INVALID) {
        // this button is being treated as a toggle.
        // when mVideoFileDescriptor is -1, the button is treated as a "start" button,
        // otherwise, it is treated as a "stop" button.
        if (mVideoFileDescriptor == -1) {

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
            soundplayer_play_sound("event_video_record_start_sound");

            char filename[CAMERA_ROLL_NAMELEN];
            if (camera_roll_open_video(mCameraHandle,
                                       &mVideoFileDescriptor,
                                       filename,
                                       sizeof(filename),
                                       CAMERA_ROLL_VIDEO_FMT_DEFAULT) == CAMERA_EOK) {
                qDebug() << "opened " << filename;
                if (camera_start_video(mCameraHandle,
                                       filename,
                                       NULL,
                                       NULL,
                                       NULL) == CAMERA_EOK) {
                    qDebug() << "started recording";
                    mStartStopButton->setText("Stop Recording");
                    mVideoFileDescriptor = 0;
                    mStopButton->setEnabled(false);
                    mStatusLabel->setText(basename(filename));
                    mStatusLabel->setVisible(true);
                    return;
                }
                qDebug() << "failed to start recording";
                camera_roll_close_video(mVideoFileDescriptor);
                mVideoFileDescriptor = -1;
            }
            soundplayer_play_sound("event_video_record_stop_sound");
        } else {
            soundplayer_play_sound("event_video_record_stop_sound");
            camera_stop_video(mCameraHandle);
            qDebug() << "stopped recording";
            camera_roll_close_video(mVideoFileDescriptor);
            mVideoFileDescriptor = -1;
            mStartStopButton->setText("Start Recording");
            mStopButton->setEnabled(true);
            mStatusLabel->setVisible(false);
        }
    }
}
