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

#include "MinutesPerGameRenderer.h"
#include "bbutil.h"
#include "GLThread.h"
#include <QDebug>
#include <Math.h>

MinutesPerGameRenderer::MinutesPerGameRenderer(const QString &group, const QString id, int width, int height) :
						m_group(group), m_id(id), m_width(width), m_height(height)
{
	m_startPos = 1;

	createScene();
}

MinutesPerGameRenderer::~MinutesPerGameRenderer() {
}

int MinutesPerGameRenderer::initGL() {

    GLThread::instance()->resize(m_width,m_height);

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

	glOrthof(0.0f, (float) (surface_width) / (float) (surface_height), 0.0f, 1.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef((float) (surface_width) / (float) (surface_height) / 2, 0.5f, 0.5f);

	glScalef(0.5f, 0.5f, 0.0f);

	return EXIT_SUCCESS;
}

GLfloat* MinutesPerGameRenderer::drawCircle(int sides, GLfloat width, GLfloat height)
{
	int count = (sides+1)*2;
	GLfloat* vertices = new GLfloat[count];
	vertices[0] = 0.0f;
	vertices[1] = 0.0f;
	vertices[2] = width;	//force the next point to be 90deg from the center
	vertices[3] = 0.0f;
	for (int i = 4; i < count/2; i+=2) {
		vertices[i] = width * cosf(i*2*(M_PI)/sides);
		vertices[i+1] = height * sinf(i*2*(M_PI)/sides);
	}
	return vertices;
}

void MinutesPerGameRenderer::drawTeamCircles(int sides, GLfloat width, GLfloat height)
{
	int vertices = sides*2;  //vertices is double the sides (x,y)
	int extras = 4;  		//add 3 extra vertices for 0,0 to 0,1 and back to 0,0 at the end
	int buffersize = (vertices/2)+(extras*2);	//half a circle
	qDebug() << "vertices=" << vertices << "buffersize=" << buffersize;

	m_teamACircle = new GLfloat[buffersize];
	m_teamBCircle = new GLfloat[buffersize];

	//fill 0,0 and 0,1
	m_teamACircle[0] = 0.0f;
	m_teamACircle[1] = 0.0f;
	m_teamACircle[2] = width;	//force the next point to be 90deg from the center
	m_teamACircle[3] = 0.0f;
	m_teamACircle[buffersize-4] = -width;
	m_teamACircle[buffersize-3] = 0.0f;
	m_teamACircle[buffersize-2] = 0.0f;
	m_teamACircle[buffersize-1] = 0.0f;

	m_teamBCircle[0] = 0.0f;
	m_teamBCircle[1] = 0.0f;
	m_teamBCircle[2] = -width;	//opposite
	m_teamBCircle[3] = 0.0f;
	m_teamBCircle[buffersize-4] = width;
	m_teamBCircle[buffersize-3] = 0.0f;
	m_teamBCircle[buffersize-2] = 0.0f;
	m_teamBCircle[buffersize-1] = 0.0f;

	GLfloat * pTeamA = m_teamACircle+4;
	GLfloat * pTeamB = m_teamBCircle+4;
	for (int i = 0; i < vertices; i+=2)
	{
		if(i<vertices/2)
		{
			pTeamA[i] = width * cosf(i*(M_PI)/sides);
			pTeamA[i+1] = height * sinf(i*(M_PI)/sides);
		}
		else
		{
			//qDebug() << "i,vertices,[i-vertices]=" <<i<<","<<vertices/2<<","<< i-(vertices/2);
			int x = i-(vertices/2);
			int y = x+1;
			pTeamB[x] = width * cosf(i*(M_PI)/sides);
			pTeamB[y] = height * sinf(i*(M_PI)/sides);
			//qDebug() << "2. [" << x << "]" << "[" <<pTeamB[x]<<","<<pTeamB[y]<<"]";
		}
	}
}

void MinutesPerGameRenderer::circleGradient(int sides)
{
	int count = sides*4;
	m_teamAColors = new GLfloat[count/2];
	m_teamBColors = new GLfloat[count/2];
	m_teamAColors[0] = 0.0f;
	m_teamAColors[1] = 0.0f;
	m_teamAColors[2] = 0.0f;
	m_teamAColors[3] = 1.0f;

	// 270-360deg is just gray bar (25%)
	for(int i=4; i<96; i+=4)
	{
		//RGB(96,97,99)
		m_teamAColors[i] = 0.376f;
		m_teamAColors[i+1] = 0.380f;
		m_teamAColors[i+2] = 0.388f;
		m_teamAColors[i+3] = 1.0;
	}

	//start from RGB(133,42,51)
	float r=0.521;
	float g=0.164;
	float b=0.2;
	float inc1=0.0235; //increment by 6 until white

	//0-270deg is shaded white to red. (25-100%)
	for(int i=96; i<376; i+=8)
	{
		for(int j=0; j<8; j+=4)
		{
			m_teamAColors[i+j] = r;
			m_teamAColors[i+1+j] = g;
			m_teamAColors[i+2+j] = b;
			m_teamAColors[i+3+j] = 1.0f;
		}
		r+=inc1;
		g+=inc1;
		b+=inc1;
	}

	for(int i=(count/2)-1,j=0; i>=0; i--,j++)
	{
		//qDebug() << "[" << i << "," << j <<"]";
		m_teamBColors[j] = m_teamAColors[i];
	}
	m_teamBColors[0] = 0.0f;
	m_teamBColors[1] = 0.0f;
	m_teamBColors[2] = 0.0f;
	m_teamBColors[3] = 1.0f;

	for(int i=280; i<376; i+=4)
	{
		//RGB(96,97,99)
		m_teamBColors[i] = 0.376f;
		m_teamBColors[i+1] = 0.380f;
		m_teamBColors[i+2] = 0.388f;
		m_teamBColors[i+3] = 1.0;
	}
}

void MinutesPerGameRenderer::createScene()
{
	drawTeamCircles(180,0.7f,0.7f);
	m_innerCircle = drawCircle(358,0.5f,0.5f);
	circleGradient(188);
}

void MinutesPerGameRenderer::render()
{
    if(m_startPos>94)
        return;

	glClear(GL_COLOR_BUFFER_BIT);

	//draw team a
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, m_teamACircle);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, m_teamAColors);
	//glColor4f(1.0, 0.0, 0.0, 1.0);

	glDrawArrays(GL_TRIANGLE_FAN, 0, m_startPos);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	//draw team b
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, m_teamBCircle);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, m_teamBColors);
	//glColor4f(0.0, 1.0, 0.0, 1.0);

	glDrawArrays(GL_TRIANGLE_FAN, 0, m_startPos);
	m_startPos++;

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	//inner black thing
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, m_innerCircle);

	glColor4f(0.0, 0.0, 0.0, 1.0);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 180);

	glDisableClientState(GL_VERTEX_ARRAY);

	GLfloat separator [] = {
			0.0f,0.0f,0.7f,0.0f,
			0.0f,0.0f,-0.7f,0.0f
	};
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, separator);
	glPointSize(5.0f);
	glColor4f(0.0, 0.0, 0.0, 1.0);
	glDrawArrays(GL_LINE_STRIP, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);


	//Use utility code to update the screen
	bbutil_swap();
}

void MinutesPerGameRenderer::screenEvent(bps_event_t *event)
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

void MinutesPerGameRenderer::initialize() {
	m_startPos = 1;

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

	int pos[2] = { 280, 0 };
	screen_set_window_property_iv(getWindow(), SCREEN_PROPERTY_POSITION, pos);
	screen_flush_context(GLThread::instance()->getContext(), SCREEN_WAIT_IDLE);
}

void MinutesPerGameRenderer::shutdown()
{
	int pos[2] = { 0, 0 };
	screen_set_window_property_iv(getWindow(), SCREEN_PROPERTY_POSITION, pos);

	int vis = 0;
    screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_VISIBLE, &vis);
}

screen_window_t MinutesPerGameRenderer::getWindow()
{
	return m_screen_win;
}
