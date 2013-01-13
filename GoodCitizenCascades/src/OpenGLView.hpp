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


#ifndef OPENGLVIEW_HPP
#define OPENGLVIEW_HPP

#include <stdlib.h>
#include <screen/screen.h>
#include <sys/platform.h>

#include <bps/navigator.h>
#include <bps/screen.h>
#include <bps/bps.h>
#include <bps/event.h>

#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES/glext.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "png.h"

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QVariant>

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct font_t {
	unsigned int font_texture;
	float pt;
	float advance[128];
	float width[128];
	float height[128];
	float tex_x1[128];
	float tex_x2[128];
	float tex_y1[128];
	float tex_y2[128];
	float offset_x[128];
	float offset_y[128];
	int initialized;
} font_t;

#ifdef __cplusplus
}
#endif

typedef enum RENDERING_API {GL_UNKNOWN = 0, GL_ES_1 = EGL_OPENGL_ES_BIT, GL_ES_2 = EGL_OPENGL_ES2_BIT, VG = EGL_OPENVG_BIT} RENDERING_API;

typedef enum VIEW_DISPLAY {DISPLAY_UNKNOWN, DISPLAY_DEVICE, DISPLAY_HDMI} VIEW_DISPLAY;


class OpenGLView : public QObject {

	Q_OBJECT

	Q_PROPERTY(bool enabled READ enabled WRITE setEnabled)

	friend class OpenGLThread;

public:
	OpenGLView();

	virtual ~OpenGLView();

	// must be defined in derived classes
	virtual int initialize() = 0;

	// display methods
	void setDisplay(VIEW_DISPLAY display);

	// handle screen events when view is an overlay ie. above Cascades layer
	virtual void handleScreenEvent(bps_event_t *event) = 0;

	// state functions
	// enable is used to enable / disable rendering
	bool enabled();
	// flags initialization of the view
	bool initialized();

	// stale flag indicates when the view needs to be rendered again
	bool stale();

	// this flag signals that the view was altered (rotated or resized) and needs to be regenerated
	bool altered();

	// custom update handling to be defined in derived class
	virtual void update() = 0;

	// main render function - obtains GL context and swaps buffer so that the derived class doesn't need to handle it
	void renderView();

	// custom rendering to be defined in base class
	virtual void render() = 0;

	// view settings
	void setAngle(int angle);
	void setPosition(int x, int y);
	void setSize(int width, int height);
	void setZ(int z);
	void setTransparency(int transparency);

	void setWindowGroup(const QString &group);
	void setWindowID(const QString id);

	// called by the thread to regenerate the surface
	virtual int regenerate();

	// custom cleanup to be implemented by derived class
	virtual void cleanup() = 0;

	// methods to add or remove this view - hide thread functions from caller
	void add();
	void remove();

	// method to set GL API
	static void setRenderingAPI(RENDERING_API api);

	// shutdown - call thread function, hiding it from caller
	static void shutdown();


public Q_SLOTS:
	// state slots
	void setEnabled(bool enabled);
	void setInitialized(bool initialized);
	void setStale(bool stale);
	void setAltered(bool stale);

protected:
	// init GL - must be called in derived initialize function
	int initGL();

	bool visible();
	void setVisible(bool visible);


	// display methods
	EGLDisplay display();

	// API / screen property calls
	void setAPI(RENDERING_API api);
	void setScreenContext(screen_context_t screen_ctx);
	void setScreenEGLConfiguration(EGLConfig egl_conf);
	void setScreenEGLDisplay(EGLDisplay egl_disp);

	// window property calls
	int setWindowAngle(int angle);
	int setWindowPosition(int x, int y);
	int setWindowSize(int width, int height);
	int setWindowZ(int z);
	int setWindowSourceSize(int width, int height);
	int setWindowBufferSize(int width, int height);
	int setWindowTransparency(int transparency);
	int setWindowUsage(int usage);

	// window group / ID calls
	int joinWindowGroup(const QString &group);
	int setScreenWindowID(const QString id);


	// OpenGL utility functions for OpenGL views - derived from bb_util.c

	/**
	 * Swaps default window surface to the screen
	 */
	void swapBuffers();

	/*
	 * makes this view's EGL context curent
	 */
	void getGLContext();

	/*
	 * releases the view's EGL context from being current
	 */
	void  releaseGLContext();

	/**
	 * Loads the font from the specified font file.
	 * NOTE: should be called after a successful return from bbutil_init() or bbutil_init_egl() call
	 * @param font_file string indicating the absolute path of the font file
	 * @param point_size used for glyph generation
	 * @param dpi used for glyph generation
	 * @return pointer to font_t structure on success or NULL on failure
	 */
	font_t* loadFont(const char* font_file, int point_size, int dpi);

	/**
	 * Destroys the passed font
	 * @param font to be destroyed
	 */
	void destroyFont(font_t* font);

	/**
	 * Renders the specified message using current font starting from the specified
	 * bottom left coordinates.
	 * NOTE: must be called after a successful return from bbutil_init() or bbutil_init_egl() call

	 *
	 * @param font to use for rendering
	 * @param msg the message to display
	 * @param x, y position of the bottom-left corner of text string in world coordinate space
	 * @param rgba color for the text to render with
	 */
	void renderText(font_t* font, const char* msg, float x, float y, float r, float g, float b, float a);

	/**
	 * Returns the non-scaled width and height of a string
	 * NOTE: must be called after a successful return from bbutil_init() or bbutil_init_egl() call

	 *
	 * @param font to use for measurement of a string size
	 * @param msg the message to get the size of
	 * @param return pointer for width of a string
	 * @param return pointer for height of a string
	 */
	void measureText(font_t* font, const  char* msg, float* width, float* height);

	/**
	 * Creates and loads a texture from a png file
	 * NOTE: must be called after a successful return from bbutil_init() or bbutil_init_egl() call

	 *
	 * @param filename path to texture png
	 * @param return width of texture
	 * @param return height of texture
	 * @param return gl texture handle
	 * @return EXIT_SUCCESS if texture loading succeeded otherwise EXIT_FAILURE
	 */

	int loadTexture(const char* filename, int* width, int* height, float* tex_x, float* tex_y, unsigned int* tex);

	/**
	 * Returns dpi for current screen
	 *
	 * @return dpi for current screen
	 */

	int calculateDPI();

protected:
	// state
	bool m_enabled;
	bool m_initialized;
	bool m_altered;
	bool m_visible;
	bool m_stale;
	VIEW_DISPLAY m_display;

	// EGL parameters
	EGLContext m_egl_ctx;
	EGLConfig m_egl_conf;
	EGLDisplay m_egl_disp;
	EGLSurface m_egl_surf;
	int m_usage;
	int m_nbuffers;

	EGLint m_surface_width;
	EGLint m_surface_height;

    RENDERING_API m_api;

	// screens / windows
	screen_context_t m_screen_ctx;
	screen_window_t m_screen_win;
	screen_display_t m_screen_disp;
	screen_display_t *m_screen_dpy;
	screen_display_mode_t *m_screen_modes;

	// view properties
	int m_angle;
	int m_x;
	int m_y;
	int m_z;
	int m_width;
	int m_height;
	int m_interval;
	int m_transparency;

	// window group / ID
	QString m_group;
	QString m_id;

	// main mutex for controlling view access
	QMutex m_viewMutex;

private:
	int nextp2(int x);

	// mutex for controlling render access across all views
	static QMutex m_renderMutex;
};

#endif /* OPENGLVIEW_HPP */

