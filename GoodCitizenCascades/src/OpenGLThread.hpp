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

#include <bb/cascades/ForeignWindow>

#include <QtCore/QThread>
#include <QtCore/QList>
#include <QtCore/QString>

#include "OpenGLView.hpp"

//#include "bbutil.h"

enum RENDERING_API {GL_ES_1 = EGL_OPENGL_ES_BIT, GL_ES_2 = EGL_OPENGL_ES2_BIT, VG = EGL_OPENVG_BIT};

class OpenGLThread :public QThread {

Q_OBJECT

public:
	OpenGLThread();

	virtual ~OpenGLThread();

	void run();

	void addView(OpenGLView *view);
	void removeView(OpenGLView *view);

	static void eglPrintError(const char *msg);

private:
	int initBPS();
	int initGL();

	// EGL initialization / cleanup functions
	/**
	 * Initializes EGL
	 *
	 * @param libscreen context that will be used for EGL setup
	 * @param rendering API that will be used
	 * @return EXIT_SUCCESS if initialization succeeded otherwise EXIT_FAILURE
	 */
	int initEGL(enum RENDERING_API api);

	/**
	 * Terminates EGL
	 */
	void cleanupEGL();


	void handleScreenEvent(bps_event_t *event);
	void handleNavigatorEvent(bps_event_t *event);

	void update();
	void render();

	int resize(bps_event_t *event);

	// EGL members
	EGLDisplay m_egl_disp;
	EGLConfig m_egl_conf;
	EGLContext m_egl_ctx;
    int m_usage;


	// screens / windows
	screen_context_t m_screen_cxt;
	float screenWidth, screenHeight;

	//static font_t* font;

	int m_number_views;
	OpenGLView** m_views;

	bool m_initialized;
	bool m_isRunning;
};

#endif /* OPENGLTHREAD_HPP */

