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

OpenGLThread OpenGLThread::singleton;

OpenGLThread::OpenGLThread()
{
    m_initialized = false;
    m_egl_initialized = false;
    m_stopped = false;
    m_api = GL_UNKNOWN;
    m_numberDisplays = 0;
    m_screen_dpy = NULL;
}

OpenGLThread::~OpenGLThread() {
}

bool OpenGLThread::renderingAPI()
{
	RENDERING_API api = GL_UNKNOWN;

	m_threadMutex.lock();

	api = m_api;

	m_threadMutex.unlock();

	return api;
}

void OpenGLThread::setRenderingAPI(RENDERING_API api)
{
	m_threadMutex.lock();

	m_api = api;

	m_threadMutex.unlock();
}

bool OpenGLThread::initialized()
{
	bool initialized = false;

	m_threadMutex.lock();

	initialized = m_initialized;

	m_threadMutex.unlock();

	return initialized;
}

void OpenGLThread::setInitialized(bool initialized)
{
	m_threadMutex.lock();

	m_initialized = initialized;

	m_threadMutex.unlock();
}

bool OpenGLThread::eglInitialized()
{
	bool initialized = false;

	m_threadMutex.lock();

	initialized = m_egl_initialized;

	m_threadMutex.unlock();

	return initialized;
}

void OpenGLThread::setEGLInitialized(bool initialized)
{
	m_threadMutex.lock();

	m_egl_initialized = initialized;

	m_threadMutex.unlock();
}

bool OpenGLThread::stopped()
{
	bool stopped = false;

	m_threadMutex.lock();

	stopped = m_stopped;

	m_threadMutex.unlock();

	return stopped;
}

void OpenGLThread::setStopped(bool stopped)
{
	m_threadMutex.lock();

	m_stopped = stopped;

	m_threadMutex.unlock();
}


void OpenGLThread::addView(OpenGLView *view) {
	if (view != NULL) {
		m_viewsMutex.lock();

		m_views.append(view);

		m_viewsMutex.unlock();

		while (!view->initialized() && view->visible()) {
			usleep(1);
		}
	}
}

void OpenGLThread::removeView(OpenGLView *view) {
	if (view != NULL) {
		m_viewsMutex.lock();
		int index = m_views.indexOf(view);

		if ( index != -1 ) {
			m_views.remove(index);
		}

		m_viewsMutex.unlock();

		view->cleanup();
	}
}

int OpenGLThread::initBPS() {
	//Initialize BPS library
	bps_initialize();

	//Create a screen context that will be used to create an EGL surface to to receive libscreen events
	if (EXIT_SUCCESS != screen_create_context(&m_screen_ctx, 0)) {
		fprintf(stderr, "screen_request_events failed\n");
		return EXIT_FAILURE;
	}

	// wait for rendering API to be set
	while (renderingAPI() == GL_UNKNOWN) {
		usleep(10);
	};

	// initialize EGL
	if (EXIT_SUCCESS != initEGL()) {
		fprintf(stderr, "initialize EGL failed\n");
		return EXIT_FAILURE;
	}

	//Signal BPS library that screen events will be requested
	if (BPS_SUCCESS != screen_request_events(m_screen_ctx)) {
		fprintf(stderr, "screen_request_events failed\n");
		return EXIT_FAILURE;
	}

	setInitialized(true);

	return EXIT_SUCCESS;
}

int OpenGLThread::initEGL() {
    int returnCode, type;
    int num_configs;

    m_numberDisplays = 0;
    EGLint attrib_list[]= { EGL_RED_SIZE,        8,
                            EGL_GREEN_SIZE,      8,
                            EGL_BLUE_SIZE,       8,
                            EGL_DEPTH_SIZE, 	 16,
                            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
                            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
                            EGL_NONE};

	screen_get_context_property_iv(m_screen_ctx, SCREEN_PROPERTY_DISPLAY_COUNT, &m_numberDisplays);

	m_screen_dpy = (screen_display_t *)calloc(m_numberDisplays, sizeof(screen_display_t));
	screen_get_context_property_pv(m_screen_ctx, SCREEN_PROPERTY_DISPLAYS, (void **)m_screen_dpy);

    m_egl_disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_egl_disp == EGL_NO_DISPLAY) {
        eglPrintError("eglGetDisplay");
        return EXIT_FAILURE;
    }

    returnCode = eglInitialize(m_egl_disp, NULL, NULL);
    if (returnCode != EGL_TRUE) {
        eglPrintError("eglInitialize");
        return EXIT_FAILURE;
    }

    if ((m_api == GL_ES_1) || (m_api == GL_ES_2)) {
        returnCode = eglBindAPI(EGL_OPENGL_ES_API);
    } else if (m_api == VG) {
        returnCode = eglBindAPI(EGL_OPENVG_API);
    }

    if (returnCode != EGL_TRUE) {
        eglPrintError("eglBindApi");
        return EXIT_FAILURE;
    }

    if(!eglChooseConfig(m_egl_disp, attrib_list, &m_egl_conf, 1, &num_configs)) {
        perror("eglChooseConfig");
        return EXIT_FAILURE;
    }

	setEGLInitialized(true);

    return EXIT_SUCCESS;
}

void
OpenGLThread::cleanupEGL() {
    //Typical EGL cleanup

    if (m_egl_disp != EGL_NO_DISPLAY) {
        eglTerminate(m_egl_disp);
        m_egl_disp = EGL_NO_DISPLAY;
    }

    if (m_egl_disp_hdmi != EGL_NO_DISPLAY) {
        eglTerminate(m_egl_disp_hdmi);
        m_egl_disp_hdmi = EGL_NO_DISPLAY;
    }

    eglReleaseThread();

    m_initialized = false;
}


void OpenGLThread::update() {
	int index, type;
	int viewCount = 0;
	int returnCode = 0;

	m_viewsMutex.lock();

	viewCount = m_views.size();

	m_viewsMutex.unlock();

	if (viewCount > 0) {
		m_viewsMutex.lock();

		int attached = 0;
		for (index = 0; index < m_numberDisplays; index++) {
	        screen_get_display_property_iv(m_screen_dpy[index], SCREEN_PROPERTY_TYPE,  &type);
			attached = 0;

	        if (type == SCREEN_DISPLAY_TYPE_HDMI) {
				screen_get_display_property_iv(m_screen_dpy[index], SCREEN_PROPERTY_ATTACHED, &attached);

				if (attached) {
					int size[2];
					screen_get_display_property_iv(m_screen_dpy[index], SCREEN_PROPERTY_SIZE, size);
					if (size[0] == 0 || size[1] == 0) {
						attached = 0;
					}
				}
	        }
		}

		for(index = 0; index < m_views.size(); index++) {
			if (m_views.at(index)->display() == m_egl_disp_hdmi) {
				if (attached) {
					m_views.at(index)->setVisible(true);
				} else {
					m_views.at(index)->setVisible(false);
				}
			} else {
				m_views.at(index)->setVisible(true);
			}
		}

		for (int index = 0; index < m_views.size(); index++) {
			if (!m_views.at(index)->initialized() && m_views.at(index)->visible()) {

				m_views.at(index)->setAPI(m_api);
				m_views.at(index)->setScreenContext(m_screen_ctx);
				m_views.at(index)->setScreenEGLConfiguration(m_egl_conf);
				m_views.at(index)->setScreenEGLDisplay(m_egl_disp);
				returnCode = m_views.at(index)->initialize();
				if (returnCode == EXIT_FAILURE) {

					qDebug()  << "OpenGLThread::update: view initialization failed: " << index;
				}
			}

			if (m_views.at(index)->altered() && m_views.at(index)->visible()) {
				returnCode = m_views.at(index)->regenerate();
				if (returnCode == EXIT_FAILURE) {

					qDebug()  << "OpenGLThread::update: view regenerate failed: " << index;
				}
			}

			if (m_views.at(index)->initialized() && m_views.at(index)->enabled() && m_views.at(index)->visible()) {
				m_views.at(index)->update();
			}
		}

		m_viewsMutex.unlock();
	}
}

void OpenGLThread::cleanup() {
	int viewCount = 0;

	m_viewsMutex.lock();

	viewCount = m_views.size();

	m_viewsMutex.unlock();

	if (viewCount > 0) {
		do {
			removeView(m_views.at(m_views.size()-1));

			m_viewsMutex.lock();

			viewCount = m_views.size();

			m_viewsMutex.unlock();

		} while (viewCount > 0);
	}

	qDebug()  << "OpenGLThread::cleanup";
}

void OpenGLThread::render() {
	int index, type;
	int viewCount = 0;

	m_viewsMutex.lock();

	viewCount = m_views.size();

	m_viewsMutex.unlock();

	if (viewCount > 0) {
		m_viewsMutex.lock();

		int attached = 0;
		for (index = 0; index < m_numberDisplays; index++) {
	        screen_get_display_property_iv(m_screen_dpy[index], SCREEN_PROPERTY_TYPE,  &type);
			int attached = 0;

	        if (type == SCREEN_DISPLAY_TYPE_HDMI) {
				screen_get_display_property_iv(m_screen_dpy[index], SCREEN_PROPERTY_ATTACHED, &attached);

				if (attached) {
					int size[2];
					screen_get_display_property_iv(m_screen_dpy[index], SCREEN_PROPERTY_SIZE, size);
					if (size[0] == 0 || size[1] == 0) {
						attached = 0;
					}
				}
	        }
		}

		for(index = 0; index < m_views.size(); index++) {
			if (m_views.at(index)->display() == m_egl_disp_hdmi) {
				if (attached) {
					m_views.at(index)->setVisible(true);
				} else {
					m_views.at(index)->setVisible(false);
				}
			} else {
				m_views.at(index)->setVisible(true);
			}
		}

		for (int index = 0; index < m_views.size(); index++) {
			if (m_views.at(index)->initialized() && m_views.at(index)->enabled() && m_views.at(index)->visible() && m_views.at(index)->stale()) {
				m_views.at(index)->renderView();
			}
		}

		m_viewsMutex.unlock();
	}
}


void OpenGLThread::handleScreenEvent(bps_event_t *event) {
    int buttons;
    int position[2];
    int index;
    screen_display_t event_disp;
    int type;

    screen_event_t screen_event = screen_event_get_event(event);

    //Query type of screen event and its location on the screen
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE,
            &type);
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_SOURCE_POSITION,
    		position);

	switch (type) {
		case SCREEN_EVENT_DISPLAY:
			screen_get_event_property_pv(screen_event, SCREEN_PROPERTY_DISPLAY, (void **)&event_disp);
			for (index = 0; index < m_numberDisplays; index++) {
				if (event_disp == m_screen_dpy[index]) {
					int attached = 0;
					screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_ATTACHED, &attached);

					if (attached) {
						int size[2];
						screen_get_display_property_iv(event_disp, SCREEN_PROPERTY_SIZE, size);
						if (size[0] == 0 || size[1] == 0) {
							attached = 0;
						}
					}


					int viewCount = 0;

					m_viewsMutex.lock();

					viewCount = m_views.size();

					m_viewsMutex.unlock();

					if (viewCount > 0) {
						m_viewsMutex.lock();

						for(index = 0; index < m_views.size(); index++) {
							if (m_views.at(index)->display() == m_egl_disp_hdmi && index == 1) {
								if (attached) {
									m_views.at(index)->setVisible(true);
								} else {
									m_views.at(index)->setVisible(false);
								}
							}
						}

						m_viewsMutex.unlock();
					}
				}
			}
			break;
	}
}

void OpenGLThread::run()
{
	int index = 0;

	setStopped(false);

	qDebug()  << "OpenGLThread started: " << !stopped();

	int returnCode = initBPS();

	if (returnCode == EXIT_SUCCESS && initialized() && eglInitialized()) {
		bps_event_t *event = NULL;

		while (!stopped()) {
			do {
				//Request and process BPS next available event
				event = NULL;
				returnCode = bps_get_event(&event, 0);
				//assert(rc == BPS_SUCCESS);

				if (event) {
					int domain = bps_event_get_domain(event);

					if (domain == screen_get_domain()) {
						handleScreenEvent(event);

						int viewCount = 0;

						m_viewsMutex.lock();

						viewCount = m_views.size();

						m_viewsMutex.unlock();

						if (viewCount > 0) {
							m_viewsMutex.lock();

							for(index = 0; index < m_views.size(); index++) {
								m_views.at(index)->handleScreenEvent(event);
							}

							m_viewsMutex.unlock();
						}
					}
				}
			} while (event);

			if (stopped()) {
				break;
			}

			//qDebug()  << "OpenGLThread stopped: " << stopped();

			update();

			render();

			usleep(5);
		}

		// remove and cleanup each view
		cleanup();
	}

	//Stop requesting events from libscreen
	screen_stop_events(m_screen_ctx);

	//Shut down BPS library for this process
	bps_shutdown();

	setInitialized(false);

	//Use utility code to terminate EGL setup
	cleanupEGL();

	//Destroy libscreen context
	screen_destroy_context(m_screen_ctx);
}

void OpenGLThread::shutdown()
{
	setStopped(true);
}

EGLDisplay OpenGLThread::getDisplay(VIEW_DISPLAY display)
{
	EGLDisplay egl_display = EGL_NO_DISPLAY;

	while (!eglInitialized()) {
		usleep(1);
	};

	switch (display) {
		case DISPLAY_DEVICE:
			egl_display = m_egl_disp;
		break;

		case DISPLAY_HDMI:
			egl_display = m_egl_disp_hdmi;
		break;
	}

	return egl_display;
}

bool OpenGLThread::isDisplayAttached(VIEW_DISPLAY display)
{
	EGLDisplay egl_display = EGL_NO_DISPLAY;

	while (!eglInitialized()) {
		usleep(1);
	};

	switch (display) {
		case DISPLAY_DEVICE:
			egl_display = m_egl_disp;
		break;

		case DISPLAY_HDMI:
			egl_display = m_egl_disp_hdmi;
		break;
	}

	return egl_display;
}

OpenGLThread* OpenGLThread::getInstance()
{
	if (!singleton.isRunning() && !singleton.stopped()) {
		singleton.start();
	}

	return &singleton;
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

