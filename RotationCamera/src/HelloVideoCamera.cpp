/* Copyright (c) 2013 Research In Motion Limited.
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
#include <bb/cascades/OrientationSupport>

#include <camera/camera_api.h>
#include <screen/screen.h>
#include <bps/soundplayer.h>
#include <unistd.h>
#include <libgen.h>
#include "HelloVideoCamera.hpp"

using namespace bb::cascades;

// qDebug() now logs to slogger2, which I find inconvenient since the NDK does not pick this up in the console,
// so I am installing a custom handler to log to stderr.
static void log_to_stderr(QtMsgType msgType, const char *msg)
{
    (void)msgType;  // go away, warning!
    fprintf(stderr, "%s\n", msg);
}


HelloVideoCameraApp::HelloVideoCameraApp(bb::cascades::Application *app) :
        QObject(app),
        mCameraHandle(CAMERA_HANDLE_INVALID),
        mVideoFileDescriptor(-1),
        mRequireUprightBuffers(false),
        mVideoAndVfAreIndependent(false),
        mNumVfRotations(0),
        mNumVideoRotations(0),
        mSupportedVfRotations(NULL),
        mSupportedVideoRotations(NULL)
{
    qInstallMsgHandler(log_to_stderr);

    qDebug() << "HelloVideoCameraApp";

    // figure out initial orientation and calculate desired camera angles
    updateAngles(OrientationSupport::instance()->displayDirection());

    QObject::connect(
            OrientationSupport::instance(),
            // previously, I was using displayDirectionChanged(), but things may look better if we
            // update the window before the rotation effect happens
            SIGNAL(displayDirectionAboutToChange(bb::cascades::DisplayDirection::Type,
                                                 bb::cascades::UIOrientation::Type)),
            this,
            SLOT(onDisplayDirectionAboutToChange(bb::cascades::DisplayDirection::Type,
                                                 bb::cascades::UIOrientation::Type)));

    // indicate all orientations are supported
    OrientationSupport::instance()->setSupportedDisplayOrientation(
                SupportedDisplayOrientation::All);

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
    // put the viewfinder window behind the cascades window
    int i = -1;
    screen_set_window_property_iv(win, SCREEN_PROPERTY_ZORDER, &i);
    updateAngles(OrientationSupport::instance()->displayDirection());
    applyRotations(false);
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
    // front-facing devices need window mirroring
    mShouldMirror = (mCameraUnit == CAMERA_UNIT_FRONT);
    // read supported rotation angles from the camera
    querySupportedRotations();
    // query initial rotation angles.. these will be whatever the platform selected
    // as defaults.
    // these values are guaranteed to be supported for the default frametype.
    // if that worries you, then you could instead query camera_get_native_orientation().
    // that angle is guaranteed to always be available across all frametypes.
    if (camera_get_video_property(mCameraHandle,
                                  CAMERA_IMGPROP_ROTATION, &mVideoAngle) == CAMERA_EOK) {
        qDebug() << "initial video angle" << mVideoAngle;
    }
    if (camera_get_videovf_property(mCameraHandle,
                                    CAMERA_IMGPROP_ROTATION, &mVfAngle) == CAMERA_EOK) {
        qDebug() << "initial viewfinder angle" << mVfAngle;
    }
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
        camera_stop_video_viewfinder(mCameraHandle);
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

            // re-compute video rotation settings with upright=true
            mRequireUprightBuffers = true;
            updateAngles(mDisplayDirection);
            // force an update of rotations and a flush of the window context
            applyRotations(true);
            // after this has been applied, let's clear the upright flag.
            // this ensures that we don't try to reconfigure video or viewfinder rotation if someone rotates
            // the device while recording.  that should be rejected anyways.  you cannot resize buffers while
            // encoding video.  a better solution is typically to apply an orientation lock at this point,
            // and release it when recording stops.
            mRequireUprightBuffers = false;
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


void HelloVideoCameraApp::applyRotations(bool flush)
{
    // update video and viewfinder rotation
    camera_set_videovf_property(mCameraHandle, CAMERA_IMGPROP_ROTATION, mVfAngle);
    camera_set_video_property(mCameraHandle, CAMERA_IMGPROP_ROTATION, mVideoAngle);

    // update window rotation
    if (mViewfinderWindow->isBoundToWindow()) {
        screen_window_t win = mViewfinderWindow->windowHandle();
        screen_set_window_property_iv(win, SCREEN_PROPERTY_ROTATION, (int*)&mWindowAngle);
        int i = mShouldMirror;
        screen_set_window_property_iv(win, SCREEN_PROPERTY_MIRROR, &i);
        if (flush) {
            // flush screen context to update window NOW?
            screen_context_t screen_ctx;
            screen_get_window_property_pv(win, SCREEN_PROPERTY_CONTEXT, (void **)&screen_ctx);
            screen_flush_context(screen_ctx, 0);
        }
    }
}


void HelloVideoCameraApp::onDisplayDirectionAboutToChange(bb::cascades::DisplayDirection::Type direction,
                                                          bb::cascades::UIOrientation::Type orientation)
{
    updateAngles(direction);
    qDebug() << "onDisplayDirectionAboutToChange() " << mDisplayDirection;
    qDebug() << "orientation " << orientation;
    // apply rotations, but don't bother flushing context since we're about to hit a big graphics update.
    // if we were calling this function
    applyRotations(false);
}


// this should be a COMPLETE example of how to handle video rotation in all expected use cases.
// the important cases are:
// 1. don't care about aligning buffers, so just update window angle and leave buffer angles as-is
// 2. video buffers need to be upright for video encoding
// 3. video buffers cannot be rotated independently of viewfinder, so set both to same value
// the rotation equation is generally:
// desired angle = viewfinder angle + window angle
// where:
// desired angle = complement of display direction angle
void HelloVideoCameraApp::updateAngles(int displayDirection)
{
    int i;

    mDisplayDirection = displayDirection;

    // For a camera facing in the same direction as the user (forward), the desired display angle is the complement
    // of the display direction.  This is because the nav display direction signals are reported as:
    // "the edge of the device which is now topmost".  In the camera's space, we report
    // angles as clockwise rotations of a buffer, or of the device.  So if nav reports that "90" is the new
    // display direction, that means that the 90-degree-clockwise edge (or 3-o'clock edge) of the device is
    // now topmost.  that means that the device has actually been rotated 90 degrees counterclockwise.
    // 90 degrees counterclockwise is equivalent to 270 degrees clockwise.  We want our picture to be rotated by
    // the same amount (270 degrees clockwise), therefore we use the complement of navigator's reported "90".
    // Here is an important distinction...
    // For a camera which faces in the opposite direction as the user (eg. backwards - towards the user), the
    // angle that the device is being rotated needs to be reversed.  This is because from the camera's perspective
    // (standing behind the device, facing the user), the device has been rotated in the opposite direction fron that which
    // the user would perceive.  Once you understand this distinction, the front/rear decisions below will make more sense.
    // Although confusingly, CAMERA_UNIT_REAR is the camera facing in the same direction as the user (it faces out
    // the rear of the device), and CAMERA_UNIT_FRONT is the camera facing towards the user.

    // here, I will reverse the nav's reported rotation, to bring it in line with the screen and camera's
    // co-ordinate reference.  eg. turning the device clockwise yields a rotation of 90 (nav reports 270)
    int deviceOrientation = (360 - mDisplayDirection) % 360;

    // now account for front/rear-facing camera reversal of rotation direction
    if (mCameraUnit == CAMERA_UNIT_FRONT) {
        mDesiredAngle = (360 - deviceOrientation) % 360;
    } else {
        mDesiredAngle = deviceOrientation;
    }

    // upright buffers are usually only required for video encoding or any image processing that
    // demands that the upper-left pixel of the buffer correspond to the upper-left pixel of the scene.
    // for photo viewfinders, this is usually not the case, as JPEG supports EXIF metadata that
    // describes how to display the uncompressed photo.  (MP4 does not, hence the reason we require this
    // for video recording).
    if (mRequireUprightBuffers) {
        if (mCameraHandle == CAMERA_HANDLE_INVALID) {
            // no camera open.. don't bother picking an angle for camera buffers yet
            mVfAngle = 0;
            mVideoAngle = 0;
        } else {
            // camera is open.. pick a video rotation first.  this is the rotation applied
            // to camera buffers which are used by the video encoder.
            for (i=0; i<mNumVideoRotations; i++) {
                if (mSupportedVideoRotations[i] == mDesiredAngle) {
                    mVideoAngle = mDesiredAngle;
                    break;
                }
            }
            if (i == mNumVideoRotations) {
                // could not select the required video angle!  give up?
                qDebug() << "video rotation " << mDesiredAngle << " not supported.";
                return;
            }
            // now that video angle has been selected, determine if the VF angle has to match
            if (!mVideoAndVfAreIndependent) {
                // has to match, since there is only one frame stream from the camera for both
                // viewfinder and video encoding
                mVfAngle = mVideoAngle;
            } else {
                // this platform supports independently configurable viewfinder and video streams,
                // so let's not actually bother to update the viewfinder angle.
                // since reconfiguring the viewfinder angle actually incurs a disruption to the viewfinder while
                // buffers are re-allocated, we want to minimize changes to mVfAngle
            }
        }
    } else {
        // we don't care about presenting upright video or viewfinder buffers.
        // since reconfiguring the viewfinder angle actually incurs a disruption to the viewfinder while
        // buffers are re-allocated, we want to minimize changes to mVfAngle.
        // for that reason.. don't bother changing mVfAngle at all.
    }

    // compute screen display angle now that we know how the viewfinder buffers are being rotated.
    // remember: desired angle = viewfinder angle + window angle.  solve for viewfinder angle:
    mWindowAngle = (360 + mDesiredAngle - mVfAngle) % 360;  // note +360 to avoid negative numbers in modulo math

    // lastly, let's deal with the mirroring situation...
    if (mCameraUnit == CAMERA_UNIT_FRONT) {
        // we have to reverse the direction of rotation for the screen window to
        // account for the fact that the camera's view is from behind the screen, while
        // the user's view is from in front of the screen.
        // If you find you have upside-down video in your app sometimes, it is likely you
        // missed this step.  Applying a matrix of mirror and flip flags to the window would
        // also solve the problem, but I find the rotation solution simpler to code. In fact,
        // since the matrix of flip & mirror flags is rather convoluted, I believe it is due to
        // order of operations in the windowing system (mirror and then rotate vs rotate and then mirror).
        mWindowAngle = (360 - mWindowAngle) % 360;
        // and we will always mirror the end result so the user is not disoriented.
        // note that video is actually recorded unmirrored.. only the screen gets a mirror
        // effect applied.
        mShouldMirror = true;
    } else {
        mShouldMirror = false;
    }
    qDebug() << "WIN:" << mWindowAngle << "VF:" << mVfAngle << "VID:" << mVideoAngle
             << "MIRROR:" << mShouldMirror;
}


void HelloVideoCameraApp::querySupportedRotations()
{
    // camera is open.. we want to identify the supported rotations for viewfinder and video.
    // we also want to determine if this platform supports independent configuration of each.
    if (camera_has_feature(mCameraHandle, CAMERA_FEATURE_PREVIEWISVIDEO)) {
        mVideoAndVfAreIndependent = false;
    } else {
        // no currently shipping platforms currently support this, but the code is provided here
        // as reference for future-proofing your implementation.
        mVideoAndVfAreIndependent = true;
    }
    // clean up previously allocated arrays
    if (mSupportedVideoRotations) {
        delete[]mSupportedVideoRotations;
        mSupportedVideoRotations = NULL;
    }
    mNumVideoRotations = 0;
    if (mSupportedVfRotations) {
        delete[]mSupportedVfRotations;
        mSupportedVfRotations = NULL;
    }
    mNumVfRotations = 0;
    camera_frametype_t frametype;
    // find out what format viewfinder is configured to use... most likely this is NV12
    if (camera_get_videovf_property(mCameraHandle,
                                    CAMERA_IMGPROP_FORMAT, &frametype)) {
        qDebug() << "error reading video format";
        return;
    }
    // determine supported rotations for vf with that frametype
    if (camera_get_video_vf_rotations(mCameraHandle, frametype, 0, &mNumVfRotations, NULL, NULL)) {
        qDebug() << "error querying num vf rotations";
    }
    // allocate array
    mSupportedVfRotations = new uint32_t[mNumVfRotations];
    if (camera_get_video_vf_rotations(mCameraHandle,
                                      frametype,
                                      mNumVfRotations,
                                      &mNumVfRotations,
                                      mSupportedVfRotations,
                                      NULL)) {
        qDebug() << "error querying vf rotations";
    }
    if (mVideoAndVfAreIndependent) {
        // find out what format video is configured to use... most likely this is NV12.
        // though, if video and viewfinder are not independent, then it is the same as the vf stream
        if (camera_get_videovf_property(mCameraHandle,
                                        CAMERA_IMGPROP_FORMAT, &frametype)) {
            qDebug() << "error reading video format";
            return;
        }
    }
    // determine supported rotations for video with that frametype
    if (camera_get_video_rotations(mCameraHandle, frametype, 0, &mNumVideoRotations, NULL, NULL)) {
        qDebug() << "error querying num video rotations";
    }
    // allocate array
    mSupportedVideoRotations = new uint32_t[mNumVideoRotations];
    if (camera_get_video_rotations(mCameraHandle,
                                   frametype,
                                   mNumVideoRotations,
                                   &mNumVideoRotations,
                                   mSupportedVideoRotations,
                                   NULL)) {
        qDebug() << "error querying video rotations";
    }
}
