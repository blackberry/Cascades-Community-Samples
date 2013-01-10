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

#include "GoodCitizen.hpp"
//#include "bbutil.h"
#include <math.h>

#include <QDebug>

using namespace bb::cascades;

float GoodCitizen::cube_vertices[] = {
        // FRONT
        -2.0f, -2.0f,  2.0f,
         2.0f, -2.0f,  2.0f,
        -2.0f,  2.0f,  2.0f,
         2.0f,  2.0f,  2.0f,
        // BACK
        -2.0f, -2.0f, -2.0f, -2.0f, 2.0f, -2.0f, 2.0f, -2.0f,
        -2.0f,
        2.0f,
        2.0f,
        -2.0f,
        // LEFT
        -2.0f, -2.0f, 2.0f, -2.0f, 2.0f, 2.0f, -2.0f, -2.0f, -2.0f,
        -2.0f,
        2.0f,
        -2.0f,
        // RIGHT
        2.0f, -2.0f, -2.0f, 2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f, 2.0f,
        2.0f,
        2.0f,
        // TOP
        -2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f, 2.0f,
        -2.0f,
        // BOTTOM
        -2.0f, -2.0f, 2.0f, -2.0f, -2.0f, -2.0f, 2.0f, -2.0f, 2.0f, 2.0f, -2.0f,
        -2.0f, };

float GoodCitizen::cube_normals[] = {
        // FRONT
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f,
        1.0f,
        // BACK
        0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.0f,
        -1.0f,
        // LEFT
        -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.0f,
        // RIGHT
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        // TOP
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        // BOTTOM
        0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
        0.0f };


float GoodCitizen::pyramid_vertices[] = {
        // FRONT
        -2.0f, -2.0f,  2.0f,
         2.0f, -2.0f,  2.0f,
         0.0f,  2.0f,  0.0f,
        // BACK
         2.0f, -2.0f, -2.0f,
        -2.0f, -2.0f, -2.0f,
         0.0f,  2.0f,  0.0f,
        // LEFT
        -2.0f, -2.0f, -2.0f,
        -2.0f, -2.0f,  2.0f,
         0.0f,  2.0f,  0.0f,
        // RIGHT
         2.0f, -2.0f,  2.0f,
         2.0f, -2.0f, -2.0f,
         0.0f,  2.0f,  0.0f,
        // BOTTOM
        -2.0f, -2.0f, -2.0f,
         2.0f, -2.0f, -2.0f,
        -2.0f, -2.0f,  2.0f,
         2.0f, -2.0f,  2.0f,
         };

float GoodCitizen::pyramid_normals[] = {
        // FRONT
        0.0f,  0.6667f,  0.5f,
        0.0f,  0.6667f,  0.5f,
        0.0f,  0.6667f,  0.5f,
        0.0f,  0.6667f,  0.5f,
        // BACK
        0.0f,  0.6667f, -0.5f,
        0.0f,  0.6667f, -0.5f,
        0.0f,  0.6667f, -0.5f,
        0.0f,  0.6667f, -0.5f,
        // LEFT
       -0.5f,  0.6667f, 0.0f,
       -0.5f,  0.6667f, 0.0f,
       -0.5f,  0.6667f, 0.0f,
       -0.5f,  0.6667f, 0.0f,
        // RIGHT
        0.5f,  0.6667f, 0.0f,
        0.5f,  0.6667f, 0.0f,
        0.5f,  0.6667f, 0.0f,
        0.5f,  0.6667f, 0.0f,
        // BOTTOM
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f,
        0.0f, -1.0f, 0.0f
};


float GoodCitizen::sphere_vertices[3 * 128 * 16] = { 0.0 };

float GoodCitizen::sphere_normals[3 * 128 * 16] = { 0.0 };


GLfloat GoodCitizen::light_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat GoodCitizen::light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat GoodCitizen::light_pos[] = { 0.0f, 25.0f, 0.0f, 1.0f };
GLfloat GoodCitizen::light_direction[] = { 0.0f, 0.0f, -30.0f, 1.0f };


GoodCitizen::GoodCitizen(VIEW_DISPLAY display)
{
		setDisplay(display);

	    _toolAxis = new QString("X");
	    _touchMode = new QString("none");
	    _model = new QString("cube");
	    _autoRotate = true;

	    // touch event handling
		currTouchX = -1;
		currTouchY = -1;
		lastTouchX = -1;
		lastTouchY = -1;

		// setup sphere
		float phi = M_PI / 2.0;
		float theta = 0.0;
		float deltaPhi = M_PI / 16.0;
		float deltaTheta = M_PI / 16.0;

		int phistep = 0;
		int thetastep = 0;
		int vertexIndex = 0;
		int normalIndex = 0;

		// build sphere vertices
		for (phistep = 0; phistep < 16; phistep++) {
			phi = M_PI / 2.0 - deltaPhi*phistep;

			for (thetastep = 0; thetastep < 32; thetastep++) {
				theta = 0.0 + deltaTheta*thetastep;
				//qDebug()  << "build sphere: "<< thetastep << ":" << phistep << theta << ":" << phi;

				sphere_vertices[vertexIndex++] = 2.0 * cos(phi) * cos(theta);
				sphere_vertices[vertexIndex++] = 2.0 * cos(phi) * sin(theta);
				sphere_vertices[vertexIndex++] = 2.0 * sin(phi);

				sphere_vertices[vertexIndex++] = 2.0 * cos(phi) * cos(theta + deltaTheta);
				sphere_vertices[vertexIndex++] = 2.0 * cos(phi) * sin(theta + deltaTheta);
				sphere_vertices[vertexIndex++] = 2.0 * sin(phi);

				sphere_vertices[vertexIndex++] = 2.0 * cos(phi - deltaPhi) * cos(theta);
				sphere_vertices[vertexIndex++] = 2.0 * cos(phi - deltaPhi) * sin(theta);
				sphere_vertices[vertexIndex++] = 2.0 * sin(phi - deltaPhi);

				sphere_vertices[vertexIndex++] = 2.0 * cos(phi - deltaPhi) * cos(theta + deltaTheta);
				sphere_vertices[vertexIndex++] = 2.0 * cos(phi - deltaPhi) * sin(theta + deltaTheta);
				sphere_vertices[vertexIndex++] = 2.0 * sin(phi - deltaPhi);


				sphere_normals[normalIndex++] = cos(phi) * cos(theta);
				sphere_normals[normalIndex++] = cos(phi) * sin(theta);
				sphere_normals[normalIndex++] = sin(phi);

				sphere_normals[normalIndex++] = cos(phi) * cos(theta + deltaTheta);
				sphere_normals[normalIndex++] = cos(phi) * sin(theta + deltaTheta);
				sphere_normals[normalIndex++] = sin(phi);

				sphere_normals[normalIndex++] = cos(phi - deltaPhi) * cos(theta);
				sphere_normals[normalIndex++] = cos(phi - deltaPhi) * sin(theta);
				sphere_normals[normalIndex++] = sin(phi - deltaPhi);

				sphere_normals[normalIndex++] = cos(phi - deltaPhi) * cos(theta + deltaTheta);
				sphere_normals[normalIndex++] = cos(phi - deltaPhi) * sin(theta + deltaTheta);
				sphere_normals[normalIndex++] = sin(phi - deltaPhi);

				//qDebug()  << "build sphere: "<< thetastep << ":" << phistep << ":" << theta << ":" << phi << ":" <<
				//		(2.0 * cos(phi) * cos(theta)) << ":" <<	(2.0 * cos(phi) * sin(theta)) << ":" <<  2.0 * sin(phi);
			}
		}
		//qDebug()  << "build sphere: "<< vertexIndex << ":" << normalIndex;
}

GoodCitizen::~GoodCitizen() {
	// TODO Auto-generated destructor stub
}

int GoodCitizen::initialize() {

	setZ(-5);

	int returnCode = OpenGLView::initGL();
	if (returnCode == EXIT_SUCCESS) {
		//Load shadow textures
		float tex_x = 1.0f, tex_y = 1.0f;
		if (EXIT_SUCCESS
				!= loadTexture("app/native/assets/images/goodcitizen/shadow.png", NULL, NULL,
						&tex_x, &tex_y, &shadow)) {
			fprintf(stderr, "Unable to load shadow texture\n");
		}

		reset(false);

		// reset stale flag for initialization only
		setStale(false);

		//Common gl setup
		glShadeModel(GL_SMOOTH);
		glClearColor(0.775f, 0.775f, 0.775f, 1.0f);
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
		glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);

		glEnable(GL_CULL_FACE);
	}

	qDebug()  << "GoodCitizen::initialize ";

    return returnCode;
}

int GoodCitizen::regenerate()
{
	int returnCode = OpenGLView::regenerate();

	reset(true);

	return returnCode;
}

void GoodCitizen::cleanup() {
	OpenGLView::cleanup();
}

void GoodCitizen::render() {
	//Typical render pass
    glClearColor(0.775f, 0.775f, 0.775f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//First render background and menu if it is enabled
	enable_2D();

	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glVertexPointer(2, GL_FLOAT, 0, shadow_vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, shadow_tex_coord);
	glBindTexture(GL_TEXTURE_2D, shadow);

	//glColor4f(0.85f, 0.85f, 0.85f, 1.0f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_TEXTURE_2D);

	//Then render the cube
	enable_3D();
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);

	glTranslatef(obj_pos_x, obj_pos_y, obj_pos_z);

	glScalef(obj_scale_x, obj_scale_y, obj_scale_z);

	glRotatef(obj_angle_roll, 1.0f, 0.0f, 0.0f);
	glRotatef(obj_angle_yaw, 0.0f, 0.0f, 1.0f);
	glRotatef(obj_angle_pitch, 0.0f, 1.0f, 0.0f);

	glColor4f(obj_color[0], obj_color[1], obj_color[2], obj_color[3]);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	if (!_model->compare("cube")) {
		glVertexPointer(3, GL_FLOAT, 0, cube_vertices);
		glNormalPointer(GL_FLOAT, 0, cube_normals);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
		glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
		glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
		glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
		glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
	} else
	if (!_model->compare("pyramid")) {
		glVertexPointer(3, GL_FLOAT, 0, pyramid_vertices);
		glNormalPointer(GL_FLOAT, 0, pyramid_normals);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
		glDrawArrays(GL_TRIANGLE_STRIP, 3, 3);
		glDrawArrays(GL_TRIANGLE_STRIP, 6, 3);
		glDrawArrays(GL_TRIANGLE_STRIP, 9, 3);
		glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
	} else
	if (!_model->compare("sphere")) {
		glVertexPointer(3, GL_FLOAT, 0, sphere_vertices);
		glNormalPointer(GL_FLOAT, 0, sphere_normals);

		int phistep = 0;
		int thetastep = 0;

		for (phistep = 0; phistep < 16; phistep++) {
			for (thetastep = 0; thetastep < 32; thetastep++) {
				glDrawArrays(GL_TRIANGLE_STRIP, phistep*128+thetastep*4, 4);
			}
		}
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_COLOR_MATERIAL);
	glDisable(GL_DEPTH_TEST);

	// after rendering this view is no longer stale
	setStale(false);
}

// properties and slots
QVariantList& GoodCitizen::objectColor() {

	m_viewMutex.lock();

	QVariantList* objectColor = new QVariantList();
	*objectColor << obj_color[0] << obj_color[1] << obj_color[2] << obj_color[3];

	m_viewMutex.unlock();

	return *objectColor;
}

void GoodCitizen::setObjectColor(QVariantList color) {

	m_viewMutex.lock();

	obj_color[0] = color[0].value<float>();
	obj_color[1] = color[1].value<float>();
	obj_color[2] = color[2].value<float>();
	obj_color[3] = color[3].value<float>();

	m_viewMutex.unlock();

	setStale(true);
}

QString GoodCitizen::toolAxis() {
	QString *axis;

	m_viewMutex.lock();

	axis = new QString(*_toolAxis);

	m_viewMutex.unlock();

	return *axis;
}

QString GoodCitizen::touchMode() {
	QString *mode;

	m_viewMutex.lock();

	mode = new QString(*_touchMode);

	m_viewMutex.unlock();

	return *mode;
}

QString GoodCitizen::model() {
	QString *model;

	m_viewMutex.lock();

	model = new QString(*_model);

	m_viewMutex.unlock();

	return *model;
}


void GoodCitizen::setToolAxis(QString toolAxis) {

	m_viewMutex.lock();

	delete _toolAxis;

	_toolAxis = new QString(toolAxis);

	m_viewMutex.unlock();

	setStale(true);
}

void GoodCitizen::setTouchMode(QString mode) {

	m_viewMutex.lock();

	delete _touchMode;

	_touchMode = new QString(mode);

	m_viewMutex.unlock();

	setStale(true);
}

void GoodCitizen::setModel(QString model) {

	m_viewMutex.lock();

	delete _model;

	_model = new QString(model);

	m_viewMutex.unlock();

	setStale(true);
}


void GoodCitizen::enable_2D() {
    glViewport(0, 0, m_width, m_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrthof(0.0f, (float)m_width / (float)m_height, 0.0f, 1.0f, -1.0f, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0f / (float)m_height, 1.0f / (float)m_height, 1.0f);
}

void GoodCitizen::enable_3D() {
    glViewport(0, 0, m_width, m_height);

    GLfloat aspect_ratio = (float)m_width / (float)m_height;

    GLfloat fovy = 45;
    GLfloat zNear = 1.0f;
    GLfloat zFar = 1000.0f;

    GLfloat top = tan(fovy * 0.0087266462599716478846184538424431f) * zNear;
    GLfloat bottom = -top;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glFrustumf(aspect_ratio * bottom, aspect_ratio * top, bottom, top, zNear,
            zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void GoodCitizen::reset(bool skipColour) {
    float tex_x = 1.0f, tex_y = 1.0f;

    obj_pos_x = 0.0f;
    obj_pos_x = 0.0f;
    obj_pos_x = 0.0f;

    if (m_width > m_height) {
        shadow_pos_x = 365.0f;
        shadow_pos_y = 0.0f;

        obj_pos_x = 2.9f;
        obj_pos_y = 0.3f;
        obj_pos_z = -20.0f;
    } else {
        shadow_pos_x = 70.0f;
        shadow_pos_y = 10.0f;

        obj_pos_x = 0.5f;
        obj_pos_y = 0.1f;
        obj_pos_z = -30.0f;
    }

    shadow_size_x = (float) 512;
    shadow_size_y = (float) 256;

    shadow_tex_coord[0] = 0.0f;
    shadow_tex_coord[1] = 0.0f;
    shadow_tex_coord[2] = tex_x;
    shadow_tex_coord[3] = 0.0f;
    shadow_tex_coord[4] = 0.0f;
    shadow_tex_coord[5] = tex_y;
    shadow_tex_coord[6] = tex_x;
    shadow_tex_coord[7] = tex_y;

    obj_angle_yaw = 0.0f;
    obj_angle_pitch = 30.0f;
    obj_angle_roll = 15.0f;

    obj_scale_x = 1.0f;
    obj_scale_y = 1.0f;
    obj_scale_z = 1.0f;

    if (!skipColour) {
		obj_color[0] = 1.0f;
		obj_color[1] = 0.0f;
		obj_color[2] = 0.0f;
		obj_color[3] = 1.0f;
    }

    shadow_vertices[0] = shadow_pos_x;
    shadow_vertices[1] = shadow_pos_y;
    shadow_vertices[2] = shadow_pos_x + shadow_size_x;
    shadow_vertices[3] = shadow_pos_y;
    shadow_vertices[4] = shadow_pos_x;
    shadow_vertices[5] = shadow_pos_y + shadow_size_y;
    shadow_vertices[6] = shadow_pos_x + shadow_size_x;
    shadow_vertices[7] = shadow_pos_y + shadow_size_y;

	setStale(true);
}

void GoodCitizen::update() {
	if (_autoRotate) {
		if (!_toolAxis->compare("X")) {
			obj_angle_roll = fmod((obj_angle_roll + 1.0f), 360.0 );

			setStale(true);
		} else
		if (!_toolAxis->compare("Y")) {
			obj_angle_pitch = fmod((obj_angle_pitch + 1.0f), 360.0 );

			setStale(true);
		} else
		if (!_toolAxis->compare("Z")) {
			obj_angle_yaw = fmod((obj_angle_yaw + 1.0f), 360.0 );

			setStale(true);
		}
	}
}

void GoodCitizen::onTouch(bb::cascades::TouchEvent *event) {

	if (event->isDown()) {
		if (!_touchMode->compare("rotate")) {
			_autoRotate = false;

			setStale(true);
		}
		lastTouchX = lastTouchY = -1;

		qDebug() << "touch down: " << _touchMode->toLatin1() << " : " << event->screenX() << " : " << event->screenY();
	}
	if (event->isUp()) {
		if (!_touchMode->compare("rotate")) {
		    _autoRotate = true;

			setStale(true);
		}
		lastTouchX = lastTouchY = -1;

		qDebug() << "touch up: " << _touchMode->toLatin1() << " : " << event->screenX() << " : " << event->screenY();
	}
	if (event->isMove()) {
		float currTouchX = event->screenX();
		float currTouchY = event->screenY();

		float moveX = currTouchX - lastTouchX;
		float moveY = -(currTouchY - lastTouchY);

		qDebug() << "move: " << _touchMode->toLatin1() << " : " << moveX << " : " << moveY;

		if (!_touchMode->compare("scale")) {
			if (lastTouchX != -1) {
				float deltaX = moveX / (100.0f);
				float deltaY = moveY / (100.0f);

				float deltaAxis = sqrt(deltaX*deltaX + deltaY*deltaY);
				if (deltaY < 0) {
					deltaAxis *= -1;
				}

				if (!_toolAxis->compare("X")) {
					obj_scale_x += deltaAxis;
					if (obj_scale_x < 0) {
						obj_scale_x = 0;
					}
					//qDebug() << "x: " << deltaAxis << " : " << obj_scale_x;

					setStale(true);
				} else
				if (!_toolAxis->compare("Y")) {
					obj_scale_y += deltaAxis;
					if (obj_scale_y < 0) {
						obj_scale_y = 0;
					}
					//qDebug() << "y: " << deltaAxis << " : " << obj_scale_y;

					setStale(true);
				} else
				if (!_toolAxis->compare("Z")) {
					obj_scale_z += deltaAxis;
					if (obj_scale_z < 0) {
						obj_scale_z = 0;
					}
					//qDebug() << "z: " << deltaAxis << " : " << obj_scale_z;

					setStale(true);
				}
			}
		} else
		if (!_touchMode->compare("translate")) {
			if (lastTouchX != -1) {
				float deltaX = moveX / (100.0f);
				float deltaY = moveY / (100.0f);

				float deltaAxis = sqrt(deltaX*deltaX + deltaY*deltaY);
				if (deltaY > 0 && !_toolAxis->compare("Z")) {
					deltaAxis *= -1;
				}
				if (deltaY < 0 && _toolAxis->compare("Z")) {
					deltaAxis *= -1;
				}

				if (!_toolAxis->compare("X")) {
					obj_pos_x += deltaAxis;
					//qDebug() << "x: " << deltaAxis << " : " << obj_pos_x << " : " << obj_pos_y << " : " << obj_pos_z;

					setStale(true);
				} else
				if (!_toolAxis->compare("Y")) {
					obj_pos_y += deltaAxis;
					//qDebug() << "y: " << deltaAxis << " : " << obj_pos_x << " : " << obj_pos_y << " : " << obj_pos_z;

					setStale(true);
				} else
				if (!_toolAxis->compare("Z")) {
					obj_pos_z += deltaAxis;
					//qDebug() << "z: " << deltaAxis << " : " << obj_pos_x << " : " << obj_pos_y << " : " << obj_pos_z;

					setStale(true);
				}

				shadow_pos_x = 70.0f + (22.5f * 100.0f / (10.0f - obj_pos_z) ) * (obj_pos_x) - 2.25f * (obj_pos_z);
		        shadow_pos_y = 10.0f  - 5.25f * (obj_pos_z);
		        shadow_size_x = (float) 512 + 3.75f * obj_pos_z + 2.25f * (obj_pos_y);
		        shadow_size_y = (float) 256 + 2.0f * obj_pos_z + 2.25f * (obj_pos_y);

				//qDebug() << "shadow: " << shadow_pos_x << " : " << shadow_pos_y << " : " << shadow_size_x << " : " << shadow_size_y;

				shadow_vertices[0] = shadow_pos_x;
				shadow_vertices[1] = shadow_pos_y;
				shadow_vertices[2] = shadow_pos_x + shadow_size_x;
				shadow_vertices[3] = shadow_pos_y;
				shadow_vertices[4] = shadow_pos_x;
				shadow_vertices[5] = shadow_pos_y + shadow_size_y;
				shadow_vertices[6] = shadow_pos_x + shadow_size_x;
				shadow_vertices[7] = shadow_pos_y + shadow_size_y;
			}
		} else
		if (!_touchMode->compare("rotate")) {

			float cosTheta, sinTheta, lastCosTheta, lastSinTheta;

			if (lastTouchX != -1) {

				//cosTheta = radius      / denominator;
				//sinTheta = deltaRadius / denominator;
				cosTheta = (currTouchX - 384.0f) / 384.0f;
				sinTheta = -(currTouchY - 640.0f) / 640.0f;
				lastCosTheta = (lastTouchX - 384.0f) / 384.0f;
				lastSinTheta = -(currTouchY - 640.0f) / 640.0f;

				float currAngle, lastAngle, deltaAngle;

				if (cosTheta != 0) {
					currAngle = acos(cosTheta) * 180.0 / M_PI;
					if (sinTheta < 0) {
						currAngle = 360.0f - currAngle;
					}
				} else {
					if (sinTheta < 0) {
						currAngle = 270.0f;
					} else {
						currAngle = 90.0f;
					}
				}

				if (lastCosTheta != 0) {
					lastAngle = acos(lastCosTheta) * 180.0 / M_PI;
					if (sinTheta < 0) {
						lastAngle = 360.0f - lastAngle;
					}
				} else {
					if (lastSinTheta < 0) {
						lastAngle = 270.0f;
					} else {
						lastAngle = 90.0f;
					}
				}

				deltaAngle = currAngle - lastAngle;

				if (_toolAxis->compare("X")) {
					obj_angle_roll = fmod((obj_angle_roll + deltaAngle), 360.0 );
					//qDebug() << "roll: " << currAngle << " : " << lastAngle << " : " << deltaAngle << " : " << obj_angle_roll;

					setStale(true);
				} else
				if (_toolAxis->compare("Y")) {
					obj_angle_pitch = fmod((obj_angle_pitch + deltaAngle), 360.0 );
					//qDebug() << "pitch: " << currAngle << " : " << lastAngle << " : " << deltaAngle << " : " << obj_angle_pitch;

					setStale(true);
				} else
				if (_toolAxis->compare("Z")) {
					obj_angle_yaw = fmod((obj_angle_yaw + deltaAngle), 360.0 );
					//qDebug() << "yaw: " << currAngle << " : " << lastAngle << " : " << deltaAngle << " : " << obj_angle_yaw;

					setStale(true);
				}

			}
		}

		lastTouchX = currTouchX;
		lastTouchY = currTouchY;
	}
}

void GoodCitizen::handleScreenEvent(bps_event_t *event) {
    int screenEvent;
    int buttons;
    int position[2];

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
