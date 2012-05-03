/*
 * Copyright (c) 2011 Research In Motion Limited.
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

#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/keycodes.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#include "bbutil.h"

#include <GLES/gl.h>
#include <GLES/glext.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "png.h"

EGLDisplay egl_disp;
EGLSurface egl_surf;

static EGLConfig egl_conf;
static EGLContext egl_ctx;

static screen_context_t screen_ctx;
static screen_window_t screen_win;
static screen_display_t screen_disp;
static int nbuffers = 2;
static int initialized = 0;

struct font_t {
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
};


static void
bbutil_egl_perror(const char *msg) {
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

int
bbutil_init_egl(screen_context_t ctx, enum RENDERING_API api, int width, int height) {
    int usage;
    int format = SCREEN_FORMAT_RGBA8888;
    EGLint interval = 1;
    int rc, num_configs;
    EGLint attributes[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

    EGLint attrib_list[]= { EGL_RED_SIZE,        8,
                            EGL_GREEN_SIZE,      8,
                            EGL_BLUE_SIZE,       8,
                            EGL_BLUE_SIZE,       8,
                            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
                            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
                            EGL_NONE};

    if (api == GL_ES_1) {
        usage = SCREEN_USAGE_OPENGL_ES1 | SCREEN_USAGE_ROTATION;
    } else if (api == GL_ES_2) {
    	attrib_list[11] = EGL_OPENGL_ES2_BIT;
        usage = SCREEN_USAGE_OPENGL_ES2 | SCREEN_USAGE_ROTATION;
    } else if (api == VG) {
    	attrib_list[11] = EGL_OPENVG_BIT;
        usage = SCREEN_USAGE_OPENVG | SCREEN_USAGE_ROTATION;
    } else {
        fprintf(stderr, "invalid api setting\n");
        return EXIT_FAILURE;
    }

    //Simple egl initialization
    screen_ctx = ctx;

    egl_disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (egl_disp == EGL_NO_DISPLAY) {
        bbutil_egl_perror("eglGetDisplay");
        bbutil_terminate();
        return EXIT_FAILURE;
    }

    rc = eglInitialize(egl_disp, NULL, NULL);
    if (rc != EGL_TRUE) {
        bbutil_egl_perror("eglInitialize");
        bbutil_terminate();
        return EXIT_FAILURE;
    }

    if ((api == GL_ES_1) || (api == GL_ES_2)) {
        rc = eglBindAPI(EGL_OPENGL_ES_API);
    } else if (api == VG) {
        rc = eglBindAPI(EGL_OPENVG_API);
    }

    if (rc != EGL_TRUE) {
        bbutil_egl_perror("eglBindApi");
        bbutil_terminate();
        return EXIT_FAILURE;
    }

    if(!eglChooseConfig(egl_disp, attrib_list, &egl_conf, 1, &num_configs)) {
        bbutil_terminate();
        return EXIT_FAILURE;
    }

    if (api == GL_ES_2) {
        egl_ctx = eglCreateContext(egl_disp, egl_conf, EGL_NO_CONTEXT, attributes);
    } else {
        egl_ctx = eglCreateContext(egl_disp, egl_conf, EGL_NO_CONTEXT, NULL);
    }

    if (egl_ctx == EGL_NO_CONTEXT) {
        bbutil_egl_perror("eglCreateContext");
        bbutil_terminate();
        return EXIT_FAILURE;
    }

    rc = screen_create_window_type(&screen_win, screen_ctx,SCREEN_CHILD_WINDOW);
    if (rc) {
        perror("screen_create_window");
        bbutil_terminate();
        return EXIT_FAILURE;
    }

    rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_FORMAT, &format);
    if (rc) {
        perror("screen_set_window_property_iv(SCREEN_PROPERTY_FORMAT)");
        bbutil_terminate();
        return EXIT_FAILURE;
    }

    rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_USAGE, &usage);
    if (rc) {
        perror("screen_set_window_property_iv(SCREEN_PROPERTY_USAGE)");
        bbutil_terminate();
        return EXIT_FAILURE;
    }

    rc = screen_get_window_property_pv(screen_win, SCREEN_PROPERTY_DISPLAY, (void **)&screen_disp);
    if (rc) {
        perror("screen_get_window_property_pv");
        bbutil_terminate();
        return EXIT_FAILURE;
    }

    int angle = atoi(getenv("ORIENTATION"));

	screen_display_mode_t screen_mode;
	rc = screen_get_display_property_pv(screen_disp, SCREEN_PROPERTY_MODE, (void**)&screen_mode);
	if (rc) {
		perror("screen_get_display_property_pv");
		bbutil_terminate();
		return EXIT_FAILURE;
	}

	int size[2];
	rc = screen_get_window_property_iv(screen_win, SCREEN_PROPERTY_BUFFER_SIZE, size);
	if (rc) {
		perror("screen_get_window_property_iv");
		bbutil_terminate();
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
		 bbutil_terminate();
		 return EXIT_FAILURE;
	}

	buffer_size[0] = width;
	buffer_size[1] = height;

	rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_BUFFER_SIZE, buffer_size);
    if (rc) {
        perror("screen_set_window_property_iv");
        bbutil_terminate();
        return EXIT_FAILURE;
    }

    rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_ROTATION, &angle);
    if (rc) {
        perror("screen_set_window_property_iv");
        bbutil_terminate();
        return EXIT_FAILURE;
    }

    rc = screen_create_window_buffers(screen_win, nbuffers);
    if (rc) {
        perror("screen_create_window_buffers");
        bbutil_terminate();
        return EXIT_FAILURE;
    }

    egl_surf = eglCreateWindowSurface(egl_disp, egl_conf, screen_win, NULL);
    if (egl_surf == EGL_NO_SURFACE) {
        bbutil_egl_perror("eglCreateWindowSurface");
        bbutil_terminate();
        return EXIT_FAILURE;
    }

    rc = eglMakeCurrent(egl_disp, egl_surf, egl_surf, egl_ctx);
    if (rc != EGL_TRUE) {
        bbutil_egl_perror("eglMakeCurrent");
        bbutil_terminate();
        return EXIT_FAILURE;
    }

    rc = eglSwapInterval(egl_disp, interval);
    if (rc != EGL_TRUE) {
        bbutil_egl_perror("eglSwapInterval");
        bbutil_terminate();
        return EXIT_FAILURE;
    }

    initialized = 1;

    return EXIT_SUCCESS;
}

void
bbutil_terminate() {
    //Typical EGL cleanup
    if (egl_disp != EGL_NO_DISPLAY) {
        eglMakeCurrent(egl_disp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (egl_surf != EGL_NO_SURFACE) {
            eglDestroySurface(egl_disp, egl_surf);
            egl_surf = EGL_NO_SURFACE;
        }
        if (egl_ctx != EGL_NO_CONTEXT) {
            eglDestroyContext(egl_disp, egl_ctx);
            egl_ctx = EGL_NO_CONTEXT;
        }
        if (screen_win != NULL) {
            screen_destroy_window(screen_win);
            screen_win = NULL;
        }
        eglTerminate(egl_disp);
        egl_disp = EGL_NO_DISPLAY;
    }
    eglReleaseThread();

    initialized = 0;
}

void
bbutil_swap() {
    int rc = eglSwapBuffers(egl_disp, egl_surf);
    if (rc != EGL_TRUE) {
        bbutil_egl_perror("eglSwapBuffers");
    }
}

/* Finds the next power of 2 */
static inline int
nextp2(int x)
{
    int val = 1;
    while(val < x) val <<= 1;
    return val;
}

font_t* bbutil_load_font(const char* path, int point_size, int dpi) {
    FT_Library library;
    FT_Face face;
    int c;
    int i, j;
    font_t* font;

    if (!initialized) {
        fprintf(stderr, "EGL has not been initialized\n");
        return NULL;
    }

    if (!path){
        fprintf(stderr, "Invalid path to font file\n");
        return NULL;
    }

    if(FT_Init_FreeType(&library)) {
        fprintf(stderr, "Error loading Freetype library\n");
        return NULL;
    }
    if (FT_New_Face(library, path,0,&face)) {
        fprintf(stderr, "Error loading font %s\n", path);
        return NULL;
    }

    if(FT_Set_Char_Size ( face, point_size * 64, point_size * 64, dpi, dpi)) {
        fprintf(stderr, "Error initializing character parameters\n");
        return NULL;
    }

    font = (font_t*) malloc(sizeof(font_t));
    font->initialized = 0;

    glGenTextures(1, &(font->font_texture));

    //Let each glyph reside in 32x32 section of the font texture
    int segment_size_x = 0, segment_size_y = 0;
    int num_segments_x = 16;
    int num_segments_y = 8;

    FT_GlyphSlot slot;
    FT_Bitmap bmp;
    int glyph_width, glyph_height;

    //First calculate the max width and height of a character in a passed font
    for(c = 0; c < 128; c++) {
        if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            fprintf(stderr, "FT_Load_Char failed\n");
            free(font);
            return NULL;
        }

        slot = face->glyph;
        bmp = slot->bitmap;

        //glyph_width = nextp2(bmp.width);
        //glyph_height = nextp2(bmp.rows);

        glyph_width = bmp.width;
        glyph_height = bmp.rows;

        if (glyph_width > segment_size_x) {
            segment_size_x = glyph_width;
        }

        if (glyph_height > segment_size_y) {
            segment_size_y = glyph_height;
        }
    }

    int font_tex_width = nextp2(num_segments_x * segment_size_x);
    int font_tex_height = nextp2(num_segments_y * segment_size_y);

    int bitmap_offset_x = 0, bitmap_offset_y = 0;

    GLubyte* font_texture_data = (GLubyte*) malloc(sizeof(GLubyte) * 2 * font_tex_width * font_tex_height);
    memset((void*)font_texture_data, 0, sizeof(GLubyte) * 2 * font_tex_width * font_tex_height);

    if (!font_texture_data) {
        fprintf(stderr, "Failed to allocate memory for font texture\n");
        free(font);
        return NULL;
    }

    // Fill font texture bitmap with individual bmp data and record appropriate size, texture coordinates and offsets for every glyph
    for(c = 0; c < 128; c++) {
        if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            fprintf(stderr, "FT_Load_Char failed\n");
            free(font);
            return NULL;
        }

        slot = face->glyph;
        bmp = slot->bitmap;

        glyph_width = nextp2(bmp.width);
        glyph_height = nextp2(bmp.rows);

        div_t temp = div(c, num_segments_x);

        bitmap_offset_x = segment_size_x * temp.rem;
        bitmap_offset_y = segment_size_y * temp.quot;

        for (j = 0; j < glyph_height; j++) {
            for (i = 0; i < glyph_width; i++) {
                font_texture_data[2 * ((bitmap_offset_x + i) + (j + bitmap_offset_y) * font_tex_width) + 0] =
                font_texture_data[2 * ((bitmap_offset_x + i) + (j + bitmap_offset_y) * font_tex_width) + 1] =
                    (i >= bmp.width || j >= bmp.rows)? 0 : bmp.buffer[i + bmp.width * j];
            }
        }

        font->advance[c] = (float)(slot->advance.x >> 6);
        font->tex_x1[c] = (float)bitmap_offset_x / (float) font_tex_width;
        font->tex_x2[c] = (float)(bitmap_offset_x + bmp.width) / (float)font_tex_width;
        font->tex_y1[c] = (float)bitmap_offset_y / (float) font_tex_height;
        font->tex_y2[c] = (float)(bitmap_offset_y + bmp.rows) / (float)font_tex_height;
        font->width[c] = bmp.width;
        font->height[c] = bmp.rows;
        font->offset_x[c] = (float)slot->bitmap_left;
        font->offset_y[c] =  (float)((slot->metrics.horiBearingY-face->glyph->metrics.height) >> 6);
    }

    glBindTexture(GL_TEXTURE_2D, font->font_texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, font_tex_width, font_tex_height, 0, GL_LUMINANCE_ALPHA , GL_UNSIGNED_BYTE, font_texture_data);

    int err = glGetError();

    free(font_texture_data);

    FT_Done_Face(face);
    FT_Done_FreeType(library);

    if (err != 0) {
        fprintf(stderr, "GL Error 0x%x", err);
        free(font);
        return NULL;
    }

    font->initialized = 1;
    return font;
}

void bbutil_render_text(font_t* font, const char* msg, float x, float y) {
    int i, c;
    GLfloat *vertices;
    GLfloat *texture_coords;
    GLshort* indices;

    float pen_x = 0.0f;

    if (!font) {
        fprintf(stderr, "Font must not be null\n");
        return;
    }

    if (!font->initialized) {
        fprintf(stderr, "Font has not been loaded\n");
        return;
    }

    if (!msg) {
        return;
    }

    int texture_enabled;
    glGetIntegerv(GL_TEXTURE_2D, &texture_enabled);
    if (!texture_enabled) {
        glEnable(GL_TEXTURE_2D);
    }

    int blend_enabled;
    glGetIntegerv(GL_BLEND, &blend_enabled);
    if (!blend_enabled) {
        glEnable(GL_BLEND);
    }

    int gl_blend_src, gl_blend_dst;
    glGetIntegerv(GL_BLEND_SRC, &gl_blend_src);
    glGetIntegerv(GL_BLEND_DST, &gl_blend_dst);

    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int vertex_array_enabled;
    glGetIntegerv(GL_VERTEX_ARRAY, &vertex_array_enabled);
    if (!vertex_array_enabled) {
        glEnableClientState(GL_VERTEX_ARRAY);
    }

    int texture_array_enabled;
    glGetIntegerv(GL_TEXTURE_COORD_ARRAY, &texture_array_enabled);
    if (!texture_array_enabled) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    vertices = (GLfloat*) malloc(sizeof(GLfloat) * 8 * strlen(msg));
    texture_coords = (GLfloat*) malloc(sizeof(GLfloat) * 8 * strlen(msg));

    indices = (GLshort*) malloc(sizeof(GLfloat) * 5 * strlen(msg));

    for(i = 0; i < strlen(msg); ++i) {
        c = msg[i];

        vertices[8 * i + 0] = x + pen_x + font->offset_x[c];
        vertices[8 * i + 1] = y + font->offset_y[c];
        vertices[8 * i + 2] = vertices[8 * i + 0] + font->width[c];
        vertices[8 * i + 3] = vertices[8 * i + 1];
        vertices[8 * i + 4] = vertices[8 * i + 0];
        vertices[8 * i + 5] = vertices[8 * i + 1] + font->height[c];
        vertices[8 * i + 6] = vertices[8 * i + 2];
        vertices[8 * i + 7] = vertices[8 * i + 5];

        texture_coords[8 * i + 0] = font->tex_x1[c];
        texture_coords[8 * i + 1] = font->tex_y2[c];
        texture_coords[8 * i + 2] = font->tex_x2[c];
        texture_coords[8 * i + 3] = font->tex_y2[c];
        texture_coords[8 * i + 4] = font->tex_x1[c];
        texture_coords[8 * i + 5] = font->tex_y1[c];
        texture_coords[8 * i + 6] = font->tex_x2[c];
        texture_coords[8 * i + 7] = font->tex_y1[c];

        indices[i * 6 + 0] = 4 * i + 0;
        indices[i * 6 + 1] = 4 * i + 1;
        indices[i * 6 + 2] = 4 * i + 2;
        indices[i * 6 + 3] = 4 * i + 2;
        indices[i * 6 + 4] = 4 * i + 1;
        indices[i * 6 + 5] = 4 * i + 3;

        //Assume we are only working with typewriter fonts
        pen_x += font->advance[c];
    }

    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, texture_coords);
    glBindTexture(GL_TEXTURE_2D, font->font_texture);

    glDrawElements(GL_TRIANGLES, 6 * strlen(msg), GL_UNSIGNED_SHORT, indices);

    if (!texture_array_enabled) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    if (!vertex_array_enabled) {
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (!texture_enabled) {
        glDisable(GL_TEXTURE_2D);
    }

    glBlendFunc(gl_blend_src, gl_blend_dst);

    if (!blend_enabled) {
        glDisable(GL_BLEND);
    }

    free(vertices);
    free(texture_coords);
    free(indices);
}

void bbutil_destroy_font(font_t* font) {
    if (!font) {
        return;
    }

    glDeleteTextures(1, &(font->font_texture));

    free(font);
}

void bbutil_measure_text(font_t* font, const char* msg, float* width, float* height) {
    int i, c;

    if (!msg) {
        return;
    }

    if (width) {
        //Width of a text rectangle is a sum advances for every glyph in a string
        *width = 0.0f;

        for(i = 0; i < strlen(msg); ++i) {
            c = msg[i];
            *width += font->advance[c];
        }
    }

    if (height) {
        //Height of a text rectangle is a high of a tallest glyph in a string
        *height = 0.0f;

        for(i = 0; i < strlen(msg); ++i) {
            c = msg[i];

            if (*height < font->height[c]) {
                *height = font->height[c];
            }
        }
    }
}

int bbutil_load_texture(const char* filename, int* width, int* height, float* tex_x, float* tex_y, unsigned int *tex) {
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
            return NULL;
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

int bbutil_calculate_dpi(screen_context_t ctx) {
    int rc;
    int screen_phys_size[2];

    rc = screen_get_display_property_iv(screen_disp, SCREEN_PROPERTY_PHYSICAL_SIZE, screen_phys_size);
    if (rc) {
        perror("screen_get_display_property_iv");
        bbutil_terminate();
        return EXIT_FAILURE;
    }

    //Simulator will return 0,0 for physical size of the screen, so use 170 as default dpi
    if ((screen_phys_size[0] == 0) && (screen_phys_size[1] == 0)) {
        return 170;
    } else {
        int screen_resolution[2];
        rc = screen_get_display_property_iv(screen_disp, SCREEN_PROPERTY_SIZE, screen_resolution);
        if (rc) {
            perror("screen_get_display_property_iv");
            bbutil_terminate();
            return EXIT_FAILURE;
        }

        int diagonal_pixels = sqrt(screen_resolution[0] * screen_resolution[0] + screen_resolution[1] * screen_resolution[1]);
        int diagontal_inches = 0.0393700787 * sqrt(screen_phys_size[0] * screen_phys_size[0] + screen_phys_size[1] * screen_phys_size[1]);
        return (int)(diagonal_pixels / diagontal_inches);
    }
}

int bbutil_rotate_screen_surface(int angle) {
    int rc, rotation, skip = 1, temp;;
    EGLint interval = 1;
    int size[2];

    if ((angle != 0) && (angle != 90) && (angle != 180) && (angle != 270)) {
        fprintf(stderr, "Invalid angle\n");
        return EXIT_FAILURE;
    }

    rc = screen_get_window_property_iv(screen_win, SCREEN_PROPERTY_ROTATION, &rotation);
    if (rc) {
        perror("screen_set_window_property_iv");
        return EXIT_FAILURE;
    }

    rc = screen_get_window_property_iv(screen_win, SCREEN_PROPERTY_BUFFER_SIZE, size);
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
        rc = eglMakeCurrent(egl_disp, NULL, NULL, NULL);
        if (rc != EGL_TRUE) {
            bbutil_egl_perror("eglMakeCurrent");
            return EXIT_FAILURE;
        }

        rc = eglDestroySurface(egl_disp, egl_surf);
        if (rc != EGL_TRUE) {
            bbutil_egl_perror("eglMakeCurrent");
            return EXIT_FAILURE;
        }

        rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_SOURCE_SIZE, size);
        if (rc) {
            perror("screen_set_window_property_iv");
            return EXIT_FAILURE;
        }

        rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_BUFFER_SIZE, size);
        if (rc) {
            perror("screen_set_window_property_iv");
            return EXIT_FAILURE;
        }
        egl_surf = eglCreateWindowSurface(egl_disp, egl_conf, screen_win, NULL);
        if (egl_surf == EGL_NO_SURFACE) {
            bbutil_egl_perror("eglCreateWindowSurface");
            return EXIT_FAILURE;
        }

        rc = eglMakeCurrent(egl_disp, egl_surf, egl_surf, egl_ctx);
        if (rc != EGL_TRUE) {
            bbutil_egl_perror("eglMakeCurrent");
            return EXIT_FAILURE;
        }

        rc = eglSwapInterval(egl_disp, interval);
        if (rc != EGL_TRUE) {
            bbutil_egl_perror("eglSwapInterval");
            return EXIT_FAILURE;
        }
    }

    rc = screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_ROTATION, &angle);
    if (rc) {
        perror("screen_set_window_property_iv");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

screen_window_t bbutil_get_window(){
	return screen_win;
}


