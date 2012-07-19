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


#ifndef GOODCITIZEN_HPP
#define GOODCITIZEN_HPP

#include <assert.h>
#include <screen/screen.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <bps/navigator.h>
#include <bps/screen.h>
#include <bps/bps.h>
#include <bps/event.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <bb/cascades/ForeignWindow>

#include <QtCore/QObject>
#include <QtCore/QString>

#include "OpenGLView.hpp"

class GoodCitizen : public OpenGLView {

Q_OBJECT

Q_PROPERTY(QVariantList objectColor READ objectColor WRITE objectColor)
Q_PROPERTY(QString toolAxis READ toolAxis WRITE setToolAxis)
Q_PROPERTY(QString touchMode READ touchMode WRITE setTouchMode)
Q_PROPERTY(QString model READ model WRITE setModel)

public:
	GoodCitizen();

	virtual ~GoodCitizen();

	void bind(const QString &group, const QString id, int x, int y, int width, int height);

	QVariantList& objectColor();
	void objectColor(QVariantList& color);

	QString toolAxis();
	QString touchMode();
	QString model();

Q_SIGNALS:
	//void result(const QString &result);

public Q_SLOTS:
	void setObjectColor(QVariantList objectColor);
	void setToolAxis(QString toolAxis);
	void setTouchMode(QString touchMode);
	void setModel(QString touchMode);
	void reset();

	void onTouch(bb::cascades::TouchEvent *event);

public:

	int initialize(EGLContext egl_ctx, EGLConfig egl_conf, EGLDisplay egl_disp, screen_context_t screen_cxt, int usage);
	virtual void cleanup();

	void handleScreenEvent(bps_event_t *event);
	void handleNavigatorEvent(bps_event_t *event);

	int rotate(int angle);
	void update();
	void render();

private:
	void enable_2D();
	void enable_3D();


	// geometry definitions
	// cube
	static float cube_vertices[];
	static float cube_normals[];
	static float pyramid_vertices[];
	static float pyramid_normals[];
	static float sphere_vertices[];
	static float sphere_normals[];
	float obj_color[4];

	float obj_pos_x, obj_pos_y, obj_pos_z;
	float obj_scale_x, obj_scale_y, obj_scale_z;

	bool _autoRotate;
	QString* _touchMode;
	QString* _model;
	QString* _toolAxis;
	float obj_angle_pitch, obj_angle_yaw, obj_angle_roll;
	int currTouchX, currTouchY;
	int lastTouchX, lastTouchY;

	// shadow
	GLfloat shadow_vertices[8];
	GLfloat shadow_tex_coord[8];

	GLuint shadow;
	float shadow_pos_x, shadow_pos_y, shadow_size_x, shadow_size_y;

	// lights
	static GLfloat light_ambient[];
	static GLfloat light_diffuse[];
	static GLfloat light_pos[];
	static GLfloat light_direction[];

	// screens / windows
	float screenWidth, screenHeight;

	//static font_t* font;

	QString m_group;
	QString m_id;
	int m_x;
	int m_y;
	int m_width;
	int m_height;
};

#endif /* GOODCITIZEN_HPP */

