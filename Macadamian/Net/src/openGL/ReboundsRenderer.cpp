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

#include "ReboundsRenderer.h"
#include "bbutil.h"
#include "GLThread.h"
#include <QDebug>
#include "Matchups.h"

class Point {

public:
    GLfloat x, y;

    Point(GLfloat x2, GLfloat y2) : x(x2), y(y2)
    {

    }

    void setxy(GLfloat x2, GLfloat y2)
    {
    	x = x2;
     	y = y2;
    }

    const Point & operator=(const Point &rPoint) {
    	x = rPoint.x;
    	y = rPoint.y;

    	return *this;
    }

};



ReboundsRenderer::ReboundsRenderer(const QString &group, const QString id, int width, int height)
: m_group(group)
, m_id(id)
, m_width(width)
, m_height(height)
{
	m_animIndex = 0;
}

ReboundsRenderer::~ReboundsRenderer() {
}

int ReboundsRenderer::initGL()
{
    GLThread::instance()->resize(m_width,m_height);

    // we call this here because run() method is too late...
	m_screen_win = bbutil_get_window();
	int angle = 42;
    int rc = screen_get_window_property_iv(m_screen_win, SCREEN_PROPERTY_ROTATION, &angle);
    if (rc) {
        perror("screen_set_window_property_iv");
    }
    qDebug() << " angle is " + angle;
    //Query width and height of the window surface created by utility code
	EGLint surface_width, surface_height;

	eglQuerySurface(egl_disp, egl_surf, EGL_WIDTH, &surface_width);
	eglQuerySurface(egl_disp, egl_surf, EGL_HEIGHT, &surface_height);

	EGLint err = eglGetError();
	if (err != 0x3000) {
		fprintf(stderr, "Unable to query EGL surface dimensions\n");
		return EXIT_FAILURE;
	}

	glShadeModel(GL_SMOOTH);
	qDebug() << __LINE__ <<"=" << glGetError();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	qDebug() << __LINE__ <<"=" << glGetError();

	glViewport(0, 0, surface_width, surface_height);
	qDebug() << __LINE__ <<"=" << glGetError();
	glMatrixMode(GL_PROJECTION);
	qDebug() << __LINE__ <<"=" << glGetError();
	glLoadIdentity();

	glOrthof(1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (angle==270) {
		GLfloat flipXY[] = {
					   0.0f,  1.0f,  0.0f,  0.0f,
					  -1.0f,  0.0f,  0.0f,  0.0f,
					   0.0f,  0.0f,  1.0f,  0.0f,
					   1.0f,  0.0f,  0.0f,  1.0f
		};
		glMultMatrixf(flipXY);
	} else if (angle==90) {
		GLfloat flipXY[] = {
					   0.0f, -1.0f,  0.0f,  0.0f,
					   1.0f,  0.0f,  0.0f,  0.0f,
					   0.0f,  0.0f,  1.0f,  0.0f,
					   0.0f,  1.0f,  0.0f,  1.0f
		};
		glMultMatrixf(flipXY);
	}

	return EXIT_SUCCESS;
}

void ReboundsRenderer::render() {
	if (m_animIndex < MAX_FRAMES)
	{
		m_animIndex++;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	for (int i = 0; i < 10; i++)
	{
		GLfloat* vertices = getGeometry(i);
		glVertexPointer(2, GL_FLOAT, 0, vertices);

		GLfloat* colours = getColours();
		glColorPointer(4, GL_FLOAT, 0, colours);

		glDrawArrays(GL_LINE_STRIP, 0, m_animIndex); // last digit here is the number of vertices we have
	}

	for (int i = 0; i < 2; i++)
	{
		GLfloat* vertices = getLegendGeometry(i);
		glVertexPointer(2, GL_FLOAT, 0, vertices);

		GLfloat* colours = getLegendColours(i);
		glColorPointer(4, GL_FLOAT, 0, colours);

		glDrawArrays(GL_LINE_STRIP, 0, 2);
	}


	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	//Use utility code to update the screen
	bbutil_swap();
}

void ReboundsRenderer::screenEvent(bps_event_t *event)
{
	screen_event_t screen_event = screen_event_get_event(event);

	int screen_val;
	screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE, &screen_val);

	switch (screen_val) {
	case SCREEN_EVENT_MTOUCH_TOUCH:
	case SCREEN_EVENT_MTOUCH_RELEASE:
		break;
	case SCREEN_EVENT_MTOUCH_MOVE:
		break;
	}
}

GLfloat* ReboundsRenderer::getGeometry(int index)
{
	GLfloat* vertices = new GLfloat[164];

	int arrayIndex = 0;

	int segments = 40;


	data::Player player = m_teamAStarters[index % 5];

	if (index >= 5)
	{
		player = m_teamBStarters[index % 5];
	}
	/*
	 * MAGIC Numbers for layout of Graphs
	 * const static GLfloat LEFT = 0.104f;
	 * const static GLfloat RIGHT = 0.883f;
	 * const static GLfloat BOTTOM = 0.580f;
     *
	 * const static GLfloat X_INC = 0.008884;
	 * const static GLfloat Y_INC = 0.0554;
	 */

	// draw the offensive lines

	Point p0 = Point(LEFT + (X_INC * player.averageAssists()), BOTTOM);
	Point p1 = Point(LEFT + (X_INC * player.averageAssists()), BOTTOM + (Y_INC * (index + 1)));
	Point p2 = Point(LEFT, BOTTOM + (Y_INC * (index + 1)));

	if (index >= 5)
	{
		int offset = index - 4;

		p0 = Point(RIGHT - (X_INC * player.averageAssists()), BOTTOM);
		p1 = Point(RIGHT - (X_INC * player.averageAssists()), BOTTOM + (Y_INC * offset));
		p2 = Point(RIGHT, BOTTOM + (Y_INC * offset));
	}


	for (int i = 0; i <= segments; i++)
	{
		GLfloat t = i / ((GLfloat) segments);
		GLfloat u = 1 - t;
		GLfloat tt = t*t;
		GLfloat uu = u*u;

		// draw point .x
		vertices[arrayIndex] = uu * p0.x + 2 * u * t * p1.x + tt * p2.x;
		arrayIndex++;
		// draw point .y
		vertices[arrayIndex] = uu * p0.y + 2 * u * t * p1.y + tt * p2.y;
		arrayIndex++;
	}

	// draw the defensive lines

	p0 = Point(LEFT, BOTTOM + (Y_INC * (index + 1)));
	p1 = Point(LEFT + (X_INC * player.averageRebounds()), BOTTOM + (Y_INC * (index + 1)));
	p2 = Point(LEFT + (X_INC * player.averageRebounds()), BOTTOM);

	if (index >= 5)
	{
		int offset = index - 4;

		p0 = Point(RIGHT, BOTTOM + (Y_INC * offset));
		p1 = Point(RIGHT - (X_INC * player.averageRebounds()), BOTTOM + (Y_INC * offset));
		p2 = Point(RIGHT - (X_INC * player.averageRebounds()), BOTTOM);
	}


	for (int i = 0; i <= segments; i++)
	{
		GLfloat t = i / ((GLfloat) segments);
		GLfloat u = 1 - t;
		GLfloat tt = t*t;
		GLfloat uu = u*u;

		// draw point .x
		vertices[arrayIndex] = uu * p0.x + 2 * u * t * p1.x + tt * p2.x;
		arrayIndex++;
		// draw point .y
		vertices[arrayIndex] = uu * p0.y + 2 * u * t * p1.y + tt * p2.y;
		arrayIndex++;
	}

	return vertices;
}

GLfloat* ReboundsRenderer::getColours()
{
	GLfloat* colours = new GLfloat[328];

	int segments = 40;

	//use RGB(133,42,51)
	float r=0.521;
	float g=0.164;
	float b=0.2;

	for(int i = 0; i <= segments * 4; i+=4)
	{
		colours[i] = r;
		colours[i+1] = g;
		colours[i+2] = b;
		colours[i+3] = 1.0f;
	}

	// make colour 2 for the lines coming down

	//use RGB(245,128,37)
	r=0.961;
	g=0.501;
	b=0.145;

	for(int i = 0; i <= segments * 4; i+=4)
	{
		colours[164+i] = r;
		colours[164+i+1] = g;
		colours[164+i+2] = b;
		colours[164+i+3] = 1.0f;
	}


	return colours;
}


GLfloat* ReboundsRenderer::getLegendGeometry(int index)
{
	GLfloat* vertices = new GLfloat[4];

	// x
	vertices[0] = 0.175f;
	// y
	vertices[1] = (index == 0) ? 0.535 : 0.510;

	// x
	vertices[2] = 0.225;
	// y
	vertices[3] = (index == 0) ? 0.535 : 0.510;

	return vertices;
}

GLfloat* ReboundsRenderer::getLegendColours(int index)
{
	GLfloat* colours = new GLfloat[8];

	if (index == 1)
	{
		for(int i = 0; i < 8; i+=4)
		{
			colours[i] = 0.961;
			colours[i+1] = 0.501;
			colours[i+2] = 0.145;
			colours[i+3] = 1.0f;
		}
	}
	else
	{
		for(int i = 0; i < 8; i+=4)
		{
			colours[i] = 0.521;
			colours[i+1] = 0.164;
			colours[i+2] = 0.2;
			colours[i+3] = 1.0f;
		}
	}

	return colours;
}


void ReboundsRenderer::initialize() {
	QByteArray groupArr = m_group.toAscii();
	QByteArray idArr = m_id.toAscii();

	// Initialize the BBS components.
	initGL();

	// We want to handle openGL Events in the foreign window. So bring the
	// in front of the cascades window (z > 0).
	m_screen_win = bbutil_get_window();
	int z = 1;
	screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_ZORDER, &z);
	screen_join_window_group(m_screen_win, groupArr.constData());
	screen_set_window_property_cv(m_screen_win, SCREEN_PROPERTY_ID_STRING, idArr.length(), idArr.constData());
	int vis = 1;
	screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_VISIBLE, &vis);
}

void ReboundsRenderer::setTeams(QString teamA, QString teamB)
{
	m_teamAStarters = data::Matchups::instance()->getTeam(data::Matchups::instance()->findByName(teamA))->getStartingLineup();
	m_teamBStarters = data::Matchups::instance()->getTeam(data::Matchups::instance()->findByName(teamB))->getStartingLineup();
}

void ReboundsRenderer::shutdown()
{
    int vis = 0;
    screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_VISIBLE, &vis);
}

screen_window_t ReboundsRenderer::getWindow()
{
	return m_screen_win;
}
