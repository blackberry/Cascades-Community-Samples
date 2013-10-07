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
#ifndef __HELLOVIDEOCAMERAAPP_HPP__
#define __HELLOVIDEOCAMERAAPP_HPP__

#include <QtCore/QObject>
#include <QtCore/QMetaType>

#include <bb/cascades/ForeignWindowControl>
#include <bb/cascades/Button>
#include <bb/cascades/Label>
#include <bb/cascades/Application>
#include <bb/cascades/OrientationSupport>
#include <bb/cascades/DisplayDirection>


#include <camera/camera_api.h>

using namespace bb::cascades;


class HelloVideoCameraApp : public QObject
{
    Q_OBJECT
public slots:
    void onWindowAttached(screen_window_t win,
                          const QString &group,
                          const QString &id);
    void onStartFront();
    void onStartRear();
    void onStopCamera();
    void onStartStopRecording();
    void onDisplayDirectionAboutToChange(bb::cascades::DisplayDirection::Type direction,
                                         bb::cascades::UIOrientation::Type orientation);


public:
    HelloVideoCameraApp(bb::cascades::Application *app);
    ~HelloVideoCameraApp();

private:
    int createViewfinder(camera_unit_t cameraUnit,
                         const QString &group,
                         const QString &id);
    void updateAngles(int displayDirection);
    void applyRotations(bool flush);
    void querySupportedRotations();

    ForeignWindowControl *mViewfinderWindow;
    Button *mStartFrontButton;
    Button *mStartRearButton;
    Button *mStopButton;
    Button *mStartStopButton;
    Label *mStatusLabel;
    camera_handle_t mCameraHandle;
    camera_unit_t mCameraUnit;
    bool mShouldMirror;
    int mVideoFileDescriptor;
    int mDisplayDirection;         // the reported device orientation
    uint32_t mDesiredAngle;             // the overall display angle desired
    uint32_t mWindowAngle;              // the screen window rotation angle
    uint32_t mVfAngle;                  // the camera viewfinder rotation angle
    uint32_t mVideoAngle;               // the video rotation angle (if available)
    bool mRequireUprightBuffers;   // set to true to force video to be upright, otherwise camera rotation is left unchanged
    bool mVideoAndVfAreIndependent; // set to true if the platform supports independent video and vf configs (not likely)
    int mNumVfRotations;           // reported # of camera viewfinder rotation angles
    int mNumVideoRotations;        // reported # of camera video rotation angles (if supported)
    uint32_t *mSupportedVfRotations;    // the reported list of supported viewfinder rotation angles
    uint32_t *mSupportedVideoRotations; // the reported list of supported video rotation angles (if supported)
};

#endif // ifndef __HELLOVIDEOCAMERAAPP_HPP__
