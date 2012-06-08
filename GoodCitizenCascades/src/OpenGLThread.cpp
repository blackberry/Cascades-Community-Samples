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

#include "OpenGLThread.hpp"
#include <math.h>

#include <QDebug>

using namespace bb::cascades;

#define MAX_NUMBER_VIEWS 1000

OpenGLThread::OpenGLThread()
{
	    m_isRunning = true;

	    m_number_views = 0;
		m_views = new OpenGLView* [MAX_NUMBER_VIEWS];

		qDebug()  << "OpenGLThread::OpenGLThread: "<< m_isRunning;
}

OpenGLThread::~OpenGLThread() {
	// TODO Auto-generated destructor stub
}

void OpenGLThread::addView(OpenGLView *view) {
	if (view != NULL) {
		m_views[m_number_views++] = view;
	}
}

void OpenGLThread::removeView(OpenGLView *view) {
	if (view != NULL) {
		m_views[--m_number_views] = NULL;
	}
}

int OpenGLThread::initBPS() {
	//Initialize BPS library
	bps_initialize();

	//Initialize application logic
	if (EXIT_SUCCESS != initGL()) {
		fprintf(stderr, "initialize failed\n");
		cleanupEGL();
		return EXIT_FAILURE;
	}

	//Signal BPS library that navigator and screen events will be requested
	if (BPS_SUCCESS != screen_request_events(m_screen_cxt)) {
		fprintf(stderr, "screen_request_events failed\n");
		cleanupEGL();
		return EXIT_FAILURE;
	}

	if (BPS_SUCCESS != navigator_request_events(0)) {
		fprintf(stderr, "navigator_request_events failed\n");
		cleanupEGL();
		return EXIT_FAILURE;
	}

	//Signal BPS library that navigator orientation is not to be locked
	if (BPS_SUCCESS != navigator_rotation_lock(false)) {
		fprintf(stderr, "navigator_rotation_lock failed\n");
		cleanupEGL();
		return EXIT_FAILURE;
	}


	return EXIT_SUCCESS;
}


int OpenGLThread::initEGL(enum RENDERING_API api) {
    int rc, num_configs;
    EGLint attributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

    EGLint attrib_list[]= { EGL_RED_SIZE,        8,
                            EGL_GREEN_SIZE,      8,
                            EGL_BLUE_SIZE,       8,
                            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
                            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
                            EGL_NONE};

    if (api == GL_ES_1) {
        m_usage = SCREEN_USAGE_OPENGL_ES1 | SCREEN_USAGE_ROTATION;
    } else if (api == GL_ES_2) {
    	attrib_list[9] = EGL_OPENGL_ES2_BIT;
    	m_usage = SCREEN_USAGE_OPENGL_ES2 | SCREEN_USAGE_ROTATION;
    } else if (api == VG) {
    	attrib_list[9] = EGL_OPENVG_BIT;
    	m_usage = SCREEN_USAGE_OPENVG | SCREEN_USAGE_ROTATION;
    } else {
        fprintf(stderr, "invalid api setting\n");
        return EXIT_FAILURE;
    }

    m_egl_disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_egl_disp == EGL_NO_DISPLAY) {
        eglPrintError("eglGetDisplay");
        //cleanupEGL();
        return EXIT_FAILURE;
    }

    rc = eglInitialize(m_egl_disp, NULL, NULL);
    if (rc != EGL_TRUE) {
        eglPrintError("eglInitialize");
        //cleanupEGL();
        return EXIT_FAILURE;
    }

    if ((api == GL_ES_1) || (api == GL_ES_2)) {
        rc = eglBindAPI(EGL_OPENGL_ES_API);
    } else if (api == VG) {
        rc = eglBindAPI(EGL_OPENVG_API);
    }

    if (rc != EGL_TRUE) {
        eglPrintError("eglBindApi");
        //cleanupEGL();
        return EXIT_FAILURE;
    }

    if(!eglChooseConfig(m_egl_disp, attrib_list, &m_egl_conf, 1, &num_configs)) {
        //cleanupEGL();
        return EXIT_FAILURE;
    }

    if (api == GL_ES_2) {
        m_egl_ctx = eglCreateContext(m_egl_disp, m_egl_conf, EGL_NO_CONTEXT, attributes);
    } else {
        m_egl_ctx = eglCreateContext(m_egl_disp, m_egl_conf, EGL_NO_CONTEXT, NULL);
    }

	qDebug()  << "OpenGLThread::initEGL: "<< m_egl_disp << ":" << m_egl_conf << ":" << m_egl_ctx << ":" << num_configs;

    if (m_egl_ctx == EGL_NO_CONTEXT) {
        eglPrintError("eglCreateContext");
        //cleanupEGL();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void
OpenGLThread::cleanupEGL() {
    //Typical EGL cleanup
	//bbutil_terminate();

    if (m_egl_disp != EGL_NO_DISPLAY) {
		if (m_egl_ctx != EGL_NO_CONTEXT) {
			eglDestroyContext(m_egl_disp, m_egl_ctx);
			m_egl_ctx = EGL_NO_CONTEXT;
		}

        eglTerminate(m_egl_disp);
        m_egl_disp = EGL_NO_DISPLAY;
    }

    eglReleaseThread();

    m_initialized = false;
}

int OpenGLThread::initGL() {
	int index = 0;
	int returnCode = EXIT_FAILURE;

	m_initialized = false;

	//Create a screen context that will be used to create an EGL surface to to receive libscreen events
	screen_create_context(&m_screen_cxt, 0);

	//Use utility code to initialize EGL for rendering with GL ES 1.1

	if (EXIT_SUCCESS != initEGL(GL_ES_1)) {
	//if (EXIT_SUCCESS != bbutil_init_egl(m_screen_cxt, GL_ES_1)) {
		fprintf(stderr, "initEGL failed\n");
		return EXIT_FAILURE;
	}

	qDebug()  << "OpenGLThread::initGL: "<< m_egl_disp << ":" << m_egl_conf << ":" << m_egl_ctx << ":" << m_screen_cxt << ":" << m_usage;

	// initialize each view
	for(index = 0; index < m_number_views; index++) {
		returnCode = m_views[index]->initialize(m_egl_ctx, m_egl_conf, m_egl_disp, m_screen_cxt, m_usage);
		if (returnCode == EXIT_FAILURE) {
			break;
		}
	}

	if (returnCode != EXIT_FAILURE) {
		m_initialized = true;
	}

    return returnCode;
}


void OpenGLThread::handleScreenEvent(bps_event_t *event) {
    int screenEvent;
    int buttons;
    int position[2];

    //static bool mouse_pressed = false;

    screen_event_t screen_event = screen_event_get_event(event);

    //Query type of screen event and its location on the screen
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE,
            &screenEvent);
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_SOURCE_POSITION,
    		position);

	switch (screenEvent) {
		case SCREEN_EVENT_MTOUCH_TOUCH:
			break;

		case SCREEN_EVENT_MTOUCH_MOVE:
			break;

		case SCREEN_EVENT_MTOUCH_RELEASE:
			break;

		case SCREEN_EVENT_POINTER:
	        screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_BUTTONS,
				&buttons);

	    	switch (buttons) {
	    		case SCREEN_LEFT_MOUSE_BUTTON:
	    		case SCREEN_RIGHT_MOUSE_BUTTON:
	                //handleClick(position[0], position[1]);
	    			break;
	    	}

			break;
	}
}

void OpenGLThread::handleNavigatorEvent(bps_event_t *event) {
    //int rc;
    //bps_event_t *activation_event = NULL;

    switch (bps_event_get_code(event)) {
    case NAVIGATOR_ORIENTATION_CHECK:
        //Signal navigator that we intend to resize
        navigator_orientation_check_response(event, true);
        break;
    case NAVIGATOR_ORIENTATION:
        if (EXIT_FAILURE == resize(event)) {
            m_isRunning = true;
        }
        break;
    case NAVIGATOR_SWIPE_DOWN:
    	// initiate app menu animation
        break;

    case NAVIGATOR_EXIT:
    	// exit the thread
		m_isRunning = false;
        break;

    case NAVIGATOR_WINDOW_ACTIVE:
    	// reactivate processing when window becomes active
        break;

    case NAVIGATOR_WINDOW_INACTIVE:
        // wait for NAVIGATOR_WINDOW_ACTIVE event
        break;
    }
}

int OpenGLThread::resize(bps_event_t *event) {
    //Query width and height of the window surface created by utility code

    if (event) {
        int angle = navigator_event_get_orientation_angle(event);
    	int index = 0;

        EGLint err = eglGetError();
        if (err != 0x3000) {
            fprintf(stderr, "Unable to query EGL surface dimensions\n");
            return EXIT_FAILURE;
        }

    	// rotate each view
    	for(index = 0; index < m_number_views; index++) {
            // rotate screen surfaces to this angle
            if (EXIT_FAILURE == m_views[index]->rotate(angle)) {
                fprintf(stderr, "Unable to handle orientation change\n");
                return EXIT_FAILURE;
    		}
    	}

    }


    update();

    if (event) {
        render();

        navigator_done_orientation(event);
    }

    return EXIT_SUCCESS;
}

void OpenGLThread::update() {
	int index = 0;

	// render each view
	for(index = 0; index < m_number_views; index++) {
		m_views[index]->update();
	}
}

void OpenGLThread::render() {
	int index = 0;

	// render each view
	for(index = 0; index < m_number_views; index++) {
		m_views[index]->render();
	}
}

void OpenGLThread::run() {
	int returnCode = initBPS();

	while (m_initialized && m_isRunning) {
		//Request and process BPS next available event
		bps_event_t *event = NULL;
		int rc = bps_get_event(&event, 0);
		assert(rc == BPS_SUCCESS);

		if (event) {
			int domain = bps_event_get_domain(event);

			if (domain == screen_get_domain()) {
				handleScreenEvent(event);
            } else if (domain == navigator_get_domain()) {
                handleNavigatorEvent(event);
			}

			if (!m_isRunning) {
				break;
			}
		}

		update();

		render();

		//msleep(5);
	}

	int index = 0;

	// cleanup and remove each view
	for(index = 0; index < m_number_views; index++) {
		m_views[index]->cleanup();
	}

	while(m_number_views > 0) {
		removeView(m_views[m_number_views-1]);
	}

	//Stop requesting events from libscreen
	screen_stop_events(m_screen_cxt);

	//Shut down BPS library for this process
	bps_shutdown();

	//Use utility code to terminate EGL setup
	cleanupEGL();

	//Destroy libscreen context
	screen_destroy_context(m_screen_cxt);
}


void OpenGLThread::eglPrintError(const char *msg) {
    static const char *errmsg[] = {
        "function succeeded",
        "EGL is not initialized, or could not be initialized, for the specified display",
        "cannot access a requested resource",
        "failed to allocate resources for the requested operation",
        "an unrecognized attribute or attribute value was passed in an attribute list",
        "an EGLConfig argument does not name a valid EGLConfig",
        "an EGLContext argument does not name a valid EGLContext",
        "the current surface of the calling thread is no longer valid",
        "an EGLDisplay argument does not name a valid EGLDisplay",
        "arguments are inconsistent",
        "an EGLNativePixmapType argument does not refer to a valid native pixmap",
        "an EGLNativeWindowType argument does not refer to a valid native window",
        "one or more argument values are invalid",
        "an EGLSurface argument does not name a valid surface configured for rendering",
        "a power management event has occurred",
    };

    fprintf(stderr, "%s: %s\n", msg, errmsg[eglGetError() - EGL_SUCCESS]);
}

