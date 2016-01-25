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

#include <stdio.h>
#include <sys/mman.h>
#include <sys/neutrino.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "ScreenCapture.hpp"

char const *vfWndWindowId = "camera_viewfinder_window";


extern "C" void camera_describe_buffer(camera_buffer_t*, bool);

static void bgra_to_rgb(uint8_t *dst, uint8_t *src, size_t cnt_pix)
{
    for (size_t i=0; i<cnt_pix; ++i) {
		*dst++ = src[2];
		*dst++ = src[1];
		*dst++ = src[0];
		src += 4;
	}
}
static int save_rgb_buffer_as_ppm(const char *fn, unsigned char* data, int width,
		int height, int stride, camera_frametype_t frameType)
{
	int rc=0;
	char header[256];
	uint8_t *rgb_scanline=NULL;

	int lenheader = snprintf(header, sizeof header, "P6\n%d %d\n255\n", width, height);

	int fd = open(fn, O_RDWR|O_CREAT|O_TRUNC, 0660);

	if (fd == -1) {
		perror("save_vf_buffer()/open():");
		rc = -1;
		goto cleanup;
	}

	write(fd, header, lenheader);

	rgb_scanline = (uint8_t*)malloc(3*width);
	if (rgb_scanline == NULL) {
		fprintf(stderr, "out of memory while allocating rgb_scanline\n");
		rc = -1;
		goto cleanup;
	}
	for (int i=0; i<height; i++) {
		if (frameType == CAMERA_FRAMETYPE_RGB8888) {
			bgra_to_rgb(rgb_scanline, data, width);
		}
		rc = write(fd, rgb_scanline, 3*width);
		if (rc != 3*width) {
			printf("%s(): write(%d) returns %d\n", __func__, width, rc);
			break;
		}
		data += stride;
	}

cleanup:
	close(fd);
	free(rgb_scanline);

	return rc;
}


int save_jpeg_buffer_to_roll(camera_handle_t handle, unsigned char* data, int len)
{
	int rc=0;
	int fd;
	char filename[CAMERA_ROLL_NAMELEN];
	rc = camera_roll_open_photo(handle, &fd, filename, CAMERA_ROLL_NAMELEN, CAMERA_ROLL_PHOTO_FMT_JPG);
	if (rc != EOK) return rc;
	write(fd, data, len);
	camera_roll_close_photo(fd);
	return rc;
}


void status_callback(camera_handle_t handle,
					 camera_devstatus_t devstatus,
					 uint16_t extra,
					 void *arg)
{
	qDebug() << "ScreenCapture: status_callback: " << devstatus << " : " << extra;
}


void video_status_callback(camera_handle_t handle,
						   camera_devstatus_t devstatus,
						   uint16_t extra,
						   void *arg)
{
	qDebug() << "ScreenCapture: video_status_callback: " << devstatus << " : " << extra;
}

void video_callback(camera_handle_t handle,
					camera_buffer_t* buffer,
					void* arg)

{

	qDebug() << "ScreenCapture: video_callback: " << ((ScreenCapture *)arg)->_frameCount;
	qDebug() << "framesize: " << buffer->framesize;
	qDebug() << "framebuf: " << buffer->framebuf;
	qDebug() << "framemetasize: " << buffer->framemetasize;
	qDebug() << "framemeta: " << buffer->framemeta;
	qDebug() << "frametimestamp: " << buffer->frametimestamp;
	qDebug() << "frameorientation: " << buffer->frameorientation;
	qDebug() << "frametype: " << buffer->frametype;

	int x = 0;
	int y = 0;

	switch(buffer->frametype) {
	case CAMERA_FRAMETYPE_NV12:
		qDebug() << "nv12.width: " << buffer->framedesc.nv12.width;
		qDebug() << "nv12.height: " << buffer->framedesc.nv12.height;
		qDebug() << "nv12.stride: " << buffer->framedesc.nv12.stride;
		qDebug() << "nv12.uv_offset: " << buffer->framedesc.nv12.uv_offset;
		qDebug() << "nv12.uv_stride: " << buffer->framedesc.nv12.uv_stride;


		break;
	case CAMERA_FRAMETYPE_RGB8888:
		qDebug() << "rgb8888.width: " << buffer->framedesc.rgb8888.width;
		qDebug() << "rgb8888.height: " << buffer->framedesc.rgb8888.height;
		qDebug() << "rgb8888.stride: " << buffer->framedesc.rgb8888.stride;

		break;

		default:
			break;
	}


	((ScreenCapture *)arg)->_frameCount++;
}


void shutter_callback(camera_handle_t handle,
					  void *arg)
{
	printf("SHUTTER CALLBACK!\n");
}

void jpeg_callback(camera_handle_t handle,
				   camera_buffer_t *buf,
				   void *arg)
{
	static int count = 0;
	printf("saving image # %d of size %d\n", ++count, buf->framedesc.jpeg.bufsize);
	save_jpeg_buffer_to_roll(handle, buf->framebuf, buf->framedesc.jpeg.bufsize);
}


void review_callback(camera_handle_t handle,
					 camera_buffer_t *buf,
					 void *arg)
{
}



void adobe_callback(camera_handle_t handle,
					camera_buffer_t *buf,
					void *arg)
{
}


void resmon_callback(camera_handle_t handle,
					 bool release,
					 void* arg)
{
}


void vf_callback(camera_handle_t handle,
				  camera_buffer_t *buf,
				  void *arg)
{
	static int record_count = 0;
	if ( (buf->frametype == CAMERA_FRAMETYPE_RGB8888) && (((ScreenCapture *)arg)->_captureNextRGB == true) ) {
		char filename[80];

		sprintf(filename, "/tmp/rgbvf%08d.ppm", ++record_count);
		if (buf->framebuf != NULL) {
			save_rgb_buffer_as_ppm(filename, buf->framebuf,
					buf->framedesc.rgb8888.width, buf->framedesc.rgb8888.height,
					buf->framedesc.rgb8888.stride, buf->frametype);
			printf("Wrote out RGB buffer as %s; width=%d, height=%d, stride%d \n",
					filename, buf->framedesc.rgb8888.width,
					buf->framedesc.rgb8888.height,
					buf->framedesc.rgb8888.stride);
		}
		((ScreenCapture *)arg)->_captureNextRGB = false;
	}
}

void videoencode_callback(camera_handle_t handle,
		camera_buffer_t *buf,
		void * arg)
{
	qDebug() << "ScreenCapture: videoencode_callback: ";

	if (buf->frametype == CAMERA_FRAMETYPE_COMPRESSEDVIDEO) {
		qDebug() << "compressed video buffer size: " << buf->framedesc.compvid.bufsize;
		qDebug() << "compressed video codec: " << buf->framedesc.compvid.codec;
		qDebug() << "compressed video keyframe: " << buf->framedesc.compvid.keyframe;
		qDebug() << "compressed video bitrate: " << buf->framedesc.compvid.bitrate;
	} else {
		qDebug() << "Frame of unknown type " << buf->frametype << " received\n";
	}
}

void audioencode_callback(camera_handle_t handle,
		camera_buffer_t *buf,
		void * arg)
{
	qDebug() << "ScreenCapture: audioencode_callback: ";

	if (buf->frametype == CAMERA_FRAMETYPE_COMPRESSEDAUDIO) {
		qDebug() << "compressed audio buffer size: " << buf->framedesc.compaud.bufsize;
		qDebug() << "compressed audio codec: " << buf->framedesc.compaud.codec;
		qDebug() << "compressed audio keyframe: " << buf->framedesc.compaud.keyframe;
		qDebug() << "compressed compaud channels: " << buf->framedesc.compaud.channels;
		qDebug() << "compressed compaud bits: " << buf->framedesc.compaud.bits;
		qDebug() << "compressed compaud samplerate: " << buf->framedesc.compaud.samplerate;
		qDebug() << "compressed compaud bitrate: " << buf->framedesc.compaud.bitrate;
	} else {
		qDebug() << "Frame of unknown type " << buf->frametype << " received\n";
	}

}


ScreenCapture::ScreenCapture(QObject *parent)
	: QObject(parent)
{
	// TODO Auto-generated constructor stub
}

ScreenCapture::~ScreenCapture() {
	// TODO Auto-generated destructor stub
}

bool ScreenCapture::proceed()
{
	return (_err == EOK);
}

void ScreenCapture::setSource(int cameraUnit)
{
	_sourceCameraUnit = cameraUnit;

	if (_sourceCameraUnit == 1) {
		_unit = CAMERA_UNIT_DESKTOP;
		//appWindow = false;
	} else {
		_unit = CAMERA_UNIT_SECONDARY_DESKTOP;
		//appWindow = false;
	}
}

void ScreenCapture::initialize()
{
	_configured = false;
   	_doMovie = false;
   	_doQuery = false;
   	_doVideoLight = false;
	_unit = CAMERA_UNIT_NONE;
	_vfFrameRate = 0;
	_frameRate = 0;
	_handle = NULL;
	_recordingActive = false;
	_videoLightActive = false;
	_encodingActive = false;
	_videoroll = false;
	_cameraOpen = false;
	_videoStabilization = 0;
	_appWindow = true;
	_captureNextRGB = false;
	_rgbViewfinder = false;


   	camera_init_video_encoder();

	// find the first photo-capable camera
	camera_feature_t features[] = { CAMERA_FEATURE_VIDEO };
	_err = camera_find_capable(features,
								 sizeof(features)/sizeof(camera_feature_t),
								 CAMERA_UNIT_NONE,
								 &_unit);

	if (_err != EOK) {
		qDebug() << "error locating a capable camera: " << strerror(_err) << "\n";
	} else {
		qDebug() << "camera found: " << _unit;
	}
}

void ScreenCapture::openCamera()
{

    _mode = CAMERA_MODE_RW | CAMERA_MODE_ROLL;

	if (!_cameraOpen) {
		_err = camera_open(_unit,
						  _mode,
						  &_handle);

		if (_err != EOK) {
			qDebug() << "error " << _err << " opening camera: " << strerror(_err) << "\n";
		} else {
			qDebug() << "camera opened ... \n";
			_cameraOpen = true;
		}
	}
}

QVariantList& ScreenCapture::viewfinderResolutions()
{
	unsigned int numresolutions;

	QVariantList *list = new QVariantList();
	QVariantMap* variantMap = NULL;

	// find the available preview resolution
	_err = camera_get_video_vf_resolutions(_handle,
										  sizeof(_resolutions)/sizeof(camera_res_t),
										  &numresolutions,
										  _resolutions);
	if (_err != EOK) {
		qDebug() << "error retrieving available preview resolutions: " << strerror(_err) << "\n";
	} else {
		unsigned int r;
		for (r=0;r<numresolutions;r++) {
			variantMap = new QVariantMap();

			QString resolution = QString::number(_resolutions[r].width);
			resolution.append(" x ");
			resolution.append(QString::number(_resolutions[r].height));

			(*variantMap)[QString("label")] = QVariant(resolution);
			(*variantMap)[QString("value")] = QVariant(r);
			qDebug() << "available resolutions: " << resolution << " : " << r << "\n";

			*list << *variantMap;
		}
	}

	qDebug() << "available resolutions size: " << list->count() << "\n";

	return *list;
}

QVariantList& ScreenCapture::viewfinderFrameRates()
{
	int numframerates;
	bool bMaxMin;
	int i;

	QVariantList *list = new QVariantList();
	QVariantMap* variantMap = NULL;

	_err = camera_get_video_vf_framerates(_handle, 0, &numframerates, NULL, &bMaxMin);

	if (_err == EOK) {
		// find the available preview frame rates
		_err = camera_get_video_vf_framerates(_handle, numframerates, &numframerates, _framerates, &bMaxMin);

		if (_err == EOK) {
			for (i=numframerates-1; i>=0; i--) {
				variantMap = new QVariantMap();

				QString framerate = QString::number(_framerates[i]);
				framerate.append("  fps");

				(*variantMap)[QString("label")] = QVariant(framerate);
				(*variantMap)[QString("value")] = QVariant(_framerates[i]);
				qDebug() << "available frameates: " << framerate << "\n";

				*list << *variantMap;
			}

		} else {
			qDebug() << "error retrieving available framerates: " << strerror(_err) << "\n";
		}

	} else {
		qDebug() << "error retrieving available framerates: " << strerror(_err) << "\n";
	}

	qDebug() << "available framerates: " << list->count() << "\n";

	return *list;
}

void ScreenCapture::setViewfinderResolution(int selectedIndex)
{
	_vfWidth = _resolutions[selectedIndex].width;
	_vfHeight = _resolutions[selectedIndex].height;

	qDebug() << "view finder resolution: " << _vfWidth << " x " << _vfHeight << "\n";
}

void ScreenCapture::setViewfinderFrameRate(int frameRate)
{
	_vfFrameRate = (double)frameRate;
}

void ScreenCapture::setViewfinderRotation(int angle)
{
	_vfRotationAngle = angle;
}

void ScreenCapture::setViewfinderFrameType(int selectedIndex)
{
	if (selectedIndex == 1) {
		_vfFrameType = CAMERA_FRAMETYPE_NV12;
		_RGBviewfinder = false;
	} else if (selectedIndex == 2) {
		_vfFrameType = CAMERA_FRAMETYPE_RGB8888;
		_RGBviewfinder = true;
	} else {
		_vfFrameType = CAMERA_FRAMETYPE_NV12;
		qDebug() << "Unsupported option selected - choosing NV12\n";
	}
}

void ScreenCapture::startViewfinder()
{
	if (_configured) {
		_err = camera_set_videovf_property(_handle,
										  CAMERA_IMGPROP_FORMAT, _vfFrameType,
										  CAMERA_IMGPROP_WIDTH, _vfWidth,
										  CAMERA_IMGPROP_HEIGHT, _vfHeight,
										  CAMERA_IMGPROP_ROTATION, _vfRotationAngle,
										  CAMERA_IMGPROP_FRAMERATE, _vfFrameRate);

		if (_err != EOK) {
			qDebug() << "error configuring viewfinder: " << strerror(_err) << "\n";
		}

		// this should be off for screen capture
		_videoStabilization = 0;

		_err = camera_set_video_property(_handle,
				CAMERA_IMGPROP_WIDTH, _vfWidth,
				CAMERA_IMGPROP_HEIGHT, _vfHeight,
				CAMERA_IMGPROP_STABILIZATION, _videoStabilization);

		if (_err != EOK) {
			qDebug() << "error configuring video stabilization\n";
		}

		// try to start viewfinder
		_err = camera_start_video_viewfinder(_handle,
											&vf_callback,
											&status_callback,
											this);
		if (_err != EOK) {
			qDebug() << "error starting viewfinder: " << strerror(_err) << "\n";
		}

		_doMovie = true;

		qDebug() << "Started video viewfinder successfully\n";
	}
}

void ScreenCapture::setVideoFrameRate(int frameRate)
{
	_frameRate = (double)frameRate;
}

void ScreenCapture::setConfigured(bool configured)
{
	_configured = configured;
}

void ScreenCapture::startVideo()
{
	if (!_recordingActive && _configured) {
		// video recording!
		_err = camera_set_video_property(_handle,
				CAMERA_IMGPROP_FRAMERATE, _frameRate);


        _err = camera_roll_open_video(_handle, &_fd, _filename, CAMERA_ROLL_NAMELEN, CAMERA_ROLL_VIDEO_FMT_MP4);
        if (_err == EOK) {
            qDebug() << "Created video file " << _filename << "!\n";
        } else {
            qDebug() << "Error creating video file: " << strerror(_err) << "\n";
        }
        _videoroll = true;

		_frameCount = 0;
		_err = camera_start_video(_handle,
								 _filename,
								 NULL,
								 //&video_callback,
								 &video_status_callback,
								 (void*)this);
		if (_err != EOK) {
			qDebug() << "Error starting video recording: " << _err << " : " << strerror(_err) << "\n";
		} else {
			qDebug() << "recording video on file " << _filename << "!\n";

			_recordingActive = true;
		}
	}
}

void ScreenCapture::stopVideo()
{
	if (_recordingActive && _configured) {
		_err = camera_stop_video(_handle);
		if (_err != EOK) {
			qDebug() << "Error stopping video recording: " <<  strerror(_err) << "\n";
		} else {
			qDebug() << "stopped video\n";
		}

		_recordingActive = false;
	}
	if (_videoroll && _configured) {
		qDebug() << "Closing video file " << _filename << "!\n";
		camera_roll_close_video(_fd);
	}
}

void ScreenCapture::startEncode()
{
	if (_encodingActive == false) {
		_err = camera_set_video_property(_handle,
				CAMERA_IMGPROP_FRAMERATE, _frameRate);

		_frameCount = 0;
		_err = camera_start_encode(_handle,
								 &video_callback,
								 &videoencode_callback,
								 &audioencode_callback,
								 &video_status_callback,
								 (void*)this);
		if (_err != EOK) {
			qDebug() << "Error starting video encoding: " << _err << " : " << strerror(_err) << "\n";
		} else {
			qDebug() << "encoding video!";

			_encodingActive = true;
		}
	}
}

void ScreenCapture::stopEncode()
{
	if (_encodingActive == true) {
		_err = camera_stop_encode(_handle);
		if (_err != EOK) {
			qDebug() << "Error stopping video encoding: " <<  strerror(_err) << "\n";
		}

		qDebug() << "stopped video encoding\n";

		_encodingActive = false;
	}
}

void ScreenCapture::cleanup()
{
	sleep(2);

	qDebug() << "all done, shutting down...\n";

	_err = CAMERA_EOK;
    if (_doMovie) {
    	_err = camera_stop_video_viewfinder(_handle);

		if (_err != EOK) {
			qDebug() << "error stopping viewfinder: " << strerror(_err) << "\n";
		} else {
			qDebug() << "stopped viewfinder\n";
			_doMovie = false;
		}
    }

	if (_cameraOpen) {
		camera_close(_handle);

		_cameraOpen = false;

		qDebug() << "closed camera: " << _handle << "\n";
	}
}


