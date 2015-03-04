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


/*A note about this project from Stephen
 *
 *
 * I created this project as part of a blackberry hackathon in Feb 2014.
 * This code is by no means intended to be for production it does however work.
 * Proof it works: https://www.youtube.com/watch?v=T9ht3vPhCQo
 *
 * A bit of background:
 *  I work for Blackberry/TAT Sweden and we had a company hackathon.
 *  I wanted to build something interesting that combined external hardware + a BlackBerry as part of the hackathon.
 *  Its important to note that this code was written over a weekend and is VERY hacky.  The base of this code is two different examples
 *  the first is the bestcam app and the second is the Råck2000 demo.  The project was a fun weekend project and if I have more time
 *  I may decide to continue it but right now I'm happy with how it performs and don't plan to do much more.
 *  As is said here in Sweden, "Lagom är bäst".
 *
 *  Enjoy!
 *  Stephen
 *
 *
 *  The source for the two apps can be found here:
 *  Råck demo: https://github.com/blackberry/Presentations/tree/master/2013-BlackBerryJam-Asia/General%20Session/rack2000
 *  BestCamera: https://github.com/blackberry/Cascades-Community-Samples/tree/master/BestCamera
 *
 * To use this code you will need to do the following:
 * 1. Get an air hockey table and mount a BlackBerry z30 above it
 * 2. Get an arduino and bluetooth board + relay board.
 * 3. Setup said boards like so: https://github.com/blackberry/Presentations/tree/master/2013-BlackBerryJam-Asia/General%20Session/rack2000
 * 4. Hookup the first 2 relays in an "H bridge"  and then give them 12 V
 * 5. Hook up a windshield wiper motor to these two relays outputs (so it will go forward/backwards)
 * 6. Get a piece of aluminum tube for an arm and make it look something like the youtube video above.
 * 7. VERY IMPORTANT: You MUST add limit switches with diodes so that they physically disconnect power from the motor
 * 8. Make sure you have set up number 7 correctly so you don't have a spinning chunk of aluminum.
 * 9. Cover an airhockey puck in green stickers
 * 10. Put it all together and play air hockey against your BlackBerry.
 *
 *
 *
 *
 * */
#include "BestCam.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/Window>
#include <bb/cascades/ForeignWindowControl>
#include <bb/cascades/OrientationSupport>
#include <bb/cascades/LayoutUpdateHandler>
#include <bps/soundplayer.h>
#include <fcntl.h>
#include <QtSensors/QOrientationSensor>

using namespace bb::cascades;
using namespace QtMobility;

// when trying to open the camera, retry for this amount of time in order to handle
// corner cases where another app may still have the camera open
#define RETRY_MS    1000
#define RETRY_DELAY (RETRY_MS/10)

//Stephen Hacking starts here:

#define DEBUG_SPAM 0

MotorPositions gMove = armStop;
//MotorState:
//MotorState -1 = left, 0 = stop, 1 = right
MotorPositions gMotorState = armStop;  //Represents current motor action.

#define MOTOR_ENABLED 1
#define DELTA_MODIFIER 10       //This is how far we wish to predict

//Stephens simple filter/prediction mech:
class puckPrediction {
    Vector2 oldPosition;
    Vector2 nextPosition;
public:
    puckPrediction(){
    }
    Vector2 getNextPosition() { return nextPosition;}
    void addPosition(Vector2 newPosition) {
        if(newPosition.x <= 0.1f && newPosition.y <= 0.1f) { //Lost puck:
            oldPosition.x = 0.f;
            oldPosition.y = 0.f;
            nextPosition.x = 0.f;
            nextPosition.y = 0.f;
        }
        else {
            //See how far we moved since last frame and then predict that we will continue moving in that direction.
            //The modifier is used to predict further than 1 frame distance into the future to accommodate for
            //the low framerate and fast moving puck
            nextPosition.x = newPosition.x + (newPosition.x - oldPosition.x) * DELTA_MODIFIER;
            nextPosition.y = newPosition.y + (newPosition.y - oldPosition.y) * DELTA_MODIFIER;
            oldPosition = newPosition;
        }
    }
};


puckPrediction nextPuckPosition;

#define PUCK_CENTER_THREASHOLD 5

//getCurrentPuckPosition  provides positions in 0-2 position format. (left/center/right)
int getCurrentPuckPosition (int width) {
    if( nextPuckPosition.getNextPosition().x <= (width/2 - PUCK_CENTER_THREASHOLD)) {
        return 2;
    } else if(nextPuckPosition.getNextPosition().x >= (width/2 + PUCK_CENTER_THREASHOLD)) {
        return 0;
    } else {
        return 1;
    }
}

struct FrameData
{
    uint8_t *pBuf;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
    uint64_t size;
    int64_t uv_offset;
    int64_t uv_stride;
    camera_frametype_t in_format;
    int out_format;
};

#define PUCK_U_UPPER_MATCH_RAW 0x73
#define PUCK_U_LOWER_MATCH_RAW 0x00
#define PUCK_V_UPPER_MATCH_RAW 0x73
#define PUCK_V_LOWER_MATCH_RAW 0x00

//This is a simple function to see if the U/V values are within the puck colour.
//Because the puck is the only green thing on the board I ignore the y values
bool isPuckColorRaw(unsigned char u, unsigned char v) {
  //Check U then V values:
  if((u <= PUCK_U_UPPER_MATCH_RAW) && (u >= PUCK_U_LOWER_MATCH_RAW)) {
      if((v <= PUCK_V_UPPER_MATCH_RAW) && (v >= PUCK_V_LOWER_MATCH_RAW)) {
          return true;
      }
  }
  return false;
}

//Lazy way of finding the puck.  Find and add up the location of all "puck colour" pixels
//Then average to get the rough location of center of the puck.  Will only work for 1 puck and
//will only work if there is nothing else puck coloured in the field of vision.
//Note to self: don't let people with green shirts play!
void findPuckAndArm(uint8_t * pRawInputData, int64_t uvOffset, int64_t uvStride,
        unsigned int width, unsigned int height, Vector2 &puckPosition) {

  unsigned int numPuckPixels = 0;
  float xPuckSum = 0;
  float yPuckSum = 0;

  width = width/2;
  height = height/2;

  pRawInputData += uvOffset; //Skip over y Data

  for(unsigned int row = 0; row < height; row++){       //height /2
      for(int col = 0; col < (int)width; col++){        //width/2
          unsigned char v = pRawInputData[row * uvStride + col * 2 + 0];
          unsigned char u = pRawInputData[row * uvStride + col * 2 + 1];

          if(isPuckColorRaw(u, v)) {
              //Found a puck pixel add it to the average
                yPuckSum += row * 2;
                xPuckSum += col * 2;
              numPuckPixels ++;
          }
      }
  }

  if(numPuckPixels) {
      puckPosition.x = xPuckSum / numPuckPixels;
      puckPosition.y = yPuckSum / numPuckPixels;
  }
  else {
      puckPosition.x = 0.f;
      puckPosition.y = 0.f;
  }
}

//Things for bluetooth arduino:  (From Råck 2000 demo)
void BestCam::createArduinoController()
{
    if (!mArduinoBTController) {
        mArduinoBTController = new ArduinoBTController(this);
    }
}

void BestCam::createMacAddressHandler()
{
    if (!mMacAddressHandler) {
        mMacAddressHandler = new MacAddressHandler(this);
    }
}
//End of things for BT/arduino

//Motor Control Functions:
//These simply turn the motor on/off in different directions
void BestCam::motorLeft() {
    if(gMotorState != armLeft) {
#if MOTOR_ENABLED
        //Send all off:
        mArduinoBTController->sendMyMessage("0");
        //Turn off relay 1, turn on relay 2
        mArduinoBTController->sendMyMessage("10");
        mArduinoBTController->sendMyMessage("21");
        gMotorState = armLeft;
#endif
    }
}
void BestCam::motorRight() {
    if(gMotorState != armRight) {
#if MOTOR_ENABLED
        //Send all off:
        mArduinoBTController->sendMyMessage("0");
        //Turn on relay 1, turn off relay 2
        mArduinoBTController->sendMyMessage("11");
        mArduinoBTController->sendMyMessage("20");
        gMotorState = armRight;
#endif
    }


}
void BestCam::motorStop() {
    if(gMotorState != armStop) {
#if MOTOR_ENABLED
        //Send all off
        mArduinoBTController->sendMyMessage("0");
        gMotorState = armStop;
#endif
    }

}
//End of motor functions

//Arm functions:
//These actually move the arm to various "positions" using the motor functions (Currently only left/right)
//Time to move arm to different positions in ms
#define TIME_LEFT_ARM 300
#define TIME_RIGHT_ARM 300

int gCurrentGoaliePos;
#define ARM_MOVE_INTERVAL 10 //prevent random sketchy bouncing

//Move to one of 2 positions:
void BestCam::moveArmToPositionLeft(){
    if(gCurrentGoaliePos != -1) {
        motorLeft();
        usleep(TIME_LEFT_ARM * 1000);
        motorStop();
        gCurrentGoaliePos = -1;
    }
    usleep(ARM_MOVE_INTERVAL * 1000);

}

void BestCam::moveArmToPositionRight() {
    if(gCurrentGoaliePos != 1) {
        motorRight();
        usleep(TIME_RIGHT_ARM * 1000);
        motorStop();
        gCurrentGoaliePos = 1;
    }
    usleep(ARM_MOVE_INTERVAL * 1000);
}
//End of arm functions

//Callback for each video frame.  This will find the puck and add it to the positionPredictor
//It will then set the motor to move if it has found the puck or to stop if it has lost the puck.
void BestCam::videoFrameCallback(camera_handle_t cameraHandle, camera_buffer_t * pCameraBuffer){
    if(pCameraBuffer) {
        Vector2 currentPuckPosition;
#if DEBUG_SPAM
            fprintf(stderr, "Camera Frame = %d x %d\n", pCameraBuffer->framedesc.nv12.height, pCameraBuffer->framedesc.nv12.width);
            fprintf(stderr, "stride = %d\n", pCameraBuffer->framedesc.nv12.uv_stride);
#endif

        findPuckAndArm(pCameraBuffer->framebuf, pCameraBuffer->framedesc.nv12.uv_offset,pCameraBuffer->framedesc.nv12.uv_stride,
                pCameraBuffer->framedesc.nv12.width, pCameraBuffer->framedesc.nv12.height, currentPuckPosition);
#if DEBUG_SPAM
        fprintf(stderr, "Current Puck before predictor: %f, %f\n", currentPuckPosition.x, currentPuckPosition.y);
#endif
        nextPuckPosition.addPosition(currentPuckPosition);
        if(nextPuckPosition.getNextPosition().x <= 0.1f && nextPuckPosition.getNextPosition().y <= 0.1f) {
            //Arm or puck can't be found don't move:
            fprintf(stderr, "Lost puck.  Not moving!\n");
            gMove = armStop;
        }
        else {
#if DEBUG_SPAM
            fprintf(stderr, "width: %d", (int)(pCameraBuffer->framedesc.nv12.width));
#endif
            int puckPosition = getCurrentPuckPosition((int)(pCameraBuffer->framedesc.nv12.width));
#if DEBUG_SPAM
            fprintf(stderr, "puckPosition = %d\n", puckPosition);
#endif
            if(puckPosition == 2) {
                gMove = armRight;
            }else if (puckPosition == 0) {
                gMove = armLeft;
            } else {
                gMove = armStop;
            }
        }
    }
}

//Stephen Hacking ends here.


// qDebug() now logs to slogger2, which I find inconvenient since the NDK does not pick this up in the console,
// so I am installing a custom handler to log to stderr.
static void log_to_stderr(QtMsgType msgType, const char *msg)
{
    (void)msgType;  // go away, warning!
    fprintf(stderr, "%s\n", msg);
}


BestCam::BestCam(bb::cascades::Application *app) :
        QObject(app),
        mUnit(CAMERA_UNIT_NONE),
        mHandle(CAMERA_HANDLE_INVALID),
        mState(StateIdle),
        mDeferredResourceWarning(false),
        mStatusThread(NULL),
        mApp(app),
        mRequireUprightVf(false),
        mRequireUprightCapture(false),
        mOrientationSensor(NULL),
        mVideoFileDescriptor(-1),
        mPhotoRotations(NULL),
        mPhotoVfRotations(NULL),
        mVideoRotations(NULL),
        mVideoVfRotations(NULL),
        mPhotoResolutions(NULL),
        mPhotoVfResolutions(NULL),
        mVideoResolutions(NULL),
        mVideoVfResolutions(NULL),
        mMacAddressHandler(NULL),
        mArduinoBTController(NULL)
{
    // NOTE: since we are passed the Application instance when constructed, we can just cache it for later.
    // if this code is eventually migrated into a custom control, then we would instead use Application::instance()

    // TODO: this is just for my own debugging purposes. should not ship with this.
    //qInstallMsgHandler(log_to_stderr);

    qmlRegisterUncreatableType<BestCam>("my.library", 1, 0, "BestCam", "BestCam is uncreatable");
    qRegisterMetaType<camera_devstatus_t>("camera_devstatus_t");
    qRegisterMetaType<uint16_t>("uint16_t");
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);
    if(!QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()))) {
        // This is an abnormal situation! Something went wrong!
        // Add own code to recover here
        qWarning() << "Recovering from a failed connect()";
    }

    if (!QObject::connect(this, SIGNAL(captureComplete(int)), this, SLOT(onCaptureComplete(int)))) {
        qWarning() << "failed to connect captureComplete signal";
    }

    if (!QObject::connect(mApp, SIGNAL(thumbnail()), this, SLOT(onThumbnail()))) {
        qWarning() << "failed to connect thumbnail signal";
    }

    if (!QObject::connect(mApp, SIGNAL(fullscreen()), this, SLOT(onFullscreen()))) {
        qWarning() << "failed to connect fullscreen signal";
    }

    if (!QObject::connect(mApp, SIGNAL(invisible()), this, SLOT(onInvisible()))) {
        qWarning() << "failed to connect invisible signal";
    }

    if (!QObject::connect(OrientationSupport::instance(),
                          // previously, I was using orientationChanged(), but things may look better if we
                          // update the window before the rotation effect happens
                          SIGNAL(displayDirectionAboutToChange(bb::cascades::DisplayDirection::Type, \
                                                               bb::cascades::UIOrientation::Type)),
                          this,
                          SLOT(onDisplayDirectionChanging(bb::cascades::DisplayDirection::Type, \
                                                          bb::cascades::UIOrientation::Type)))) {
        qWarning() << "failed to connect displayDirectionAboutToChange signal";
    }

    // connect to the QOrientationSensor and get ready to receive readings
    if (startOrientationReadings() != EOK) {
        qWarning() << "failed to connect to orientation sensor?";
    }
    // indicate all orientations are supported
    OrientationSupport::instance()->setSupportedDisplayOrientation(SupportedDisplayOrientation::All);
    // initialize angles.  for the 2nd reading, we should probably be querying our QOrientationSensor, but I'm not sure if it will be
    // available yet, since we just started the sensor.  it's okay to pick another default for now.. we can update when the reading becomes
    // available.
    mDisplayDirection = OrientationSupport::instance()->displayDirection();
    mOrientationDirection = mDisplayDirection;
    updateAngles();

    // initial load
    onSystemLanguageChanged();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    qml->setContextProperty("Cam", this);
    // Create root object for the UI
    mRoot = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    app->setScene(mRoot);

    // find the ForeignWindowControl and cache it for later
    mFwc = mRoot->findChild<ForeignWindowControl*>("vfForeignWindow");

    //Stephen Hacking in some startup stuff:

    //Setup bluetooth/ardruino:
    createMacAddressHandler();
    createArduinoController();

    //Turn off the motor if its moving then initialize it to the left all the way:
    motorStop();
    usleep(50 * 1000);
    motorLeft();
    usleep(500 * 1000);
    motorStop();
    gCurrentGoaliePos = -1;

    //End of hacking in the startup stuff

    // hook into the layout engine to find the size of the parent container of the viewfinder FWC
    LayoutUpdateHandler::create((bb::cascades::Control*)(mFwc->parent()))
        .onLayoutFrameChanged(this, SLOT(onVfParentLayoutFrameChanged(QRectF)));

    // start up in photo mode
    inventoryCameras();
    emit canCaptureChanged(mCanCapture = false);
    emit capturingChanged(mCapturing = false);
    if (mHasRearCamera) {
        mUnit = CAMERA_UNIT_REAR;
    } else if (mHasFrontCamera) {
        mUnit = CAMERA_UNIT_FRONT;
    } else {
        mUnit = CAMERA_UNIT_NONE;
    }

    //Hack to change to start video by default
    mResumeVfMode = ModeVideo;
    setCameraUnit((CameraUnit)mUnit);
    setVfMode(ModeVideo);
}


void BestCam::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("BestCamera_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}


int BestCam::capture()
{
    int err = EOK;
    switch (mState) {
    case StateVideoVf:
        mStopViewfinder = false;
        err = runStateMachine(StateVideoCapture);
        break;
    case StateVideoCapture:
        mStopViewfinder = false;
        err = runStateMachine(StateVideoVf);
        break;
    case StatePhotoVf:
        mStopViewfinder = false;
        err = runStateMachine(StatePhotoCapture);
        break;
    default:
        qDebug() << "error, cannot capture in state" << stateName(mState);
        err = EINVAL;
        break;
    }
    return err;
}


int BestCam::openCamera(camera_unit_t unit)
{
    if (mHandle != CAMERA_HANDLE_INVALID) {
        qDebug() << "already have a camera open";
        return CAMERA_EALREADY;
    }
    // Implement a retry loop here to handle cases where another app may not have released the camera yet.
    // (eg. when switching between camera apps)
    int err;
    for (int retry=RETRY_MS; retry; retry-=RETRY_DELAY) {
        err = camera_open((camera_unit_t)unit, CAMERA_MODE_RW | CAMERA_MODE_ROLL, &mHandle);
        if (err == EOK) break;
        qDebug() << "failed to open camera unit" << unit << ": error" << err << "(will retry)";
        usleep(RETRY_DELAY * 1000);
    }
    if (err) {
        qDebug() << "could not open camera unit" << unit << ": error" << err;
        mHandle = CAMERA_HANDLE_INVALID;
        mUnit = CAMERA_UNIT_NONE;
    } else {
        qDebug() << "opened camera unit" << unit;
        mUnit = (camera_unit_t)unit;
        err = discoverCameraCapabilities();
        if (err) {
            qDebug() << "failed to query camera capabilities.";
            // just abort here
            closeCamera();
            return err;
        }
        // attach status event monitoring thread.
        // ordinarilly, we would just use a status callback via one of the viewfinder start functions,
        // but that would mean we would only be able to monitor status while a viewfinder is running.
        // registering explicitly for a status event this way allows us to monitor for camera powerdown/powerup events
        // as long as we still have the camera device open.
        mStatusThread = new StatusThread(this);
        if (!mStatusThread) {
            err = errno;
            qDebug() << "failed to attach status thread";
        } else {
            if (!QObject::connect(mStatusThread, SIGNAL(statusChanged(camera_devstatus_t, uint16_t)),
                                  this, SLOT(onStatusChanged(camera_devstatus_t, uint16_t)))) {
                qDebug() << "failed to connect statusChanged";
            } else {
                mStatusThread->start();
            }
        }
    }
    return err;
}


int BestCam::closeCamera()
{
    if (mHandle == CAMERA_HANDLE_INVALID) {
        qDebug() << "no camera to close. ignoring.";
    } else {
        // detach the status thread
        if (mStatusThread) {
            mStatusThread->cleanShutdown();
            mStatusThread->wait();
            delete mStatusThread;
            mStatusThread = NULL;
        }
        qDebug() << "closing camera";
        camera_close(mHandle);
        mHandle = CAMERA_HANDLE_INVALID;
    }
    return EOK;
}


int BestCam::setCameraUnit(CameraUnit unit)
{
    int err = EOK;
    // only allow this if we are not in a transitory state
    if ((mState == StateStartingPhotoVf) || (mState == StateStartingVideoVf)) {
        return EINVAL;
    }
    err = runStateMachine(StateIdle);
    closeCamera();
    if (err == EOK) {
        err = openCamera((camera_unit_t)unit);
        if (err) {
            qDebug() << "failed to open camera" << unit;
        } else {
            err = setVfMode(mVfMode);
        }
        emit cameraUnitChanged((CameraUnit)mUnit);
    }
    return err;
}


int BestCam::setVfMode(VfMode mode)
{
    int err = EOK;
    switch (mode) {
    case ModePhoto:
        mStopViewfinder = true;
        if (err == EOK) {
            err = runStateMachine(StateStartingPhotoVf);
        } else {
            err = CAMERA_EALREADY;
        }
        break;
    case ModeVideo:
        mStopViewfinder = true;
        if (err == EOK) {
            err = runStateMachine(StateStartingVideoVf);
        } else {
            err = CAMERA_EALREADY;
        }
        break;
    default:
        err = runStateMachine(StateIdle);
        break;
    }
    if (err == EOK) {
        mVfMode = mode;
        if (mVfMode != ModeNone) {
            // remember last-set mode so that we can resume into that mode after minimize/powerdown
            mResumeVfMode = mVfMode;
        }
        emit vfModeChanged(mVfMode);
    }
    return err;
}


void BestCam::inventoryCameras()
{
    unsigned int num;
    mHasFrontCamera = mHasRearCamera = false;
    if (camera_get_supported_cameras(0, &num, NULL) != EOK) {
        qDebug() << "failed to query number of supported cameras";
    } else {
        camera_unit_t units[num];
        if (camera_get_supported_cameras(num, &num, units) != EOK) {
            qDebug() << "failed to query supported cameras";
        } else {
            for (unsigned int i=0; i<num; i++) {
                if (units[i] == CAMERA_UNIT_FRONT) {
                    mHasFrontCamera = true;
                } else if (units[i] == CAMERA_UNIT_REAR) {
                    mHasRearCamera = true;
                }
            }
        }
    }
    emit hasFrontCameraChanged(mHasFrontCamera);
    emit hasRearCameraChanged(mHasRearCamera);
}


//Todo: Cleanup This is not used by the air hockey app
int BestCam::startPhotoVf()
{
    int err = EOK;

    mRequireUprightVf = false;      // since we are not processing pixels, we don't care which way the vf buffer is oriented
    mRequireUprightCapture = true;  // try our best to orient capture buffers upright, but rely on EXIF if not possible

    // when configuring a viewfinder, the capture and viewfinder resolutions must have the same aspect ratio.
    // let's check what the current (default) photo capture size is...
    err = camera_get_photo_property(mHandle,
                                    CAMERA_IMGPROP_WIDTH, &mCapWidth,
                                    CAMERA_IMGPROP_HEIGHT, &mCapHeight);
    if (err) {
        qDebug() << "error querying photo resolution";
    } else {
        qDebug() << "photo capture resolution is" << mCapWidth << "x" << mCapHeight;
        camera_res_t capres;
        capres.width = mCapWidth;
        capres.height = mCapHeight;
        camera_res_t* vfres = matchAspectRatio(&capres, mPhotoVfResolutions, mNumPhotoVfResolutions, 0.01);
        if (!vfres) {
            qDebug() << "could not find a matching aspect ratio for the viewfinder";
            err = EINVAL;
        } else {
            mVfWidth = vfres->width;
            mVfHeight = vfres->height;
            qDebug() << "matching viewfinder resolution is" << mVfWidth << "x" << mVfHeight;
            err = camera_set_photovf_property(mHandle,
                                              CAMERA_IMGPROP_WIDTH, mVfWidth,
                                              CAMERA_IMGPROP_HEIGHT, mVfHeight);
            if (err) {
                qDebug() << "failed to set photovf resolution";
            } else {
                // find the window group & window id required by the ForeignWindowControl
                QByteArray groupBA = mFwc->windowGroup().toLocal8Bit();
                QByteArray winBA = mFwc->windowId().toLocal8Bit();
                err = camera_set_photovf_property(mHandle,
                                                  CAMERA_IMGPROP_WIN_GROUPID, groupBA.data(),
                                                  CAMERA_IMGPROP_WIN_ID, winBA.data());
                if (err) {
                    qDebug() << "error setting photovf properties:" << err;
                } else {
                    err = camera_start_photo_viewfinder(mHandle,
                                                        NULL,
                                                        NULL,
                                                        NULL);
                    if (err) {
                        qDebug() << "error starting photo viewfinder:" << err;
                    } else {
                        // register for resource warning notifications.
                        // we may not really require this, however since we will be handling buffers when saving photos,
                        // it is a good idea to register -- this allows us a small grace period to finish saving to disk before the
                        // OS forcefully unmaps buffers from our address space.  (we would probably just core dump if the camera
                        // were to power down while we were saving a photo).  this mechanism should also be used by any app that
                        // is processing viewfinder buffers for image processing, recognition, tracking, etc.
                        // note: this is implemented as a counter by the OS, so any time we increment using _register_, we need
                        // to make sure to decrement using _deregister_ when we are finished with resources. when the count of
                        // registrations reaches 0, the OS will allow the camera to be powered off (or after a couple of seconds of grace).
                        err = camera_register_resource(mHandle);
                    }
                }
            }
        }
    }
    return err;
}

//Todo: Cleanup This is not used by the air hockey app
int BestCam::stopPhotoVf()
{
    qDebug() << "stopping photo viewfinder";
    // when stopping the photo viewfinder, we'll deregister our interest in buffer resource.
    // note that if we were shutting down due to a CAMERA_STATUS_RESOURCENOTAVAIL notification,
    // power-down will proceed immediately and the viewfinder will be stopped by the OS for us.
    // the camera_stop_photo_viewfinder() call which follows may return an error in that case, but
    // that's ok -- it's already stopped.
    int err = camera_deregister_resource(mHandle);
    if (err) {
        qDebug() << "error trying to deregister resource:" << err;
    }
    err = camera_stop_photo_viewfinder(mHandle);
    if (err) {
        qDebug() << "error trying to shut down photo viewfinder:" << err;
    }
    return err;
}


int BestCam::startVideoVf()
{
    int err = EOK;

    mRequireUprightVf = false;      // since we are not processing pixels, we don't care which way the vf buffer is oriented.
                                    // however, when it comes time to record video, we will need to change this if the video source is the vf.
    mRequireUprightCapture = true;  // capture buffers must be upright since we don't have a metadata solution at this time

    // find the window group & window id required by the ForeignWindowControl
    QByteArray groupBA = mFwc->windowGroup().toLocal8Bit();
    QByteArray winBA = mFwc->windowId().toLocal8Bit();

    double frameRates[100];
    int numSupported = 0;
    unsigned int numRes = 0;
    camera_res_t resolutionsSupported[100];

    camera_get_video_output_resolutions(mHandle, 100, &numRes, resolutionsSupported);
    double maxFrameRate = 0;
    camera_res_t maxFrameRateResolution;

    for(int i = 0; i < (int)numRes; i++) {
        camera_set_videovf_property(mHandle,
                                    CAMERA_IMGPROP_HEIGHT, resolutionsSupported[i].height,
                                    CAMERA_IMGPROP_WIDTH, resolutionsSupported[i].width);
        camera_set_video_property(mHandle,
                                  CAMERA_IMGPROP_HEIGHT, resolutionsSupported[i].height,
                                  CAMERA_IMGPROP_WIDTH, resolutionsSupported[i].width);
        //Check to see what framerates we get if we set this:
        camera_get_video_vf_framerates(mHandle, 100, &numSupported, frameRates, false);
        for(int j = 0; j < numSupported; j++) {
        	if(frameRates[j] > maxFrameRate) {
        		maxFrameRate = frameRates[j];
        		maxFrameRateResolution = resolutionsSupported[i];
        		fprintf(stderr, "Found max framerate: %g at resolution %d x %d\n", maxFrameRate, maxFrameRateResolution.height, maxFrameRateResolution.width);
        	}
        }
    }

    err = camera_set_videovf_property(  mHandle,
                                        CAMERA_IMGPROP_HEIGHT,          maxFrameRateResolution.height,
                                        CAMERA_IMGPROP_WIDTH,           maxFrameRateResolution.width,
                                        CAMERA_IMGPROP_FRAMERATE,       maxFrameRate);

    if (err) {
        qDebug() << "error setting videovf properties:" << err;
    }

    //Set matching video properties:
    err = camera_set_video_property(    mHandle,
                                        CAMERA_IMGPROP_HEIGHT,          maxFrameRateResolution.height,
                                        CAMERA_IMGPROP_WIDTH,           maxFrameRateResolution.width,
                                        CAMERA_IMGPROP_FRAMERATE,       maxFrameRate);
    if (err) {
        qDebug() << "error setting video properties:" << err;
    }

    err = camera_set_videovf_property(mHandle,
                                      CAMERA_IMGPROP_WIN_GROUPID, groupBA.data(),
                                      CAMERA_IMGPROP_WIN_ID, winBA.data());
    if (err) {
        qDebug() << "error setting videovf properties:" << err;
    } else {
        err = camera_start_video_viewfinder(mHandle,
                                            NULL,
                                            NULL,
                                            NULL);
        if (err) {
            qDebug() << "error starting video viewfinder:" << err;
        } else {
            // let's force the screen to stay awake as long as our app is visible
            mApp->mainWindow()->setScreenIdleMode(ScreenIdleMode::KeepAwake);
            // TODO: move this to the smart config section
            camera_get_videovf_property(mHandle,
                                        CAMERA_IMGPROP_WIDTH, &mVfWidth,
                                        CAMERA_IMGPROP_HEIGHT, &mVfHeight);
        }
    }

    return err;
}


int BestCam::stopVideoVf()
{
    qDebug() << "stopping video viewfinder";
    int err = camera_stop_video_viewfinder(mHandle);
    if (err) {
        qDebug() << "error trying to shut down video viewfinder:" << err;
    }
    mApp->mainWindow()->setScreenIdleMode(ScreenIdleMode::Normal);
    return err;
}


int BestCam::startRecording()
{
    int err = EOK;

    // THE CAMERA SERVICE DOES NOT PLAY SOUNDS WHEN PICTURES ARE TAKEN OR
    // VIDEOS ARE RECORDED.  IT IS THE APP DEVELOPER'S RESPONSIBILITY TO
    // PLAY AN AUDIBLE SHUTTER SOUND WHEN A PICTURE IS TAKEN AND WHEN VIDEO
    // RECORDING STARTS AND STOPS.  NOTE THAT WHILE YOU MAY CHOOSE TO MUTE
    // SUCH SOUNDS, YOU MUST ENSURE THAT YOUR APP ADHERES TO ALL LOCAL LAWS
    // OF REGIONS WHERE IT IS DISTRIBUTED.  FOR EXAMPLE, IT IS ILLEGAL TO
    // MUTE OR MODIFY THE SHUTTER SOUND OF A CAMERA APPLICATION IN JAPAN OR
    // KOREA.
    // TBD:
    //   BlackBerry will be providing clarification of this policy as part of the
    //   NDK developer agreement and App World guidelines.  A link will
    //   be provided when the policy is publicly available.

    // NOTE: we use the _blocking variant here so that the sound doesn't bleed over
    // into our recording.  An alternate solution may involve muting the mic temporarily,
    // in order to allow video recording to start slightly sooner.
    soundplayer_play_sound_blocking("event_recording_start");

    if (camera_has_feature(mHandle, CAMERA_FEATURE_PREVIEWISVIDEO)) {
        // if the video output uses the same buffers as the viewfinder, then they must both be oriented the same if required
        mRequireUprightVf = mRequireUprightCapture;
        // need to update vf angle to match recording angle
        mDesiredVfAngle = mDesiredCapAngle;
    }
    // apply the changes
    updateVideoAngle();
    // flush our screen context to make changes take effect
    screen_window_t win = mFwc->windowHandle();
    if (!win) {
        qDebug() << "can't get window handle to flush context";
    } else {
        screen_context_t screen_ctx;
        screen_get_window_property_pv(win, SCREEN_PROPERTY_CONTEXT, (void **)&screen_ctx);
        screen_flush_context(screen_ctx, 0);

        // open file for output and start the recorder
        char filename[CAMERA_ROLL_NAMELEN];
        err = camera_roll_open_video(mHandle,
                                     &mVideoFileDescriptor,
                                     filename,
                                     sizeof(filename),
                                     CAMERA_ROLL_VIDEO_FMT_DEFAULT);
        if (err == EOK) {
            qDebug() << "opened " << filename;
            //Stephen added the video frame callback
            err = camera_start_video(mHandle, filename, videoFrameCallbackEntry, NULL, NULL);
            if (err == EOK) {
                qDebug() << "started recording";

                //Massive hack: This stops the display from updating properly... but hey its quick and this was a hackathon project:
                while(1){
                    switch(gMove) {
                        case armLeft:
                            moveArmToPositionLeft();
                            break;
                        case armRight:
                            moveArmToPositionRight();
                            break;
                        case armStop:
                            //I used to move to center but it was not really needed since I can cover the whole goal mouth with 2 positions
//                            moveArmToPositionCenter();    //Never move to center :)
                            break;
                    }
                    usleep(ARM_MOVE_INTERVAL * 1000); //prevent too much twitching
                }
                //end of Stephens massive hack
                return EOK;
            }
            qDebug() << "failed to start recording";
            // the file will be closed and the stop/abort tone will be played by stopRecording() when we exit the state in failure
        }
    }
    return err;
}


int BestCam::stopRecording()
{
    int err = EOK;

    // revert to not requiring the viewfinder be upright, since we are not using the pixels from the buffer anymore
    mRequireUprightVf = false;

    // if a recording is in progress...
    if (mVideoFileDescriptor != -1) {
        // stop recording.
        err = camera_stop_video(mHandle);
        if (err != EOK) {
            qDebug() << "failed to stop video recording. err " << err;
        }
        camera_roll_close_video(mVideoFileDescriptor);
        mVideoFileDescriptor = -1;
    }

    // play the stop/abort tone
    soundplayer_play_sound("event_recording_stop");

    return err;
}

//Todo: Cleanup This is not used by the air hockey app
void BestCam::takePhoto()
{
    int err = EOK;
    err = camera_take_photo(mHandle,
                            shutterCallbackEntry,
                            NULL,
                            NULL,
                            stillCallbackEntry,
                            (void*)this,
                            true);
    emit captureComplete(err);
}


int BestCam::runStateMachine(CamState newState)
{
    int err = EOK;
    CamState nextState;
    // try to enter the requested state
    while (mState != newState) {
        qDebug() << "exiting state" << stateName(mState);
        err = exitState();
        if (err != EOK) {
            return err;
        }

        mState = newState;

        qDebug() << "entering state" << stateName(newState);
        err = enterState(newState, nextState);
        if (err != EOK) {
            qDebug() << "error" << err << "entering state" << stateName(newState);
            // in error case, we will actually just move to idle
        }
        // check if a new transition was requested by attempting to enter state
        if (nextState != newState) {
            newState = nextState;
        }
    }
    return err;
}


int BestCam::exitState()
{
    int err = EOK;
    switch(mState) {
    case StateIdle:
        // update UI?
        break;

    case StatePhotoCapture:
        emit capturingChanged(mCapturing = false);
        /* no break */
    case StateStartingPhotoVf:
    case StatePhotoVf:
        // update UI
        emit canCaptureChanged(mCanCapture = false);
        if (mStopViewfinder) {
            err = stopPhotoVf();
        }
        break;
    case StateVideoCapture:
        // unlock orientation when video recording ends
        OrientationSupport::instance()->setSupportedDisplayOrientation(SupportedDisplayOrientation::All);
        err = stopRecording();
        if (mStopViewfinder) {
            err = stopVideoVf();
        }
        emit capturingChanged(mCapturing = false);
        break;
    case StateStartingVideoVf:
    case StateVideoVf:
        // update UI
        emit canCaptureChanged(mCanCapture = false);
        if (mStopViewfinder) {
            err = stopVideoVf();
        }
        break;

    default:
        // nothing to do when exiting other states
        break;
    }

    return err;
}


int BestCam::enterState(CamState state, CamState &nextState)
{
    // unless otherwise told, we will be remaining in the requested state
    nextState = state;
    int err = EOK;

    switch(state) {
    case StateIdle:
        // update UI
        mCanCapture = false;
        canCaptureChanged(mCanCapture);
        break;
    case StateStartingPhotoVf:
        err = startPhotoVf();
        if (err) {
            nextState = StateIdle;
        } else {
            mStopViewfinder = true;
        }
        break;
    case StatePhotoVf:
        mStopViewfinder = true;
        // update UI
        emit canCaptureChanged(mCanCapture = true);

        break;
    case StatePhotoCapture:
        emit capturingChanged(mCapturing = true);
        QtConcurrent::run(this, &BestCam::takePhoto);
        break;
    case StateStartingVideoVf:
        err = startVideoVf();
        if (err) {
            nextState = StateIdle;
        } else {
            mStopViewfinder = true;
        }
        break;
    case StateVideoVf:
        mStopViewfinder = true;
        updateAngles();
        // update UI
        emit canCaptureChanged(mCanCapture = true);
        //I got tired of climbing a chair to start the camera.
        //So I just hacked this to put it into capture mode.
        // autostart the capture::
//        emit capturingChanged(mCapturing = true);
    	fprintf(stderr, "Starting capture:\n");
        capture();
        static bool first = true;

        if(first){
        	first = false;
        	sleep(1);
        	nextState = StateVideoCapture;
        }

        break;
    case StateVideoCapture:
        // lock orientation while video recording
        OrientationSupport::instance()->setSupportedDisplayOrientation(SupportedDisplayOrientation::CurrentLocked);
        emit capturingChanged(mCapturing = true);
        err = startRecording();
        if (err) {
            nextState = StateVideoVf;
        } else {
            emit canCaptureChanged(mCanCapture = true);
        }
        break;
    case StatePowerDown:
    case StateMinimized:
        // NOTE: we are combining powerdown and minimized states here for now, as we are treating them the same.
        // We are also going to be closing the camera in this state in order to play nice with other apps.
        closeCamera();
        emit vfModeChanged(mVfMode = ModeNone);
        break;
    default:
        // nothing to do?
        break;
    }

    // if we have just entered a state and resource warning is pending
    // (eg. we were in the middle of a photo capture when the warning was received),
    // then deal with it now
    if (mDeferredResourceWarning) {
        mDeferredResourceWarning = false;
        // TODO: which state is the best one in this case? Idle? PowerDown?
        nextState = StatePowerDown;
    }

    return err;
}


const char* BestCam::stateName(CamState state)
{
    switch(state) {
    case StateIdle:
        return "Idle";
    case StateStartingPhotoVf:
        return "StartingPhotoVf";
    case StatePhotoVf:
        return "PhotoVf";
    case StatePhotoCapture:
        return "PhotoCapture";
    case StateStartingVideoVf:
        return "StartingVideoVf";
    case StateVideoVf:
        return "VideoVf";
    case StateVideoCapture:
        return "VideoCapture";
    case StatePowerDown:
        return "PowerDown";
    case StateMinimized:
        return "Minimized";
    default:
        return "UNKNOWN";
    }
}


int BestCam::windowAttached()
{
    int err = EOK;

    // update window details
    screen_window_t win = mFwc->windowHandle();
    // put the viewfinder window behind the cascades window
    int i = -1;
    screen_set_window_property_iv(win, SCREEN_PROPERTY_ZORDER, &i);

    CamState newState = StateIdle;
    mStopViewfinder = true;
    switch (mState) {
    case StateStartingPhotoVf:
        // ensure we don't stop the viewfinder when transitioning out of StateStartingPhotoVf
        mStopViewfinder = false;
        updateAngles();
        updatePhotoAngle();
        newState = StatePhotoVf;
        break;
    case StateStartingVideoVf:
        // ensure we don't stop the viewfinder when transitioning out of StateStartingVideoVf
        mStopViewfinder = false;
        updateAngles();
        updateVideoAngle();
        newState = StateVideoVf;
        break;
    default:
        qDebug() << "unexpected window attach while not waiting for one";
        emit vfModeChanged(mVfMode = ModeNone);
        err = EINVAL;
        break;
    }

    screen_context_t screen_ctx;
    screen_get_window_property_pv(win, SCREEN_PROPERTY_CONTEXT, (void **)&screen_ctx);
    screen_flush_context(screen_ctx, 0);

    err = runStateMachine(newState);
    return err;
}


void BestCam::shutterCallback(camera_handle_t handle)
{
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

    (void)handle;  // silence compiler warning
}


//Fixme: Cleanup not used!
void BestCam::stillCallback(camera_handle_t handle, camera_buffer_t* buf)
{
    qDebug() << "still buffer received";
    if (buf->frametype == CAMERA_FRAMETYPE_JPEG) {
        qDebug() << "still image size:" << buf->framedesc.jpeg.bufsize;
        int fd;
        char filename[CAMERA_ROLL_NAMELEN];
        int err = camera_roll_open_photo(handle,
                                         &fd,
                                         filename,
                                         sizeof(filename),
                                         CAMERA_ROLL_PHOTO_FMT_JPG);
        if (err) {
            qDebug() << "error opening camera roll:" << err;
        } else {
            qDebug() << "saving:" << filename;
            int index = 0;
            while(index < (int)buf->framedesc.jpeg.bufsize) {
                int rc = write(fd, &buf->framebuf[index], buf->framedesc.jpeg.bufsize-index);
                if (rc > 0) {
                    index += rc;
                } else if (rc == -1) {
                    if ((errno == EAGAIN) || (errno == EINTR)) continue;
                    qDebug() << "write error:" << errno;
                    break;
                }
            }
            close(fd);
        }
    }
}


void BestCam::onCaptureComplete(int err)
{
    qDebug() << "capture complete. err =" << err;
    runStateMachine(StatePhotoVf);
}


StatusThread::StatusThread(BestCam* cam) :
    QThread(),
    mCam(cam),
    mStop(false)
{
    mHandle = mCam->mHandle;
    mChId = ChannelCreate(0);
    mCoId = ConnectAttach(0, 0, mChId, _NTO_SIDE_CHANNEL, 0);
    SIGEV_PULSE_INIT(&mEvent, mCoId, SIGEV_PULSE_PRIO_INHERIT, mPulseId, 0);
}


void StatusThread::run()
{
    int err = EOK;
    int rcvid;
    struct _pulse pulse;
    camera_eventkey_t key;
    camera_devstatus_t status;
    uint16_t extra;
    err = camera_enable_status_event(mHandle, &key, &mEvent);
    if (err) {
        qDebug() << "could not enable status event. err =" << err;
    } else {
        qDebug() << "status thread running";
        while(!mStop) {
            rcvid = MsgReceive(mChId, &pulse, sizeof(pulse), NULL);
            // not a pulse?
            if (rcvid != 0) continue;
            // not our pulse?
            if (pulse.code != mPulseId) continue;
            // instructed to stop?
            if (mStop) break;
            err = camera_get_status_details(mHandle, pulse.value, &status, &extra);
            if (err) {
                qDebug() << "failed to get status event details??";
            } else {
                emit statusChanged(status, extra);
            }
        }
        camera_disable_event(mHandle, key);
    }
    qDebug() << "status thread exiting";
}


void StatusThread::cleanShutdown()
{
    mStop = true;
    MsgSendPulse(mCoId, -1, mPulseId, 0);
}


void BestCam::onStatusChanged(camera_devstatus_t status, uint16_t extra)
{
    //qDebug() << "status event:" << status << "," << extra;
    switch(status) {
    case CAMERA_STATUS_RESOURCENOTAVAIL:
        qDebug() << "camera resources are about to become unavailable";
        resourceWarning();
        break;
    case CAMERA_STATUS_POWERDOWN:
        qDebug() << "camera powered down";
        poweringDown();
        break;
    case CAMERA_STATUS_POWERUP:
        qDebug() << "camera powered up";
        // since the onFullscreen handler will restart the camera for us, there is not much to do here.
        // however, if we were the sort of app that wanted to start running again even if we were backgrounded,
        // then we could consider resuming the viewfinder here.
        break;
    default:
        break;
    }
    // suppress warning
    (void)extra;
}

//Todo: Cleanup not really needed for airhockey
void BestCam::onFullscreen()
{
    qDebug() << "onFullscreen()";
    switch(mState) {
    case StateMinimized:
    case StatePowerDown:  // TODO: does this one need to be different?
        // coming back to the foreground, resume viewfinder
        runStateMachine(StateIdle);
        setCameraUnit((CameraUnit)mUnit);
        qDebug() << "setting vf mode" << mResumeVfMode;
        setVfMode(mResumeVfMode);
        break;
    default:
        // nothing to do
        break;
    }
}

//Todo: Cleanup not really needed for airhockey
void BestCam::onThumbnail()
{
    qDebug() << "onThumbnail()";
    switch (mState) {
    case StateVideoCapture:
        // if we are recording a video when we get minimized, let's keep recording
        qDebug() << "ignoring thumbnail signal... keep running!";
        break;
    default:
        runStateMachine(StateMinimized);
        break;
    }
}


void BestCam::onInvisible()
{
    qDebug() << "onInsivible()";
    switch (mState) {
    case StateVideoCapture:
        // if we are recording a video when we get covered, let's keep recording.
        // NOTE: since the app is no longer visible on-screen, the ScreenIdleMode::KeepAwake setting
        // will not be enough to keep the device from shutting down.. that's alright, because when the
        // video recorder (or encoder) is active, the OS will prevent the device from going into standby automatically.
        qDebug() << "ignoring invisible signal... keep running!";
        break;
    default:
        // not really treating thumbnail/invisible differently.  could have connected both signals to a single slot really.
        runStateMachine(StateMinimized);
        break;
    }
}


void BestCam::resourceWarning()
{
    switch (mState) {
    case StatePhotoCapture:
        // just set a flag that we should handle the resourceWarning after capture completes
        mDeferredResourceWarning = true;
        break;
    default:
        runStateMachine(StatePowerDown);
        break;
    }
}


void BestCam::poweringDown()
{
    runStateMachine(StatePowerDown);
}

//Todo: Cleanup not really needed for airhockey
void BestCam::onDisplayDirectionChanging(bb::cascades::DisplayDirection::Type displayDirection,
                                         bb::cascades::UIOrientation::Type orientation)
{
    qDebug() << "onDisplayOrientationChange()";

    // this will only be called when supported orientations are activated.
    // on the Q10, there is only one official orientation, and on most other devices, 180 degrees
    // is not supported.  we have to also hook into the orientation sensor api in order to cover all angles
    // on all devices.
    qDebug() << "display direction change:" << displayDirection;
    // note: this was only a vf orientation change event, so leave the QOrientationSensor reading as its last cached value...
    mDisplayDirection = displayDirection;
    updateAngles();
    // silence compiler warning
    (void)orientation;
}

//Todo: Cleanup not really needed for airhockey
void BestCam::updateAngles()
{
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
    // (standing behind the device, facing the user), the device has been rotated in the opposite direction from that which
    // the user would perceive.  Once you understand this distinction, the front/rear decisions below will make more sense.
    // Although confusingly, CAMERA_UNIT_REAR is the camera facing in the same direction as the user (it faces out
    // the rear of the device), and CAMERA_UNIT_FRONT is the camera facing towards the user.

    // here, I will reverse the nav's reported rotation, to bring it in line with the screen and camera's
    // co-ordinate reference.  eg. turning the device clockwise yields a rotation of 90 (nav reports 270)
    int clockwiseDisplayAngle = (360 - mDisplayDirection) % 360;
    int clockwiseOrientationAngle = (360 - mOrientationDirection) % 360;

    // note that the device orientation is not dependent on which camera (front vs rear) we are using when used with
    // camera_set_device_orientation().  the distinction is performed by the camera service.
    mDeviceOrientation = clockwiseOrientationAngle;

    // now account for front/rear-facing camera reversal of rotation direction
    if (mUnit == CAMERA_UNIT_FRONT) {
        mDesiredVfAngle = (360 - clockwiseDisplayAngle) % 360;
        mDesiredCapAngle = (360 - clockwiseOrientationAngle) % 360;
    } else {
        mDesiredVfAngle = clockwiseDisplayAngle;
        mDesiredCapAngle = clockwiseOrientationAngle;
    }

    qDebug() << "display direction:" << mDisplayDirection
             << "orientation direction:" << mOrientationDirection
             << "desired vf angle:" << mDesiredVfAngle
             << "desired cap angle:" << mDesiredCapAngle
             << "device orientation: " << mDeviceOrientation;

    // now that we know which way is up, let's decide if we need to do anything about it.
    switch (mState) {
    case StatePhotoVf:
        updatePhotoAngle();
        break;
    case StateVideoVf:
        updateVideoAngle();
        break;
    default:
        // we can't change the angle while recording, or taking a picture, or starting up, so may as well ignore.
        // TODO: could just set a deferred flag and deal with it on a state transition.
        qDebug() << "not in a stable viewfinder state, ignoring angle change";
        break;
    }
}

//Todo: Cleanup not really needed for airhockey
void BestCam::updatePhotoAngle()
{
    int i;
    int err = EOK;
    err = camera_get_photovf_property(mHandle, CAMERA_IMGPROP_ROTATION, &mVfAngle);

    if (mRequireUprightVf) {
        // if required, let's select a physical viewfinder buffer rotation which will result in an upright buffer.
        // if this is not possible, then we have to make up the difference with a screen rotation effect.
        // check whether the desired angle is available...
        for (i=0; i<mNumPhotoVfRotations; i++) {
            if (mPhotoVfRotations[i] == mDesiredVfAngle) break;
        }
        if (i == mNumPhotoVfRotations) {
            qDebug() << "desired photovf angle" << mDesiredVfAngle << "is not available";
            // we'll have to rely on screen alone in this case.
        } else {
            err = camera_set_photovf_property(mHandle, CAMERA_IMGPROP_ROTATION, mDesiredVfAngle);
            if (err) {
                qDebug() << "failed to set photovf angle" << mDesiredVfAngle << "err:" << err;
            } else {
                mVfAngle = mDesiredVfAngle;
                qDebug() << "set photovf rotation" << mVfAngle;
            }
        }
    }

    if (mRequireUprightCapture) {
        // if required, let's select a physical capture buffer rotation which will result in an upright buffer.
        // not all platforms support this, but instead depend on EXIF metadata in order to instruct picture viewers to
        // display the JPEG in the correct orientation.
        // see the note below about EXIF orientation tags...  It's actually arguable whether physically rotating the buffer is
        // necessary if EXIF tags are respected by all photo viewers.  We will try our best in this code to be friendly :)
        // check whether the desired angle is available...
        for (i=0; i<mNumPhotoRotations; i++) {
            if (mPhotoRotations[i] == mDesiredCapAngle) break;
        }
        if (i == mNumPhotoRotations) {
            qDebug() << "desired photo output angle" << mDesiredCapAngle << "is not available";
            // we'll have to rely on EXIF alone in this case.
        } else {
            err = camera_set_photo_property(mHandle, CAMERA_IMGPROP_ROTATION, mDesiredCapAngle);
            if (err) {
                qDebug() << "failed to set photo output angle" << mDesiredCapAngle << "err:" << err;
            } else {
                mCaptureAngle = mDesiredCapAngle;
                qDebug() << "set photo output rotation" << mCaptureAngle;
            }
        }
    }

    // compute screen display angle now that we know how the viewfinder buffers are being rotated.
    // remember: desired angle = viewfinder angle + window angle.  solve for viewfinder angle:
    mWindowAngle = (360 + mDesiredVfAngle - mVfAngle) % 360;  // note +360 to avoid negative numbers in modulo math

    // all we need to do here is update the screen window associated with the viewfinder.
    screen_window_t win = mFwc->windowHandle();
    if (!win) {
        qDebug() << "no window handle available to update";
    } else {
        screen_set_window_property_iv(win, SCREEN_PROPERTY_ROTATION, (int*)&mWindowAngle);
        int mirror = 0;
        int flip = 0;
        if (mUnit == CAMERA_UNIT_FRONT) {
            // NOTE: since mirroring is applies after rotation in the order-of-operations, it is necessary to
            // decide between a flip or a mirror in order to make the screen behave like a mirror on the front camera.
            if (mWindowAngle % 180) {
                flip = 1;
            } else {
                mirror = 1;
            }
            screen_set_window_property_iv(win, SCREEN_PROPERTY_MIRROR, &mirror);
            screen_set_window_property_iv(win, SCREEN_PROPERTY_FLIP, &flip);
        }
    }

    // always tell camera which way device is oriented. this allows the EXIF orientation tags to be set correctly,
    // so that physically sideways buffers can be displayed correctly in a photo viewer.  telling the camera which way is up
    // also helps to optimize exposure profiles and aids the face detection algorithms (which only detect upright faces).
    // NOTE: If for some reason we want to rely solely on the physical buffer orientation set by CAMERA_IMGPROP_ROTATION above, then
    // the CAMERA_IMGPROP_METAORIENTATIONHINT photo property can be set to 0.  This would cause the EXIF orientation tag to not be
    // written to the JPEG.  Since many devices do not support physical buffer rotation though, we shouldn't do this.
    err = camera_set_device_orientation(mHandle, mDeviceOrientation);
    if (err) {
        qDebug() << "failed to set camera device orientation to" << mDeviceOrientation <<" err:" << err;
    } else {
        qDebug() << "camera device orientation set to" << mDeviceOrientation;
    }
}

//Todo: Cleanup not really needed for airhockey
void BestCam::updateVideoAngle()
{
    int i;
    int err = EOK;
    err = camera_get_videovf_property(mHandle, CAMERA_IMGPROP_ROTATION, &mVfAngle);

    if (mRequireUprightVf) {
        // if required, let's select a physical viewfinder buffer rotation which will result in an upright buffer.
        // if this is not possible, then we have to make up the difference with a screen rotation effect.
        // NOTE: that this is only typically required when we are starting video recording and CAMERA_FEATURE_PREVIEWISVIDEO is
        // asserted.  if we were processing viewfinder pixels and expecting the first one to be in the top-left position, then we
        // would also set this flag.
        // check whether the desired angle is available...
        for (i=0; i<mNumVideoVfRotations; i++) {
            if (mVideoVfRotations[i] == mDesiredVfAngle) break;
        }
        if (i == mNumVideoVfRotations) {
            qDebug() << "desired videovf angle" << mDesiredVfAngle << "is not available";
            // we'll have to rely on screen alone in this case.
        } else {
            err = camera_set_videovf_property(mHandle, CAMERA_IMGPROP_ROTATION, mDesiredVfAngle);
            if (err) {
                qDebug() << "failed to set videovf angle" << mDesiredVfAngle << "err:" << err;
            } else {
                mVfAngle = mDesiredVfAngle;
                qDebug() << "set videovf rotation" << mVfAngle;
            }
        }
    }

    if (mRequireUprightCapture) {
        // if required, let's select a physical capture buffer rotation which will result in an upright buffer.
        // today, all platforms support this in video mode, but in the future, we may need to rely on metadata in the video stream.
        err = camera_get_video_property(mHandle, CAMERA_IMGPROP_ROTATION, &mCaptureAngle);
        if (err) {
            qDebug() << "failed to query capture angle. err:" << err;
        } else {
            // check whether the desired angle is available...
            for (i=0; i<mNumVideoRotations; i++) {
                if (mVideoRotations[i] == mDesiredCapAngle) break;
            }
            if (i == mNumVideoRotations) {
                qDebug() << "desired video output angle" << mDesiredCapAngle << "is not available";
                // we'll have to rely on metadata alone in this case. (which we cannot do today, since there is no standard for mp4)
            } else {
                err = camera_set_video_property(mHandle, CAMERA_IMGPROP_ROTATION, mDesiredCapAngle);
                if (err) {
                    qDebug() << "failed to set video output angle" << mDesiredCapAngle << "err:" << err;
                } else {
                    mCaptureAngle = mDesiredCapAngle;
                    qDebug() << "set video output rotation" << mCaptureAngle;
                }
            }
        }
    }

    // compute screen display angle now that we know how the viewfinder buffers are being rotated.
    // remember: desired angle = viewfinder angle + window angle.  solve for viewfinder angle:
    mWindowAngle = (360 + mDesiredVfAngle - mVfAngle) % 360;  // note +360 to avoid negative numbers in modulo math

    if (mRequireUprightVf) {
        // NOTE: in the video case, since viewfinder buffers may be required to match the angle of the video buffers, we must apply a
        // correction offset here (which is the difference between the UI angle and the device orientation angle).  this is typically needed
        // when recording with the device held in an orientation where the UI cannot be rotated.  (eg. most angles on a Q10 or upside-down on a Z10).
        // also note that we are only applying this offset in the case where mRequireUprightVf is asserted.  this is to ensure that
        // this adjustment is only made when we are reconfiguring the viewfinder buffers during recording.
        // There is probably a less confusing way to orchestrate this series of corner-case events, but for now, this should be fine.
        uint32_t uiOffsetAngle = (360 + mOrientationDirection - mDisplayDirection) % 360;
        if (mUnit == CAMERA_UNIT_FRONT) {
            uiOffsetAngle = (360 - uiOffsetAngle) % 360;
        }
        mWindowAngle = (mWindowAngle + uiOffsetAngle) % 360;
    }

    // all we need to do here is update the screen window associated with the viewfinder.
    screen_window_t win = mFwc->windowHandle();
    if (!win) {
        qDebug() << "no window handle available to update";
    } else {
        screen_set_window_property_iv(win, SCREEN_PROPERTY_ROTATION, (int*)&mWindowAngle);
        int mirror = 0;
        int flip = 0;
        if (mUnit == CAMERA_UNIT_FRONT) {
            // NOTE: since mirroring is applies after rotation in the order-of-operations, it is necessary to
            // decide between a flip or a mirror in order to make the screen behave like a mirror on the front camera.
            if (mWindowAngle % 180) {
                flip = 1;
            } else {
                mirror = 1;
            }
            screen_set_window_property_iv(win, SCREEN_PROPERTY_MIRROR, &mirror);
            screen_set_window_property_iv(win, SCREEN_PROPERTY_FLIP, &flip);
        }
    }

    // always tell camera which way device is oriented.
    // even though we don't have any metadata tags which can be used with video recordings, telling the camera which way is up
    // helps to optimize exposure profiles and aids the face detection algorithms (which only detect upright faces).
    err = camera_set_device_orientation(mHandle, mDeviceOrientation);
    if (err) {
        qDebug() << "failed to set camera device orientation to" << mDeviceOrientation <<" err:" << err;
    } else {
        qDebug() << "camera device orientation set to" << mDeviceOrientation;
    }
}

//Fixme: Cleanup not needed for airhockey ...
int BestCam::startOrientationReadings()
{
    if (!mOrientationSensor) {
        mOrientationSensor = new QOrientationSensor(this);
        if (!mOrientationSensor) {
            qWarning() << "failed to allocate QOrientationSensor.";
            return ENOMEM;
        }
        mOrientationSensor->setSkipDuplicates(true);
        mOrientationSensor->setDataRate(1);
        mOrientationSensor->setAlwaysOn(true);
        if (!QObject::connect(mOrientationSensor, SIGNAL(readingChanged()), this, SLOT(onOrientationReadingChanged()))) {
            qWarning() << "failed to connect readingChanged signal";
            return EIO;
        }
    }
    mOrientationSensor->start();
    return EOK;
}

//fixme Cleanup: Not needed
void BestCam::stopOrientationReadings()
{
    if (mOrientationSensor) {
        mOrientationSensor->stop();
    }
}

//fixme Cleanup not needed
void BestCam::onOrientationReadingChanged()
{
    if (!mOrientationSensor) {
        return;
    }
    qDebug() << "onOrientationReadingChanged()";
    QOrientationReading* reading = mOrientationSensor->reading();
    if (reading) {
        switch(reading->orientation()) {
        case QOrientationReading::TopUp:
            mOrientationDirection = DisplayDirection::North;
            break;
        case QOrientationReading::TopDown:
            mOrientationDirection = DisplayDirection::South;
            break;
        case QOrientationReading::LeftUp:
            mOrientationDirection = DisplayDirection::West;
            break;
        case QOrientationReading::RightUp:
            mOrientationDirection = DisplayDirection::East;
            break;
        default:
            // this is an unhandled direction (eg. face-up or face-down), so just reuse the last known reading
            break;
        }
        // note: this was only a QOrientationSensor change event, so leave the UI display direction reading at its last known value...
        updateAngles();
    }
}


void BestCam::onVfParentLayoutFrameChanged(QRectF frame)
{
    qDebug() << "viewfinder parent size:" << frame;
    // by default, the ForeignWindowControl that houses the viewfinder will scale to fit the available screen real-estate.
    // this will likely lead to it being stretched in one direction.
    // we're going to un-stretch it, and peg it's aspect ratio at 16:9 (or 9:16).
    mVfContainerSize = frame;
    constrainViewfinderAspectRatio();
}


void BestCam::constrainViewfinderAspectRatio()
{
    if ((mVfContainerSize.width() == 0) ||
        (mVfContainerSize.height() == 0)) {
        // one of the dimensions is a zero, not wise to do math with this yet
        return;
    }

    // first, determine whether we are aiming for a portrait or landscape aspect ratio (eg. 9:16 or 16:9)
    float aspect = (float)mVfWidth / (float)mVfHeight;
    // if window is displayed at 90 or 270 degrees, then flip the target aspect ratio
    if (mDesiredVfAngle % 180) {
        aspect = 1 / aspect;
    }

    // until we figure otherwise, fit to max size
    float width = mVfContainerSize.width();
    float height = mVfContainerSize.height();

    if (height * aspect > width) {
        // constrain height, since width cannot be increased
        height = width / aspect;
    } else {
        // constrain width
        width = height * aspect;
    }
    mFwc->setPreferredSize(width, height);
    qDebug() << "resized viewfinder to" << width << "x" << height <<"to maintain aspect ratio" << aspect;
}

//Fixme: Cleanup not really needed for airhockey... or at least reduce
int BestCam::discoverCameraCapabilities()
{
    // In this function, we will query and cache some core camera capabilities to use for later configuration.
    // 1. photo? video?
    // 2. photo format, rotations, resolutions
    // 3. photo viewfinder format, rotations, resolutions
    // 4. video format, rotations, resolutions
    // 5. video viewfinder format, rotations, resolutions


    // first check for photo & video support
    mCanDoPhoto = mCanDoVideo = false;
    if (camera_has_feature(mHandle, CAMERA_FEATURE_PHOTO)) {
        mCanDoPhoto = true;
    }
    if (camera_has_feature(mHandle, CAMERA_FEATURE_VIDEO)) {
        mCanDoVideo = true;
    }
    emit canDoPhotoChanged(mCanDoPhoto);
    emit canDoVideoChanged(mCanDoVideo);

    int err = EOK;
    if (mCanDoPhoto) {
        err = discoverPhotoCapabilities();
        if (err) {
            qDebug() << "failed to discover photo capabilities.";
            return err;
        }
        err = discoverPhotoVfCapabilities();
        if (err) {
            qDebug() << "failed to discover photovf capabilities.";
            return err;
        }
    }

    if (mCanDoVideo) {
        err = discoverVideoCapabilities();
        if (err) {
            qDebug() << "failed to discover video capabilities.";
            return err;
        }
        err = discoverVideoVfCapabilities();
        if (err) {
            qDebug() << "failed to discover videovf capabilities.";
            return err;
        }
    }

    return err;
}

//Todo: Cleanup not really needed for airhockey
int BestCam::discoverPhotoCapabilities()
{
    int err = EOK;
    // clean up any pre-discovered stuff
    delete[] mPhotoRotations;
    delete[] mPhotoResolutions;
    mNumPhotoRotations = 0;
    mNumPhotoResolutions = 0;

    // now query the current format for photo capture.  in this sample, we are not implementing configurable formats,
    // however we should make sure to know what the default is so that we can query some other discovery functions.
    err = camera_get_photo_property(mHandle, CAMERA_IMGPROP_FORMAT, &mPhotoFormat);
    if (err) {
        qDebug() << "failed to query photo format";
    } else {
        // log it.
        qDebug() << "current photo format is:" << mPhotoFormat;

        // now query which buffer rotations are available for this format.
        // since we don't know how large the list may be (technically it shouldn't be more than 4 entries), we can
        // query the function in pre-sizing mode -- eg. by providing numasked=0 and a NULL array).
        err = camera_get_photo_rotations(mHandle, mPhotoFormat, false, 0, &mNumPhotoRotations, NULL, NULL);
        if (err) {
            qDebug() << "failed to query num photo rotations";
        } else {
            // now allocate enough storage to hold the array
            mPhotoRotations = new uint32_t[mNumPhotoRotations];
            if (!mPhotoRotations) {
                qDebug() << "failed to allocate storage for photo rotations array";
                err = ENOMEM;
            } else {
                // now fill the array
                err = camera_get_photo_rotations(mHandle,
                                                 mPhotoFormat,
                                                 false,  // we are not asking about burst mode
                                                 mNumPhotoRotations,
                                                 &mNumPhotoRotations,
                                                 mPhotoRotations,
                                                 NULL);
                if (err) {
                    qDebug() << "failed to query photo rotations";
                } else {
                    // log the list.
                    qDebug() << "supported photo rotations:";
                    for (int i=0; i<mNumPhotoRotations; i++) {
                        qDebug() << mPhotoRotations[i];
                    }
                }
            }
        }
    }

    if (err == EOK) {
        // now query the supported photo resolutions
        err = camera_get_photo_output_resolutions(mHandle, mPhotoFormat, 0, &mNumPhotoResolutions, NULL);
        if (err) {
            qDebug() << "failed to query num photo resolutions";
        } else {
            // now allocate enough storage to hold the array
            mPhotoResolutions = new camera_res_t[mNumPhotoResolutions];
            if (!mPhotoResolutions) {
                qDebug() << "failed to allocate storage for photo resolutions array";
                err = ENOMEM;
            } else {
                // now fill the array
                err = camera_get_photo_output_resolutions(mHandle,
                                                          mPhotoFormat,
                                                          mNumPhotoResolutions,
                                                          &mNumPhotoResolutions,
                                                          mPhotoResolutions);
                if (err) {
                    qDebug() << "failed to query photo resolutions";
                } else {
                    // log the list
                    qDebug() << "supported photo resolutions:";
                    for (unsigned int i=0; i<mNumPhotoResolutions; i++) {
                        qDebug() << mPhotoResolutions[i].width << "x" << mPhotoResolutions[i].height;
                    }
                }
            }
        }
    }

    return err;
}

//Todo: Cleanup not really needed for airhockey
int BestCam::discoverPhotoVfCapabilities()
{
    int err = EOK;
    // clean up any pre-discovered stuff
    delete[] mPhotoVfRotations;
    delete[] mPhotoVfResolutions;
    mNumPhotoVfRotations = 0;
    mNumPhotoVfResolutions = 0;

    // now query the current format for photo viewfinder.  in this sample, we are not implementing configurable formats,
    // however we should make sure to know what the default is so that we can query some other discovery functions.
    err = camera_get_photovf_property(mHandle, CAMERA_IMGPROP_FORMAT, &mPhotoVfFormat);
    if (err) {
        qDebug() << "failed to query photovf format";
    } else {
        // log it.
        qDebug() << "current photovf format is:" << mPhotoVfFormat;

        // now query which buffer rotations are available for this format.
        // since we don't know how large the list may be (technically it shouldn't be more than 4 entries), we can
        // query the function in pre-sizing mode -- eg. by providing numasked=0 and a NULL array).
        err = camera_get_photo_vf_rotations(mHandle, mPhotoVfFormat, 0, &mNumPhotoVfRotations, NULL, NULL);
        if (err) {
            qDebug() << "failed to query num photovf rotations";
        } else {
            // now allocate enough storage to hold the array
            mPhotoVfRotations = new uint32_t[mNumPhotoVfRotations];
            if (!mPhotoVfRotations) {
                qDebug() << "failed to allocate storage for photovf rotations array";
                err = ENOMEM;
            } else {
                // now fill the array
                err = camera_get_photo_vf_rotations(mHandle,
                                                    mPhotoVfFormat,
                                                    mNumPhotoVfRotations,
                                                    &mNumPhotoVfRotations,
                                                    mPhotoVfRotations,
                                                    NULL);
                if (err) {
                    qDebug() << "failed to query photovf rotations";
                } else {
                    // log the list.
                    qDebug() << "supported photovf rotations:";
                    for (int i=0; i<mNumPhotoVfRotations; i++) {
                        qDebug() << mPhotoVfRotations[i];
                    }
                }
            }
        }
    }

    if (err == EOK) {
        // now query the supported photo vf resolutions
        err = camera_get_photo_vf_resolutions(mHandle, 0, &mNumPhotoVfResolutions, NULL);
        if (err) {
            qDebug() << "failed to query num photovf resolutions";
        } else {
            // now allocate enough storage to hold the array
            mPhotoVfResolutions = new camera_res_t[mNumPhotoVfResolutions];
            if (!mPhotoVfResolutions) {
                qDebug() << "failed to allocate storage for photovf resolutions array";
                err = ENOMEM;
            } else {
                // now fill the array
                err = camera_get_photo_vf_resolutions(mHandle,
                                                      mNumPhotoVfResolutions,
                                                      &mNumPhotoVfResolutions,
                                                      mPhotoVfResolutions);
                if (err) {
                    qDebug() << "failed to query photovf resolutions";
                } else {
                    // log the list
                    qDebug() << "supported photovf resolutions:";
                    for (unsigned int i=0; i<mNumPhotoVfResolutions; i++) {
                        qDebug() << mPhotoVfResolutions[i].width << "x" << mPhotoVfResolutions[i].height;
                    }
                }
            }
        }
    }

    return err;
}

//Fixme: use to discover the max framerate instead of the other spot I hacked it in
int BestCam::discoverVideoCapabilities()
{
    int err = EOK;
    // clean up any pre-discovered stuff
    delete[] mVideoRotations;
    delete[] mVideoResolutions;
    mNumVideoRotations = 0;
    mNumVideoResolutions = 0;

    // now query the current format for video capture.  in this sample, we are not implementing configurable formats,
    // however we should make sure to know what the default is so that we can query some other discovery functions.
    // TODO: this is embarrassing.. apparently the video property is not queryable presently. that is okay, since on all
    // current platforms, the video and videovf streams are the same.  we can add a check here and query the videovf as a workaround.
    if (camera_has_feature(mHandle, CAMERA_FEATURE_PREVIEWISVIDEO)) {
        err = camera_get_videovf_property(mHandle, CAMERA_IMGPROP_FORMAT, &mVideoFormat);
    } else {
        err = camera_get_video_property(mHandle, CAMERA_IMGPROP_FORMAT, &mVideoFormat);
    }
    if (err) {
        qDebug() << "failed to query video format";
    } else {
        // log it.
        qDebug() << "current video format is:" << mVideoFormat;

        // now query which buffer rotations are available for this format.
        // since we don't know how large the list may be (technically it shouldn't be more than 4 entries), we can
        // query the function in pre-sizing mode -- eg. by providing numasked=0 and a NULL array).
        err = camera_get_video_rotations(mHandle, mVideoFormat, 0, &mNumVideoRotations, NULL, NULL);
        if (err) {
            qDebug() << "failed to query num video rotations";
        } else {
            // now allocate enough storage to hold the array
            mVideoRotations = new uint32_t[mNumVideoRotations];
            if (!mVideoRotations) {
                qDebug() << "failed to allocate storage for video rotations array";
                err = ENOMEM;
            } else {
                // now fill the array
                err = camera_get_video_rotations(mHandle,
                                                 mVideoFormat,
                                                 mNumVideoRotations,
                                                 &mNumVideoRotations,
                                                 mVideoRotations,
                                                 NULL);
                if (err) {
                    qDebug() << "failed to query video rotations";
                } else {
                    // log the list.
                    qDebug() << "supported video rotations:";
                    for (int i=0; i<mNumVideoRotations; i++) {
                        qDebug() << mVideoRotations[i];
                    }
                }
            }
        }
    }

    if (err == EOK) {
        // now query the supported video resolutions
        err = camera_get_video_output_resolutions(mHandle, 0, &mNumVideoResolutions, NULL);
        if (err) {
            qDebug() << "failed to query num video resolutions";
        } else {
            // now allocate enough storage to hold the array
            mVideoResolutions = new camera_res_t[mNumVideoResolutions];
            if (!mVideoResolutions) {
                qDebug() << "failed to allocate storage for video resolutions array";
                err = ENOMEM;
            } else {
                // now fill the array
                err = camera_get_video_output_resolutions(mHandle,
                                                          mNumVideoResolutions,
                                                          &mNumVideoResolutions,
                                                          mVideoResolutions);
                if (err) {
                    qDebug() << "failed to query video resolutions";
                } else {
                    // log the list
                    qDebug() << "supported video resolutions:";
                    for (unsigned int i=0; i<mNumVideoResolutions; i++) {
                        qDebug() << mVideoResolutions[i].width << "x" << mVideoResolutions[i].height;
                    }
                }
            }
        }
    }

    return err;
}

//Fixme: discover maximum video vf capabilities
int BestCam::discoverVideoVfCapabilities()
{
    int err = EOK;
    // clean up any pre-discovered stuff
    delete[] mVideoVfRotations;
    delete[] mVideoVfResolutions;
    mNumVideoVfRotations = 0;
    mNumVideoVfResolutions = 0;

    // now query the current format for video viewfinder.  in this sample, we are not implementing configurable formats,
    // however we should make sure to know what the default is so that we can query some other discovery functions.
    err = camera_get_videovf_property(mHandle, CAMERA_IMGPROP_FORMAT, &mVideoVfFormat);
    if (err) {
        qDebug() << "failed to query videovf format";
    } else {
        // log it.
        qDebug() << "current videovf format is:" << mVideoVfFormat;

        // now query which buffer rotations are available for this format.
        // since we don't know how large the list may be (technically it shouldn't be more than 4 entries), we can
        // query the function in pre-sizing mode -- eg. by providing numasked=0 and a NULL array).
        err = camera_get_video_vf_rotations(mHandle, mVideoVfFormat, 0, &mNumVideoVfRotations, NULL, NULL);
        if (err) {
            qDebug() << "failed to query num videovf rotations";
        } else {
            // now allocate enough storage to hold the array
            mVideoVfRotations = new uint32_t[mNumVideoVfRotations];
            if (!mVideoVfRotations) {
                qDebug() << "failed to allocate storage for videovf rotations array";
                err = ENOMEM;
            } else {
                // now fill the array
                err = camera_get_video_vf_rotations(mHandle,
                                                    mVideoVfFormat,
                                                    mNumVideoVfRotations,
                                                    &mNumVideoVfRotations,
                                                    mVideoVfRotations,
                                                    NULL);
                if (err) {
                    qDebug() << "failed to query videovf rotations";
                } else {
                    // log the list.
                    qDebug() << "supported videovf rotations:";
                    for (int i=0; i<mNumVideoVfRotations; i++) {
                        qDebug() << mVideoVfRotations[i];
                    }
                }
            }
        }
    }

    if (err == EOK) {
        // now query the supported video vf resolutions
        err = camera_get_video_vf_resolutions(mHandle, 0, &mNumVideoVfResolutions, NULL);
        if (err) {
            qDebug() << "failed to query num videovf resolutions";
        } else {
            // now allocate enough storage to hold the array
            mVideoVfResolutions = new camera_res_t[mNumVideoVfResolutions];
            if (!mVideoVfResolutions) {
                qDebug() << "failed to allocate storage for videovf resolutions array";
                err = ENOMEM;
            } else {
                // now fill the array
                err = camera_get_video_vf_resolutions(mHandle,
                                                      mNumVideoVfResolutions,
                                                      &mNumVideoVfResolutions,
                                                      mVideoVfResolutions);
                if (err) {
                    qDebug() << "failed to query videovf resolutions";
                } else {
                    // log the list
                    qDebug() << "supported videovf resolutions:";
                    for (unsigned int i=0; i<mNumVideoVfResolutions; i++) {
                        qDebug() << mVideoVfResolutions[i].width << "x" << mVideoVfResolutions[i].height;
                    }
                }
            }
        }
    }

    return err;
}


camera_res_t* BestCam::matchAspectRatio(camera_res_t* target, camera_res_t* resList, int numRes, float accuracy)
{
    // this function will scan the list (resList) for resolutions which match the input aspect ratio (target) within
    // a margin of error of accuracy %.  eg. 0% means only match exact aspect ratios.
    camera_res_t* best = NULL;
    if (target && resList && numRes) {
        float targetRatio = (float)(target->width) / (float)(target->height);
        float bestError = 0;
        for (int i=0; i<numRes; i++) {
            float thisRatio = (float)(resList[i].width) / (float)(resList[i].height);
            float thisError = fabs((thisRatio / targetRatio) - 1.0f);
            if (thisError <= accuracy) {
                if (!best || (thisError < bestError)) {
                    best = &resList[i];
                    bestError = thisError;
                }
            }
        }
    }
    return best;
}
