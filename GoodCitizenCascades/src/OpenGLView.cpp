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

OpenGLView::OpenGLView() : QObject(NULL)
{
	// initialize members shared by derived classes
	m_nbuffers = 2;
}

OpenGLView::~OpenGLView() {
	// TODO Auto-generated destructor stub
}



int OpenGLView::initGL(EGLContext egl_ctx, EGLConfig egl_conf, EGLDisplay egl_disp, screen_context_t screen_cxt, int usage, int z, const char* group, const char* id) {
    int format = SCREEN_FORMAT_RGBA8888;
    EGLint interval = 1;

	m_screen_cxt = screen_cxt;
	m_egl_disp = egl_disp;
	m_egl_conf = egl_conf;
	m_egl_ctx = egl_ctx;

    //int rc = screen_create_window(&m_screen_win, screen_cxt);
	int rc = screen_create_window_type(&m_screen_win, screen_cxt, SCREEN_CHILD_WINDOW);
    if (rc) {
        perror("screen_create_window");
        return EXIT_FAILURE;
    }

	qDebug()  << "OpenGLView::initialize: "<< m_screen_cxt << ":" << m_egl_disp << ":" << m_egl_conf << ":" << m_egl_ctx << ":" << m_screen_win;

	rc = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_FORMAT, &format);
	if (rc) {
		perror("screen_set_window_property_iv(SCREEN_PROPERTY_FORMAT)");
		return EXIT_FAILURE;
	}

	rc = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_USAGE, &usage);
	if (rc) {
		perror("screen_set_window_property_iv(SCREEN_PROPERTY_USAGE)");
		return EXIT_FAILURE;
	}

	rc = screen_get_window_property_pv(m_screen_win, SCREEN_PROPERTY_DISPLAY, (void **)&m_screen_disp);
	if (rc) {
		perror("screen_get_window_property_pv");
		return EXIT_FAILURE;
	}

	int angle = atoi(getenv("ORIENTATION"));

	screen_display_mode_t screen_mode;
	rc = screen_get_display_property_pv(m_screen_disp, SCREEN_PROPERTY_MODE, (void**)&screen_mode);
	if (rc) {
		perror("screen_get_display_property_pv");
		return EXIT_FAILURE;
	}

    int dpi = calculateDPI();

    if (dpi == EXIT_FAILURE) {
        fprintf(stderr, "Unable to calculate dpi\n");
        return EXIT_FAILURE;
    }

	int size[2];
	rc = screen_get_window_property_iv(m_screen_win, SCREEN_PROPERTY_BUFFER_SIZE, size);
	if (rc) {
		perror("screen_get_window_property_iv");
		return EXIT_FAILURE;
	}

	int buffer_size[2] = {size[0], size[1]};

	if ((angle == 0) || (angle == 180)) {
		if (((screen_mode.width > screen_mode.height) && (size[0] < size[1])) ||
			((screen_mode.width < screen_mode.height) && (size[0] > size[1]))) {
				buffer_size[1] = size[0];
				buffer_size[0] = size[1];
		}
	} else if ((angle == 90) || (angle == 270)){
		if (((screen_mode.width > screen_mode.height) && (size[0] > size[1])) ||
			((screen_mode.width < screen_mode.height && size[0] < size[1]))) {
				buffer_size[1] = size[0];
				buffer_size[0] = size[1];
		}
	} else {
		 fprintf(stderr, "Navigator returned an unexpected orientation angle.\n");
		 return EXIT_FAILURE;
	}

	rc = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_BUFFER_SIZE, buffer_size);
	if (rc) {
		perror("screen_set_window_property_iv");
		return EXIT_FAILURE;
	}

	rc = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_ROTATION, &angle);
	if (rc) {
		perror("screen_set_window_property_iv");
		return EXIT_FAILURE;
	}

	rc = screen_create_window_buffers(m_screen_win, m_nbuffers);
	if (rc) {
		perror("screen_create_window_buffers");
		return EXIT_FAILURE;
	}

	m_egl_surf = eglCreateWindowSurface(m_egl_disp, m_egl_conf, m_screen_win, NULL);
	if (m_egl_surf == EGL_NO_SURFACE) {
		OpenGLThread::eglPrintError("eglCreateWindowSurface");
		return EXIT_FAILURE;
	}

	rc = eglMakeCurrent(m_egl_disp, m_egl_surf, m_egl_surf, m_egl_ctx);
	if (rc != EGL_TRUE) {
		OpenGLThread::eglPrintError("eglMakeCurrent");
		return EXIT_FAILURE;
	}

	rc = eglSwapInterval(m_egl_disp, interval);
	if (rc != EGL_TRUE) {
		OpenGLThread::eglPrintError("eglSwapInterval");
		return EXIT_FAILURE;
	}

    eglQuerySurface(m_egl_disp, m_egl_surf, EGL_WIDTH, &m_surface_width);
    eglQuerySurface(m_egl_disp, m_egl_surf, EGL_HEIGHT, &m_surface_height);

	rc = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_ZORDER, &z);
	if (rc) {
		perror("screen_set_window_property_iv");
		return EXIT_FAILURE;
	}

	if (group) {
		rc = screen_join_window_group(m_screen_win, group);
		if (rc) {
			perror("screen_set_window_property_iv");
			return EXIT_FAILURE;
		}
	}

	if (id) {
		rc = screen_set_window_property_cv(m_screen_win, SCREEN_PROPERTY_ID_STRING,
				sizeof(id), id);
		if (rc) {
			perror("screen_set_window_property_iv");
			return EXIT_FAILURE;
		}
	}

	qDebug()  << "OpenGLView::initGL: "<< angle << ":" << screen_mode.width << ":" << screen_mode.height << ":" << m_screen_disp << ":" << dpi << ":" << z << ":" << group << ":" << id;

	return EXIT_SUCCESS;
}

void OpenGLView::cleanup() {
    if (m_egl_disp != EGL_NO_DISPLAY) {
		eglMakeCurrent(m_egl_disp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (m_egl_surf != EGL_NO_SURFACE) {
			eglDestroySurface(m_egl_disp, m_egl_surf);
			m_egl_surf = EGL_NO_SURFACE;
		}
    }
	if (m_screen_win != NULL) {
		screen_destroy_window(m_screen_win);
		m_screen_win = NULL;
	}
}

int OpenGLView::rotate(int angle) {
	return rotateScreenSurface(angle);
}

void OpenGLView::swapBuffers() {
    int rc = eglSwapBuffers(m_egl_disp, m_egl_surf);
    if (rc != EGL_TRUE) {
        OpenGLThread::eglPrintError("eglSwapBuffers");
    }

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


int OpenGLView::rotateScreenSurface(int angle) {
    int rc, rotation, skip = 1, temp;
    EGLint interval = 1;
    int size[2];

    eglQuerySurface(m_egl_disp, m_egl_surf, EGL_WIDTH, &m_surface_width);
    eglQuerySurface(m_egl_disp, m_egl_surf, EGL_HEIGHT, &m_surface_height);

    if ((angle != 0) && (angle != 90) && (angle != 180) && (angle != 270)) {
        fprintf(stderr, "Invalid angle\n");
        return EXIT_FAILURE;
    }

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

    if (!skip) {
        rc = eglMakeCurrent(m_egl_disp, NULL, NULL, NULL);
        if (rc != EGL_TRUE) {
            OpenGLThread::eglPrintError("eglMakeCurrent");
            return EXIT_FAILURE;
        }

        rc = eglDestroySurface(m_egl_disp, m_egl_surf);
        if (rc != EGL_TRUE) {
            OpenGLThread::eglPrintError("eglMakeCurrent");
            return EXIT_FAILURE;
        }

        rc = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_SOURCE_SIZE, size);
        if (rc) {
            perror("screen_set_window_property_iv");
            return EXIT_FAILURE;
        }

        rc = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_BUFFER_SIZE, size);
        if (rc) {
            perror("screen_set_window_property_iv");
            return EXIT_FAILURE;
        }
        m_egl_surf = eglCreateWindowSurface(m_egl_disp, m_egl_conf, m_screen_win, NULL);
        if (m_egl_surf == EGL_NO_SURFACE) {
            OpenGLThread::eglPrintError("eglCreateWindowSurface");
            return EXIT_FAILURE;
        }

        rc = eglMakeCurrent(m_egl_disp, m_egl_surf, m_egl_surf, m_egl_ctx);
        if (rc != EGL_TRUE) {
            OpenGLThread::eglPrintError("eglMakeCurrent");
            return EXIT_FAILURE;
        }

        rc = eglSwapInterval(m_egl_disp, interval);
        if (rc != EGL_TRUE) {
            OpenGLThread::eglPrintError("eglSwapInterval");
            return EXIT_FAILURE;
        }
    }

    rc = screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_ROTATION, &angle);
    if (rc) {
        perror("screen_set_window_property_iv");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int OpenGLView::calculateDPI() {
    int rc;
    int screen_phys_size[2];

	qDebug()  << "OpenGLView::calculateDPI: physical: "<< m_screen_disp;

    rc = screen_get_display_property_iv(m_screen_disp, SCREEN_PROPERTY_PHYSICAL_SIZE, screen_phys_size);
    if (rc) {
        perror("screen_get_display_property_iv");
        return EXIT_FAILURE;
    }

    //Simulator will return 0,0 for physical size of the screen, so use 170 as default dpi
    if ((screen_phys_size[0] == 0) && (screen_phys_size[1] == 0)) {
        return 170;
    } else {
        int screen_resolution[2];

        qDebug()  << "OpenGLView::calculateDPI: screen: "<< m_screen_disp;

        rc = screen_get_display_property_iv(m_screen_disp, SCREEN_PROPERTY_SIZE, screen_resolution);
        if (rc) {
            perror("screen_get_display_property_iv");
            return EXIT_FAILURE;
        }

        int diagonal_pixels = sqrt(screen_resolution[0] * screen_resolution[0] + screen_resolution[1] * screen_resolution[1]);
        int diagontal_inches = 0.0393700787 * sqrt(screen_phys_size[0] * screen_phys_size[0] + screen_phys_size[1] * screen_phys_size[1]);
        return (int)(diagonal_pixels / diagontal_inches);
    }
}

