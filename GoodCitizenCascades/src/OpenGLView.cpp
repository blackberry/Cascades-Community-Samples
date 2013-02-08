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

#include "OpenGLView.hpp"
#include "OpenGLThread.hpp"

#include <QDebug>

QMutex OpenGLView::m_renderMutex;

void OpenGLView::setRenderingAPI(RENDERING_API api)
{
	OpenGLThread::getInstance()->setRenderingAPI(api);
}

OpenGLView::OpenGLView() : QObject(OpenGLThread::getInstance())
{
	// initialize members shared by derived classes
	m_nbuffers = 2;
	m_enabled = false;
	m_initialized = false;
	m_stale = false;
	m_altered = false;
	m_visible = false;

	m_screen_win = NULL;
	m_screen_dpy = NULL;

	m_angle = 0.0;
	m_x = 0;
	m_y = 0;
	m_width = 0;
	m_height = 0;
	m_interval = 0;
	m_transparency = 0;
}

OpenGLView::~OpenGLView() {
}

int OpenGLView::initGL()
{
    int numberDisplays;
    int numberModes;
    int returnCode;
    EGLBoolean status;
    int type;
    EGLint attributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    EGLint attrib_list[]= { EGL_RED_SIZE,        8,
                            EGL_GREEN_SIZE,      8,
                            EGL_BLUE_SIZE,       8,
                            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
                            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
                            EGL_NONE};

    // try this first as it will fail if an HDMI display is not attached
    if (m_api == GL_ES_2) {
        m_egl_ctx = eglCreateContext(m_egl_disp, m_egl_conf, EGL_NO_CONTEXT, attributes);
    } else {
        m_egl_ctx = eglCreateContext(m_egl_disp, m_egl_conf, EGL_NO_CONTEXT, NULL);
    }
    if (m_egl_ctx == EGL_NO_CONTEXT) {
        perror("eglCreateContext");
        return EXIT_FAILURE;
    }

	screen_get_context_property_iv(m_screen_ctx, SCREEN_PROPERTY_DISPLAY_COUNT, &numberDisplays);

	m_screen_dpy = (screen_display_t *)calloc(numberDisplays, sizeof(screen_display_t));
	screen_get_context_property_pv(m_screen_ctx, SCREEN_PROPERTY_DISPLAYS, (void **)m_screen_dpy);


	for (int index = 0; index < numberDisplays; index++) {
		int displayID;

        returnCode = screen_get_display_property_iv(m_screen_dpy[index], SCREEN_PROPERTY_ID,  (int *)&displayID);
    	if (returnCode) {
    		perror("display ID");
    		return EXIT_FAILURE;
    	} else {
			if (displayID == m_display) {
			    screen_get_display_property_iv(m_screen_dpy[index], SCREEN_PROPERTY_TYPE,  &type);
			    if (type == SCREEN_DISPLAY_TYPE_HDMI) {
			    	returnCode = screen_create_window(&m_screen_win, m_screen_ctx);
			    	if (returnCode) {
			            perror("screen_create_window");
			            return EXIT_FAILURE;
			        }
			    } else {
			    	returnCode = screen_create_window_type(&m_screen_win, m_screen_ctx, SCREEN_CHILD_WINDOW);
			    	if (returnCode) {
			            perror("screen_create_window (child window)");
			            return EXIT_FAILURE;
			        }
			    }
			    if (type == SCREEN_DISPLAY_TYPE_HDMI) {
					returnCode = screen_set_window_property_pv(m_screen_win, SCREEN_PROPERTY_DISPLAY, (void **)&(m_screen_dpy[index]));
					if (returnCode) {
						perror("window display");
						return EXIT_FAILURE;
					}
		        }
			}
        }
	}

	qDebug()  << "OpenGLView::initialize: "<< m_screen_ctx << ":" << m_egl_disp << ":" << m_egl_conf << ":" << m_egl_ctx << ":" << m_screen_win;


	int format = SCREEN_FORMAT_RGBA8888;
	returnCode = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_FORMAT, &format);
	if (returnCode) {
		perror("screen_set_window_property_iv(SCREEN_PROPERTY_FORMAT)");
		return EXIT_FAILURE;
	}

	if (m_transparency > 0) {
		returnCode = setWindowTransparency(m_transparency);
		if (returnCode) {
			perror("transparency");
			return EXIT_FAILURE;
		}
	}

	returnCode = screen_get_window_property_pv(m_screen_win, SCREEN_PROPERTY_DISPLAY, (void **)&m_screen_disp);
	if (returnCode) {
		perror("screen_get_window_property_pv");
		return EXIT_FAILURE;
	}

	int angle = atoi(getenv("ORIENTATION"));

	screen_get_display_property_iv(m_screen_disp, SCREEN_PROPERTY_MODE_COUNT, &numberModes);

	m_screen_modes = (screen_display_mode_t *)calloc(numberModes, sizeof(screen_display_mode_t));
	returnCode = screen_get_display_property_pv(m_screen_disp, SCREEN_PROPERTY_MODE, (void**)m_screen_modes);
	if (returnCode) {
		perror("screen modes");
		return EXIT_FAILURE;
	}

    int dpi = calculateDPI();
    if (dpi == EXIT_FAILURE) {
        fprintf(stderr, "Unable to calculate dpi\n");
        return EXIT_FAILURE;
    }

	returnCode = setWindowPosition(m_x, m_y);
	if (returnCode) {
		perror("window position");
		return EXIT_FAILURE;
	}

	returnCode = setWindowSize(m_width, m_height);
	if (returnCode) {
		perror("window size");
		return EXIT_FAILURE;
	}

	returnCode = setWindowZ(m_z);
	if (returnCode) {
		perror("z order");
		return EXIT_FAILURE;
	}

	returnCode = setWindowBufferSize(m_width, m_height);
	if (returnCode) {
		perror("buffer size");
		return EXIT_FAILURE;
	}

	returnCode = setWindowAngle(m_angle);
	if (returnCode) {
		perror("angle");
		return EXIT_FAILURE;
	}

	returnCode = screen_create_window_buffers(m_screen_win, m_nbuffers);
	if (returnCode) {
		perror("screen_create_window_buffers");
		return EXIT_FAILURE;
	}


    if (m_api == GL_ES_1) {
        m_usage = SCREEN_USAGE_OPENGL_ES1 | SCREEN_USAGE_ROTATION;
    } else if (m_api == GL_ES_2) {
    	attrib_list[9] = EGL_OPENGL_ES2_BIT;
    	m_usage = SCREEN_USAGE_OPENGL_ES2 | SCREEN_USAGE_ROTATION;
    } else if (m_api == VG) {
    	attrib_list[9] = EGL_OPENVG_BIT;
    	m_usage = SCREEN_USAGE_OPENVG | SCREEN_USAGE_ROTATION;
    } else {
        fprintf(stderr, "invalid api setting\n");
        return EXIT_FAILURE;
    }

	returnCode = setWindowUsage(m_usage);
	if (returnCode) {
		perror("screen_set_window_property_iv(window usage)");
		return EXIT_FAILURE;
	}

	qDebug()  << "OpenGLView::initGL:eglCreateContext "<< m_egl_ctx;
	m_egl_surf = eglCreateWindowSurface(m_egl_disp, m_egl_conf, m_screen_win, NULL);
	if (m_egl_surf == EGL_NO_SURFACE) {
		OpenGLThread::eglPrintError("eglCreateWindowSurface");
		return EXIT_FAILURE;
	}

	getGLContext();

    EGLint interval = 1;
    status = eglSwapInterval(m_egl_disp, interval);
	if (status != EGL_TRUE) {
		OpenGLThread::eglPrintError("eglSwapInterval");
		return EXIT_FAILURE;
	}

    status = eglQuerySurface(m_egl_disp, m_egl_surf, EGL_WIDTH, &m_surface_width);
	if (status != EGL_TRUE) {
		perror("query surface width");
		return EXIT_FAILURE;
	}

    status = eglQuerySurface(m_egl_disp, m_egl_surf, EGL_HEIGHT, &m_surface_height);
	if (status != EGL_TRUE) {
		perror("query surface height");
		return EXIT_FAILURE;
	}

	returnCode = joinWindowGroup(m_group);
	if (returnCode) {
		perror("window group");
		return EXIT_FAILURE;
	}

	returnCode = setScreenWindowID(m_id);
	if (returnCode) {
		perror("window ID");
		return EXIT_FAILURE;
	}

	qDebug()  << "OpenGLView::initGL: "<< angle << ":" << numberModes << ":" << m_screen_modes[0].width << ":" << m_screen_modes[0].height << ":" << m_egl_disp << ":" << dpi;

	setInitialized(true);

	return EXIT_SUCCESS;
}

EGLDisplay OpenGLView::display()
{
	return m_egl_disp;
}

void OpenGLView::setDisplay(VIEW_DISPLAY display)
{
	m_display = display;
	m_egl_disp = OpenGLThread::getInstance()->getDisplay(display);
	setScreenEGLDisplay(m_egl_disp);
}

void OpenGLView::setScreenContext(screen_context_t screen_ctx)
{
    m_screen_ctx = screen_ctx;
}


void OpenGLView::setAPI(RENDERING_API api)
{
	m_api = api;
}

void OpenGLView::setScreenEGLConfiguration(EGLConfig egl_conf)
{
    m_egl_conf = egl_conf;
}

void OpenGLView::setScreenEGLDisplay(EGLDisplay egl_disp)
{
    m_egl_disp = egl_disp;
}

void OpenGLView::shutdown() {
	OpenGLThread::getInstance()->shutdown();

	while (OpenGLThread::getInstance()->isRunning()) {
		usleep(100);
	}

}

void OpenGLView::cleanup() {
    if (m_egl_disp != EGL_NO_DISPLAY) {
    	releaseGLContext();
		if (m_egl_ctx != EGL_NO_CONTEXT) {
			eglDestroyContext(m_egl_disp, m_egl_ctx);
			m_egl_ctx = EGL_NO_CONTEXT;
		}
		if (m_egl_surf != EGL_NO_SURFACE) {
			eglDestroySurface(m_egl_disp, m_egl_surf);
			m_egl_surf = EGL_NO_SURFACE;
		}
    }

	if (m_screen_win != NULL) {
		screen_destroy_window(m_screen_win);
		m_screen_win = NULL;
	}

	if (m_screen_dpy != NULL) {
		free(m_screen_dpy);
	}

	if (m_screen_win != NULL) {
		free(m_screen_modes);
	}
}

void OpenGLView::setEnabled(bool enabled) {
	m_enabled = enabled;
}

bool OpenGLView::enabled() {
	return m_enabled;
}

void OpenGLView::setInitialized(bool initialized)
{
	m_viewMutex.lock();

	m_initialized = initialized;

	m_viewMutex.unlock();
}

bool OpenGLView::initialized() {
	bool initialized;

	m_viewMutex.lock();

	initialized = m_initialized;

	m_viewMutex.unlock();

	return initialized;
}

void OpenGLView::setStale(bool stale)
{
	m_viewMutex.lock();

	m_stale = stale;

	m_viewMutex.unlock();
}

bool OpenGLView::stale() {
	bool stale;

	m_viewMutex.lock();

	stale = m_stale;

	m_viewMutex.unlock();

	return stale;
}

void OpenGLView::setAltered(bool altered)
{
	m_viewMutex.lock();

	m_altered = altered;

	m_viewMutex.unlock();
}

bool OpenGLView::altered() {
	bool altered;

	m_viewMutex.lock();

	altered = m_altered;

	m_viewMutex.unlock();

	return altered;
}

void OpenGLView::setVisible(bool visible)
{
	m_viewMutex.lock();

	m_visible = visible;

	m_viewMutex.unlock();
}

bool OpenGLView::visible() {
	bool visible;

	m_viewMutex.lock();

	visible = m_visible;

	m_viewMutex.unlock();

	return visible;
}

void OpenGLView::setAngle(int angle)
{
	if ((angle != 0) && (angle != 90) && (angle != 180) && (angle != 270)) {
		fprintf(stderr, "Invalid angle\n");
	} else {
		m_angle = angle;
	}
}

void OpenGLView::setPosition(int x, int y)
{
	m_x = x;
	m_y = y;
}

void OpenGLView::setSize(int width, int height)
{
	m_width = width;
	m_height = height;
}

void OpenGLView::setZ(int z)
{
	m_z = z;
}

void OpenGLView::setTransparency(int transparency)
{
	m_transparency = transparency;
}

void OpenGLView::setWindowGroup(const QString &group)
{
	m_group = group;
}

void OpenGLView::setWindowID(const QString id)
{
	m_id = id;
}

int OpenGLView::setWindowAngle(int angle)
{
	int returnCode;

	if (m_screen_win != NULL) {
		returnCode = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_ROTATION, &angle);
	} else {
		returnCode = EXIT_SUCCESS;
	}

	return returnCode;
}

int OpenGLView::setWindowPosition(int x, int y)
{
	int returnCode;
	int position[2];

	if (m_screen_win != NULL) {
		position[0] = x;
		position[1] = y;

		returnCode = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_POSITION, position);
	} else {
		returnCode = EXIT_SUCCESS;
	}

	return returnCode;
}

int OpenGLView::setWindowSize(int width, int height)
{
	int returnCode;
	int size[2];

	if (m_screen_win != NULL) {
		size[0] = width;
		size[1] = height;

		returnCode = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_SIZE, size);
	} else {
		returnCode = EXIT_SUCCESS;
	}

	return returnCode;
}

int OpenGLView::setWindowZ(int z)
{
	int returnCode = 0;

	if (m_screen_win != NULL) {
		returnCode = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_ZORDER, &z);
	} else {
		returnCode = EXIT_SUCCESS;
	}

	return returnCode;
}

int OpenGLView::setWindowTransparency(int transparency)
{
	int returnCode = 0;

	if (m_screen_win != NULL) {
		returnCode = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_TRANSPARENCY, &transparency);
	} else {
		returnCode = EXIT_SUCCESS;
	}

	return returnCode;
}

int OpenGLView::setWindowUsage(int usage)
{
	int returnCode = 0;

	if (m_screen_win != NULL) {
		returnCode = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_USAGE, &usage);
	} else {
		returnCode = EXIT_SUCCESS;
	}

	return returnCode;
}

int OpenGLView::setWindowSourceSize(int width, int height)
{
	int returnCode;
	int size[2];

	if (m_screen_win != NULL) {
		size[0] = width;
		size[1] = height;

		returnCode = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_SOURCE_SIZE, size);
	} else {
		returnCode = EXIT_SUCCESS;
	}

	return returnCode;
}

int OpenGLView::setWindowBufferSize(int width, int height)
{
	int returnCode;
	int size[2];

	if (m_screen_win != NULL) {
		size[0] = width;
		size[1] = height;

		returnCode = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_BUFFER_SIZE, size);
	} else {
		returnCode = EXIT_SUCCESS;
	}

	return returnCode;
}

int OpenGLView::joinWindowGroup(const QString &group)
{
	int returnCode = 0;

	if (m_screen_win != NULL) {
		returnCode = screen_join_window_group(m_screen_win, group.toAscii());
	} else {
		returnCode = EXIT_SUCCESS;
	}

	return returnCode;
}

int OpenGLView::setScreenWindowID(const QString id)
{
	int returnCode = 0;

	if (m_screen_win != NULL) {
		returnCode = screen_set_window_property_cv(m_screen_win, SCREEN_PROPERTY_ID_STRING, id.toAscii().length(), id.toAscii());
	} else {
		returnCode = EXIT_SUCCESS;
	}

	return returnCode;
}



int OpenGLView::regenerate() {
	int returnCode;
	EGLBoolean status;
	EGLint interval = 1;

	OpenGLView::m_renderMutex.lock();

    status = eglQuerySurface(m_egl_disp, m_egl_surf, EGL_WIDTH, &m_surface_width);
	if (status != EGL_TRUE) {
		perror("query surface width");
		return EXIT_FAILURE;
	}

    status = eglQuerySurface(m_egl_disp, m_egl_surf, EGL_HEIGHT, &m_surface_height);
	if (status != EGL_TRUE) {
		perror("query surface height");
		return EXIT_FAILURE;
	}

/*
	rc = screen_get_window_property_iv(m_screen_win, SCREEN_PROPERTY_ROTATION, &rotation);
	if (rc) {
		perror("screen_set_window_property_iv");
		return EXIT_FAILURE;
	}

	rc = screen_get_window_property_iv(m_screen_win, SCREEN_PROPERTY_BUFFER_SIZE, size);
	if (rc) {
		perror("screen_set_window_property_iv");
		return EXIT_FAILURE;
	}

	switch (angle - rotation) {
		case -270:
		case -90:
		case 90:
		case 270:
			temp = size[0];
			size[0] = size[1];
			size[1] = temp;
			skip = 0;
			break;
	}
*/

	status = eglMakeCurrent(m_egl_disp, NULL, NULL, NULL);
	if (status != EGL_TRUE) {
		OpenGLThread::eglPrintError("eglMakeCurrent");
		return EXIT_FAILURE;
	}

	status = eglDestroySurface(m_egl_disp, m_egl_surf);
	if (status != EGL_TRUE) {
		OpenGLThread::eglPrintError("eglMakeCurrent");
		return EXIT_FAILURE;
	}

	returnCode = setWindowPosition(m_x, m_y);
	if (returnCode) {
		perror("window position");
		return EXIT_FAILURE;
	}

	returnCode = setWindowSize(m_width, m_height);
	if (returnCode) {
		perror("window size");
		return EXIT_FAILURE;
	}
/*
	setWindowAngle(m_angle);
	if (returnCode) {
		perror("window angle");
		return EXIT_FAILURE;
	}
*/
	returnCode = setWindowSourceSize(m_width, m_height);
	if (returnCode) {
		perror("unable to set window source size");
		return EXIT_FAILURE;
	}

	returnCode = setWindowBufferSize(m_width, m_height);
	if (returnCode) {
		perror("buffer size");
		return EXIT_FAILURE;
	}

	m_egl_surf = eglCreateWindowSurface(m_egl_disp, m_egl_conf, m_screen_win, NULL);
	if (m_egl_surf == EGL_NO_SURFACE) {
		OpenGLThread::eglPrintError("eglCreateWindowSurface");
		return EXIT_FAILURE;
	}

	getGLContext();

    status = eglSwapInterval(m_egl_disp, interval);
	if (status != EGL_TRUE) {
		OpenGLThread::eglPrintError("eglSwapInterval");
		return EXIT_FAILURE;
	}

	OpenGLView::m_renderMutex.unlock();

	setAltered(false);

	setStale(true);

	return EXIT_SUCCESS;
}

void OpenGLView::getGLContext()
{
	EGLBoolean status;

	if (m_egl_ctx != eglGetCurrentContext()) {
		status = eglMakeCurrent(m_egl_disp, m_egl_surf, m_egl_surf, m_egl_ctx);
		if (status != EGL_TRUE) {
			OpenGLThread::eglPrintError("getGLContext (eglMakeCurrent)");
		}
	}
}

void OpenGLView::releaseGLContext()
{
	EGLBoolean status;

	if (m_egl_ctx != NULL) {
		status = eglMakeCurrent(m_egl_disp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (status != EGL_TRUE) {
			OpenGLThread::eglPrintError("releaseGLContext (eglMakeCurrent)");
		}
	}
}

void OpenGLView::swapBuffers() {
	EGLBoolean status;

    status = eglSwapBuffers(m_egl_disp, m_egl_surf);
    if (status != EGL_TRUE) {
        OpenGLThread::eglPrintError("eglSwapBuffers");
    }

}

void OpenGLView::renderView()
{
	OpenGLView::m_renderMutex.lock();

	getGLContext();

	render();

	swapBuffers();

	OpenGLView::m_renderMutex.unlock();
}

void OpenGLView::add()
{
	OpenGLThread::getInstance()->addView(this);
}

void OpenGLView::remove()
{
	OpenGLThread::getInstance()->removeView(this);
}


/* Finds the next power of 2 */
int OpenGLView::nextp2(int x)
{
    int val = 1;
    while(val < x) val <<= 1;
    return val;
}

int OpenGLView::loadTexture(const char* filename, int* width, int* height, float* tex_x, float* tex_y, unsigned int *tex) {
    int i;
    GLuint format;
    //header for testing if it is a png
    png_byte header[8];

    if (!tex) {
        return EXIT_FAILURE;
    }

    //open file as binary
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        return EXIT_FAILURE;
    }

    //read the header
    fread(header, 1, 8, fp);

    //test if png
    int is_png = !png_sig_cmp(header, 0, 8);
    if (!is_png) {
        fclose(fp);
        return EXIT_FAILURE;
    }

    //create png struct
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fclose(fp);
        return EXIT_FAILURE;
    }

    //create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
        fclose(fp);
        return EXIT_FAILURE;
    }

    //create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return EXIT_FAILURE;
    }

    //setup error handling (required without using custom error handlers above)
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return EXIT_FAILURE;
    }

    //init png reading
    png_init_io(png_ptr, fp);

    //let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    //variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 image_width, image_height;

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &image_width, &image_height, &bit_depth, &color_type, NULL, NULL, NULL);

    switch (color_type)
    {
        case PNG_COLOR_TYPE_RGBA:
            format = GL_RGBA;
            break;
        case PNG_COLOR_TYPE_RGB:
            format = GL_RGB;
            break;
        default:
            fprintf(stderr,"Unsupported PNG color type (%d) for texture: %s", (int)color_type, filename);
            fclose(fp);
            png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
            return EXIT_FAILURE;
    }

    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // Allocate the image_data as a big block, to be given to opengl
    png_byte *image_data = (png_byte*) malloc(sizeof(png_byte) * rowbytes * image_height);

    if (!image_data) {
        //clean up memory and close stuff
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return EXIT_FAILURE;
    }

    //row_pointers is for pointing to image_data for reading the png with libpng
    png_bytep *row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * image_height);
    if (!row_pointers) {
        //clean up memory and close stuff
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(fp);
        return EXIT_FAILURE;
    }

    // set the individual row_pointers to point at the correct offsets of image_data
    for (i = 0; i < image_height; i++) {
        row_pointers[image_height - 1 - i] = image_data + i * rowbytes;
    }

    //read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);

    int tex_width, tex_height;

    tex_width = nextp2(image_width);
    tex_height = nextp2(image_height);

    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_2D, (*tex));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, format, tex_width, tex_height, 0, format, GL_UNSIGNED_BYTE, NULL);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width, image_height, format, GL_UNSIGNED_BYTE, image_data);

    GLint err = glGetError();

    //clean up memory and close stuff
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    free(image_data);
    free(row_pointers);
    fclose(fp);

    if (err == 0) {
        //Return physical with and height of texture if pointers are not null
        if(width) {
            *width = image_width;
        }
        if (height) {
            *height = image_height;
        }
        //Return modified texture coordinates if pointers are not null
        if(tex_x) {
            *tex_x = ((float) image_width - 0.5f) / ((float)tex_width);
        }
        if(tex_y) {
            *tex_y = ((float) image_height - 0.5f) / ((float)tex_height);
        }
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "GL error %i \n", err);
        return EXIT_FAILURE;
    }
}

int OpenGLView::calculateDPI() {
    int returnCode;
    int screen_phys_size[2];

	qDebug()  << "OpenGLView::calculateDPI: physical: "<< m_egl_disp;

    returnCode = screen_get_display_property_iv(m_screen_disp, SCREEN_PROPERTY_PHYSICAL_SIZE, screen_phys_size);
    if (returnCode) {
        perror("screen_get_display_property_iv");
        return EXIT_FAILURE;
    }

    //Simulator will return 0,0 for physical size of the screen, so use 170 as default dpi
    if ((screen_phys_size[0] == 0) && (screen_phys_size[1] == 0)) {
        return 170;
    } else {
        int screen_resolution[2];

        qDebug()  << "OpenGLView::calculateDPI: screen: "<< m_egl_disp;

        returnCode = screen_get_display_property_iv(m_screen_disp, SCREEN_PROPERTY_SIZE, screen_resolution);
        if (returnCode) {
            perror("screen_get_display_property_iv");
            return EXIT_FAILURE;
        }

        int diagonal_pixels = sqrt(screen_resolution[0] * screen_resolution[0] + screen_resolution[1] * screen_resolution[1]);
        int diagontal_inches = 0.0393700787 * sqrt(screen_phys_size[0] * screen_phys_size[0] + screen_phys_size[1] * screen_phys_size[1]);
        return (int)(diagonal_pixels / diagontal_inches);
    }
}

