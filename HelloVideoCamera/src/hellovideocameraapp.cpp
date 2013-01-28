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
#include <libgen.h>
#include "hellovideocameraapp.hpp"

using namespace bb::cascades;


HelloVideoCameraApp::HelloVideoCameraApp(bb::cascades::Application *app) :
        QObject(app),
        mCameraHandle(CAMERA_HANDLE_INVALID),
        mVideoFileDescriptor(-1)
{
    qDebug() << "HelloVideoCameraApp";

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
    mStartStopButton = Button::create("Record Start")
        .onClicked(this, SLOT(onStartStopRecording()));
    mStartStopButton->setVisible(false);

    mStatusLabel = Label::create("filename");
    mStatusLabel->setVisible(false);

    // using dock layout mainly.  the viewfinder foreign window sits in the center,
    // and the buttons live in their own container at the bottom.
    // a single text label sits at the top of the screen to report recording status.
    Container* container = Container::create()
        .layout(DockLayout::create())
        .add(Container::create()
            .horizontal(HorizontalAlignment::Center)
            .vertical(VerticalAlignment::Center)
            .add(mViewfinderWindow))
        .add(Container::create()
            .horizontal(HorizontalAlignment::Left)
            .vertical(VerticalAlignment::Top)
            .add(mStatusLabel))
        .add(Container::create()
            .horizontal(HorizontalAlignment::Center)
            .vertical(VerticalAlignment::Bottom)
            .layout(StackLayout::create()
    			.orientation(LayoutOrientation::LeftToRight))
			.add(mStartFrontButton)
			.add(mStartRearButton)
			.add(mStartStopButton)
			.add(mStopButton));


    app->setScene(Page::create().content(container));
}


HelloVideoCameraApp::~HelloVideoCameraApp()
{
}


void HelloVideoCameraApp::onWindowAttached(screen_window_t win,
                                          const QString &group,
                                          const QString &id)
{
    qDebug() << "onWindowAttached: " << win << ", " << group << ", " << id;
    // set screen properties to mirror if this is the front-facing camera
    int i = (mCameraUnit == CAMERA_UNIT_FRONT);
    screen_set_window_property_iv(win, SCREEN_PROPERTY_MIRROR, &i);
    // put the viewfinder window behind the cascades window
    i = -1;
    screen_set_window_property_iv(win, SCREEN_PROPERTY_ZORDER, &i);
    screen_context_t screen_ctx;
    screen_get_window_property_pv(win, SCREEN_PROPERTY_CONTEXT, (void **)&screen_ctx);
    screen_flush_context(screen_ctx, 0);
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
        // create a window and see if we can catch the join
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
        // create a window and see if we can catch the join
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
        // closing the camera handle causes the viewfinder to stop which will in turn
        // cause it to detach from the foreign window
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

            // NOTE: we use the _blocking variant here so that the sound doesn't bleed over
            // into our recording.  An alternate solution may involve muting the mic temporarily,
            // in order to allow video recording to start slightly sooner.
            soundplayer_play_sound_blocking("event_recording_start");

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
                    mStopButton->setEnabled(false);
                    mStatusLabel->setText(basename(filename));
                    mStatusLabel->setVisible(true);
                    return;
                }
                qDebug() << "failed to start recording";
                camera_roll_close_video(mVideoFileDescriptor);
                mVideoFileDescriptor = -1;
            }
            soundplayer_play_sound("event_recording_stop");
        } else {
            soundplayer_play_sound("event_recording_stop");
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
