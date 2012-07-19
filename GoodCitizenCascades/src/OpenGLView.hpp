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
#include <QtCore/QVariant>

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

class OpenGLView : public QObject {

	Q_OBJECT

public:
	OpenGLView();

	virtual ~OpenGLView();

	virtual int initialize(EGLContext egl_ctx, EGLConfig egl_conf, EGLDisplay egl_disp, screen_context_t screen_cxt, int  usage) = 0;
	//virtual void handleScreenEvent(bps_event_t *event);
	//virtual void handleNavigatorEvent(bps_event_t *event);
	virtual void update() = 0;
	virtual void render() = 0;
	virtual int rotate(int angle);
	virtual void cleanup() = 0;

protected:
	int initGL(EGLContext egl_ctx, EGLConfig egl_conf, EGLDisplay egl_disp, screen_context_t screen_cxt, int  usage, int z, const char* group, const char* id);

	// OpenGL utility functions for OpenGL views

	/**
	 * Swaps default window surface to the screen
	 */
	void swapBuffers();

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

	/**
	 * Rotates the screen to a given angle

	 *
	 * @param angle to rotate screen surface to, must by 0, 90, 180, or 270
	 * @return EXIT_SUCCESS if texture loading succeeded otherwise EXIT_FAILURE
	 */

	int rotateScreenSurface(int angle);

private:
	int nextp2(int x);

protected:
	EGLContext m_egl_ctx;
	EGLConfig m_egl_conf;
	EGLDisplay m_egl_disp;
	EGLSurface m_egl_surf;

	// screens / windows
	screen_context_t m_screen_cxt;
	screen_window_t m_screen_win;
	screen_display_t m_screen_disp;
	int m_nbuffers;
	EGLint m_surface_width;
	EGLint m_surface_height;

};

#endif /* OPENGLVIEW_HPP */

