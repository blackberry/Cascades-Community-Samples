/*
 * Copyright (c) 2016 BlackBerry Limited.
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

#ifndef SCREENCAPTURE_HPP_
#define SCREENCAPTURE_HPP_


#include <camera/camera_api.h>

#include <QDebug>
#include <QObject>
#include <QString>
#include <QVariantList>

class ScreenCapture : public QObject {
	Q_OBJECT

	Q_PROPERTY(bool proceed READ proceed)
	Q_PROPERTY(QVariantList viewfinderResolutions READ viewfinderResolutions)
	Q_PROPERTY(QVariantList viewfinderFrameRates READ viewfinderFrameRates)

public:
	ScreenCapture(QObject *parent);
	virtual ~ScreenCapture();

	bool proceed();
	QVariantList& viewfinderResolutions();
	QVariantList& viewfinderFrameRates();


public Q_SLOTS:
	void          initialize();
	void          cleanup();
	void          setSource(int cameraUnit);
	void          openCamera();
	void          setViewfinderResolution(int selectedIndex);
	void          setViewfinderFrameRate(int frameRate);
	void          setViewfinderRotation(int angle);
	void          setViewfinderFrameType(int selectedIndex);
	void          setVideoFrameRate(int frameRate);
	void          setConfigured(bool configured);
	void          startViewfinder();
	void          startVideo();
	void          stopVideo();
	void          startEncode();
	void          stopEncode();

public:
	bool 			_captureNextRGB;
	int 			_frameCount;

private:
   	bool            _configured;
	int             _sourceCameraUnit;
	camera_unit_t   _unit;
	camera_handle_t _handle;
	uint32_t        _mode;
	camera_res_t    _resolutions[40];
	double			_framerates[40];
	int             _vfWidth;
	int             _vfHeight;
	double          _vfFrameRate;
	double           _frameRate;
	int             _vfRotationAngle;
	int             _vfFrameType;
   	bool            _cameraOpen;
   	bool            _doMovie;
   	bool            _doQuery;
   	bool            _doVideoLight;
	int             _RGBviewfinder;
	bool            _recordingActive;
	bool            _videoLightActive;
	bool            _encodingActive;
	long long       _available;
	bool            _videoroll;
	int				_fd;
	char            _filename[CAMERA_ROLL_NAMELEN];
	int 			_videoStabilization;
	bool 			_appWindow;
	bool 			_rgbViewfinder;


	camera_error_t _err;
};

#endif /* SCREENCAPTURE_HPP_ */
