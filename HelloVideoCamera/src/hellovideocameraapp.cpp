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
    // NOTE: there is a bug in 10.0.4 which forces us to re-create the
    // foreign window whenever the backing screen window is detached.
    // Using .id() in the builder is equivalent to mViewfinderWindow->setWindowId()
    mViewfinderWindow = ForeignWindow::create()
        .id(QString("cameraViewfinder"));
    QObject::connect(mViewfinderWindow,
                     SIGNAL(windowAttached(unsigned long,
                                           const QString &, const QString &)),
                     this,
                     SLOT(onWindowAttached(unsigned long,
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
    // set bitrate - it has no sane default presently
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
        camera_close(mCameraHandle);
        mCameraHandle = CAMERA_HANDLE_INVALID;
        // This is a HACK
        // ForeignWindow currently does not properly handle detach events,
        // so there is no way to disconnect and reconnect a screen window to it.
        // Instead, we have to delete the old ForeignWindow instance, and dynamically
        // create a new one to replace it.
        // We can use Container::replace() to do this.
        ForeignWindow *oldViewfinderWindow = mViewfinderWindow;
        mViewfinderWindow = ForeignWindow::create()
            .id(QString("viewfinderWindow"));
        mViewfinderWindow->setWindowId(QString("cameraViewfinder"));
        Container *container = (Container*)oldViewfinderWindow->parent();
        container->replace(container->indexOf(oldViewfinderWindow), mViewfinderWindow);
        delete oldViewfinderWindow;
        QObject::connect(mViewfinderWindow,
                         SIGNAL(windowAttached(unsigned long,
                                               const QString &, const QString &)),
                         this,
                         SLOT(onWindowAttached(unsigned long,
                              const QString &,const QString &)));
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

            // LEGAL REQUIREMENTS DICTATE THAT ALL CAMERA APPS MUST PRODUCE AN AUDIBLE
            // TONE WHEN RECORDING FUNCTION IS STARTED.  DO THIS OR YOUR APP WILL BE
            // PULLED FROM APP WORLD.
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
