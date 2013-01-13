/*
 * Copyright (c) 2011-2012 Research In Motion Limited.
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

#ifndef OPENGLTHREAD_HPP
#define OPENGLTHREAD_HPP

#include <assert.h>
#include <screen/screen.h>
#include <bps/navigator.h>
#include <bps/screen.h>
#include <bps/bps.h>
#include <bps/event.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <bb/cascades/TouchEvent>

#include <QtCore/QThread>
#include <QtCore/QList>
#include <QtCore/QString>

#include "OpenGLView.hpp"

class OpenGLThread :public QThread {

Q_OBJECT

public:
	OpenGLThread();

	virtual ~OpenGLThread();

	void run();

	static OpenGLThread *getInstance();

	// signal the thread to shut down
	void shutdown();

	// add / remove views
	void addView(OpenGLView *view);
	void removeView(OpenGLView *view);

	// get the desired display
	EGLDisplay getDisplay(VIEW_DISPLAY display);
	bool isDisplayAttached(VIEW_DISPLAY display);

	// read / set the GL rendering API
	bool renderingAPI();
	void setRenderingAPI(RENDERING_API api);

	// control the initialized state
	bool initialized();
	void setInitialized(bool initialized);

	// control the EGL initialized state
	bool eglInitialized();
	void setEGLInitialized(bool initialized);

	// control the stopped state
	bool stopped();
	void setStopped(bool initialized);

	// handy print error function derived from bb_util.c
	static void eglPrintError(const char *msg);

private:
	// initialize / cleanup functions
	int initBPS();
	int initEGL();
	void cleanupEGL();
	void cleanup();


	void update();
	void render();


	// handle screen events - only need to handle display events in the thread
	virtual void handleScreenEvent(bps_event_t *event);

	// EGL members
	EGLDisplay m_egl_disp;
	EGLDisplay m_egl_disp_hdmi;
	EGLConfig m_egl_conf;
	EGLContext m_egl_ctx;
    int m_usage;

    RENDERING_API m_api;

    // display info
    int m_numberDisplays;
    screen_display_t *m_screen_dpy;


	// screens / windows
	screen_context_t m_screen_ctx;
	float screenWidth, screenHeight;

	QVector<OpenGLView*> m_views;

	// state fields
	bool m_initialized;
	bool m_egl_initialized;
	bool m_stopped;

	// mutexes to control thread access
	QMutex m_viewsMutex;
	QMutex m_threadMutex;

	static OpenGLThread singleton;
};

#endif /* OPENGLTHREAD_HPP */

