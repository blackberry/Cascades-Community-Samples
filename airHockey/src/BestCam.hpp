#ifndef __BESTCAM_HPP__
#define __BESTCAM_HPP__

/* Copyright (c) 2013 BlackBerry Limited
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

#include <QObject>
#include <QMetaType>
#include <QThread>
#include <camera/camera_api.h>
#include <bb/cascades/DisplayDirection>
#include <bb/cascades/UIOrientation>
#include <bb/cascades/Label>
#include <QRectF>
#include "messaging/BTController.hpp"
#include "messaging/ArduinoBTController.hpp"

#include "connection/MacAddressHandler.hpp"


#include <pthread.h>



namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
        class AbstractPane;
        class ForeignWindowControl;
        class Label;

    }
}

namespace QtMobility
{
    class QOrientationSensor;
}

class QTranslator;
class BestCam;
class MacAddressHandler;
class ArduinoBTController;

/*!
 * @brief Application object
 *
 *
 */

class StatusThread : public QThread {
    Q_OBJECT
public:
    StatusThread(BestCam* cam);
    void run();
    void cleanShutdown();
signals:
    void statusChanged(camera_devstatus_t, uint16_t);
private:
    BestCam* mCam;
    camera_handle_t mHandle;
    bool mStop;
    int mChId;
    int mCoId;
    struct sigevent mEvent;
    static const int mPulseId = 123;
};


//Stephens hacking:
//Make a vector2 struct type for positions ect.
struct Vector2 {
        float x;
        float y;
};

enum MotorPositions {
    armLeft    = -1,
    armStop    =  0,
    armRight   =  1
};

class BestCam : public QObject
{
    Q_OBJECT
public:
    BestCam(bb::cascades::Application *app);
    virtual ~BestCam() {}

    friend class StatusThread;  // TODO - formalize interface & remove friendship

    Q_ENUMS(VfMode);
    Q_ENUMS(CameraUnit);

    // an enum we can expose to QML
    enum CameraUnit {
        UnitNone = CAMERA_UNIT_NONE,
        UnitFront = CAMERA_UNIT_FRONT,
        UnitRear = CAMERA_UNIT_REAR
    };

    enum VfMode {
        ModeNone = 0,
        ModePhoto,
        ModeVideo,
    };

    enum CamState {
        StateIdle = 0,
        StateStartingPhotoVf,
        StatePhotoVf,
        StatePhotoCapture,
        StateStartingVideoVf,
        StateVideoVf,
        StateVideoCapture,
        StatePowerDown,
        StateMinimized
    };

    enum GoalPosition {
        Left = 0,
        Center,
        Right
    };

    //Stephen Hacking: Motor control functions:
    void motorLeft();
    void motorRight();
    void motorStop();

    void moveArmTenPercentLeft();
    void moveArmTenPercentRight();

    void moveArmToPositionLeft();
    void moveArmToPositionRight();
    void moveArmToPositionCenter();

    Q_PROPERTY(CameraUnit cameraUnit READ cameraUnit NOTIFY cameraUnitChanged);
    CameraUnit cameraUnit() const { return (CameraUnit)mUnit; }

    Q_PROPERTY(VfMode vfMode READ vfMode NOTIFY vfModeChanged);
    VfMode vfMode() const { return mVfMode; }

    Q_PROPERTY(bool hasFrontCamera READ hasFrontCamera NOTIFY hasFrontCameraChanged)
    bool hasFrontCamera() const { return mHasFrontCamera; }

    Q_PROPERTY(bool hasRearCamera READ hasRearCamera NOTIFY hasRearCameraChanged)
    bool hasRearCamera() const { return mHasRearCamera; }

    Q_PROPERTY(bool canDoPhoto READ canDoPhoto NOTIFY canDoPhotoChanged)
    bool canDoPhoto() const { return mCanDoPhoto; }

    Q_PROPERTY(bool canDoVideo READ canDoVideo NOTIFY canDoVideoChanged)
    bool canDoVideo() const { return mCanDoVideo; }

    Q_PROPERTY(bool canCapture READ canCapture NOTIFY canCaptureChanged)
    bool canCapture() const { return mCanCapture; }

    Q_PROPERTY(bool capturing READ capturing NOTIFY capturingChanged)
    bool capturing() const { return mCapturing; }

    Q_PROPERTY(Vector2 currentPosition READ currentPosition NOTIFY currentPositionChanged)
    Vector2 currentPosition() const { return mCurrentPosition; }
//    Q_PROPERTY(int currentYPosition READ currentYPosition NOTIFY currentYPositionChanged)
//    int currentYPosition() const { return (int) pos.x; }

    Q_INVOKABLE
    int setCameraUnit(CameraUnit unit);

    Q_INVOKABLE
    int setVfMode(VfMode mode);

    Q_INVOKABLE
    int windowAttached();

    Q_INVOKABLE
    int capture();

    Q_INVOKABLE
//    void setCurrentPosition(Vector2 currentPosition);

    bb::cascades::AbstractPane* getRoot(void) { return mRoot;}

signals:
    void cameraUnitChanged(CameraUnit);
    void vfModeChanged(VfMode);
    void hasFrontCameraChanged(bool);
    void hasRearCameraChanged(bool);
    void canDoPhotoChanged(bool);
    void canDoVideoChanged(bool);
    void canCaptureChanged(bool);
    void suppressStartChanged(bool);
    void captureComplete(int);
    void capturingChanged(bool);
//    void wantsToMoveArm(int direction);

    Q_SIGNALS:
    void currentPositionChanged(Vector2 pos);

public slots:
    void onSystemLanguageChanged();
    void onCaptureComplete(int err);
    void onStatusChanged(camera_devstatus_t status, uint16_t extra);
    void onFullscreen();
    void onThumbnail();
    void onInvisible();
    void onDisplayDirectionChanging(bb::cascades::DisplayDirection::Type displayDirection,
                                    bb::cascades::UIOrientation::Type orientation);
    void onOrientationReadingChanged();
    void onVfParentLayoutFrameChanged(QRectF frame);

private:
    int runStateMachine(CamState newState);
    int enterState(CamState state, CamState &nextState);
    int exitState();
    void inventoryCameras();
    int openCamera(camera_unit_t unit);
    int closeCamera();
    int startPhotoVf();
    int stopPhotoVf();
    int startVideoVf();
    int stopVideoVf();
    int startRecording();
    int stopRecording();
    int orientationToAngle(bb::cascades::UIOrientation::Type orientation);
    void resourceWarning();
    void poweringDown();
    void takePhoto();
    static void shutterCallbackEntry(camera_handle_t handle, void* arg) { ((BestCam*)arg)->shutterCallback(handle); }
    void shutterCallback(camera_handle_t handle);
    static void stillCallbackEntry(camera_handle_t handle, camera_buffer_t* buf, void* arg) { ((BestCam*)arg)->stillCallback(handle, buf); }
    void stillCallback(camera_handle_t handle, camera_buffer_t* buf);
    void updateAngles();
    void updatePhotoAngle();
    void updateVideoAngle();
    // using QOrientationSensor in order to know which way is up
    int startOrientationReadings();
    void stopOrientationReadings();
    void constrainViewfinderAspectRatio();
    int discoverCameraCapabilities();
    int discoverPhotoCapabilities();
    int discoverPhotoVfCapabilities();
    int discoverVideoCapabilities();
    int discoverVideoVfCapabilities();
    camera_res_t* matchAspectRatio(camera_res_t* target, camera_res_t* resList, int numRes, float accuracy);

    const char* stateName(CamState state);

    //Stephen hacking:
    static void videoFrameCallbackEntry(camera_handle_t handle, camera_buffer_t * buf, void * arg) {((BestCam*)arg)->videoFrameCallback(handle, buf); }
    void videoFrameCallback(camera_handle_t cameraHandle, camera_buffer_t * pCameraBuffer);

    void createMacAddressHandler();
    void createArduinoController();


    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;
    bb::cascades::AbstractPane* mRoot;
    bb::cascades::ForeignWindowControl* mFwc;

    //Stephen hacking:
    bb::cascades::Label *mCurrentPositionLabel;

    camera_unit_t mUnit;
    camera_handle_t mHandle;
    VfMode mVfMode;
    VfMode mResumeVfMode;
    bool mHasFrontCamera;
    bool mHasRearCamera;
    bool mCanDoPhoto;
    bool mCanDoVideo;
    bool mCanCapture;
    bool mCapturing;
    //STEPHEN HACKING::
    Vector2 mCurrentPosition;
    pthread_t *mpArmMotorThread_tid;
    MacAddressHandler* mMacAddressHandler;
    ArduinoBTController* mArduinoBTController;

    //End of hacking
    CamState mState;
    bool mStopViewfinder;
    bool mDeferredResourceWarning;
    StatusThread* mStatusThread;
    bb::cascades::Application* mApp;
    // here is a bunch of rotation state...
    // note that we are tracking display direction via naviagtor signals in order to know which way the UI is being rendered.
    // this will help us to rotate the viewfinder correctly for display.
    // but we are also tracking the device orientation directly via QOrientationSensor.  this is because navigator may not report
    // all orientations (eg. Q10 is not rotatable, and Z10 cannot be used upside-down), but we still want to know which way is up in
    // order to have our photos and videos displayed upright later.
    uint32_t mDisplayDirection;  // as reported by navigator (opposite from the angle we use internally)
    uint32_t mOrientationDirection; // as reported by QOrientationSensor reading (opposite from the angle we use internally)
    uint32_t mDeviceOrientation; // the clockwise-angle equivalent of mOrientationDirection
    uint32_t mDesiredVfAngle;    // the angle we would prefer all viewfinder buffers to be rendered on screen so that pixel 0 is top-left
    uint32_t mDesiredCapAngle;   // the angle we would prefer all captured buffers to be rotated so that pixel 0 is top-left
    uint32_t mVfAngle;           // the angle programmed for viewfinder buffer rotation (desired angle may not be supported)
    uint32_t mCaptureAngle;      // the angle programmed for photo/video capture buffer rotation (desired angle may not be supported)
    uint32_t mWindowAngle;       // the angle to rotate the viewfinder screen window so that the contents appear upright on the display
    bool mRequireUprightVf; // whether vf buffer rotation should be applied, or whether screen rotation is good enough
    bool mRequireUprightCapture; // whether capture buffer rotation should be applied, or whether metadata is good enough
    QtMobility::QOrientationSensor* mOrientationSensor;
    QRectF mVfContainerSize;
    uint32_t mVfWidth;
    uint32_t mVfHeight;
    uint32_t mCapWidth;
    uint32_t mCapHeight;
    int mVideoFileDescriptor;
    // frame formats used by viewfinder & capture
    camera_frametype_t mPhotoFormat;
    camera_frametype_t mPhotoVfFormat;
    camera_frametype_t mVideoFormat;
    camera_frametype_t mVideoVfFormat;
    // rotation capabilities
    int mNumPhotoRotations;
    uint32_t* mPhotoRotations;
    int mNumPhotoVfRotations;
    uint32_t* mPhotoVfRotations;
    int mNumVideoRotations;
    uint32_t* mVideoRotations;
    int mNumVideoVfRotations;
    uint32_t* mVideoVfRotations;
    // resolution capabilities
    unsigned int mNumPhotoResolutions;
    camera_res_t* mPhotoResolutions;
    unsigned int mNumPhotoVfResolutions;
    camera_res_t* mPhotoVfResolutions;
    unsigned int mNumVideoResolutions;
    camera_res_t* mVideoResolutions;
    unsigned int mNumVideoVfResolutions;
    camera_res_t* mVideoVfResolutions;

    //Stephen hacking:


};

#endif /* __BESTCAM_HPP__ */
