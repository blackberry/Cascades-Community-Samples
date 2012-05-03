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

#include "TwitterStreamRenderer.h"
#include "TwitterLiveData.h"
#include "bbutil.h"
#include "GLThread.h"
#include <QDebug>
#include <math.h>

namespace {
	int nPoints = 240;
    int tick=-60; // negative ==> delay to start anim...
    int selectedTeamSet=0; // as read from TwitterLiveData, stored to detect update
    const int numTeams=16;
    const int ticksPerScrollAnimation=600; //
    const int ticksForFadeInOut=20;
}

TwitterStreamRenderer::TwitterStreamRenderer(const QString &group, const QString id, int width, int height) :
						m_group(group), m_id(id), m_width(width), m_height(height)
{
}

TwitterStreamRenderer::~TwitterStreamRenderer() {
	// TODO Auto-generated destructor stub
}

int TwitterStreamRenderer::initGL() {
//	qDebug() << " TwitterStreamGraphThread initGL ";

	GLThread::instance()->resize(m_width,m_height);

	// we call this here because run() method is too late...
	m_screen_win = bbutil_get_window();
	int angle = 42;
    int rc = screen_get_window_property_iv(m_screen_win, SCREEN_PROPERTY_ROTATION, &angle);
    if (rc) {
        perror("screen_set_window_property_iv");
    }

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

    //Initialize GL for 2D rendering
	glViewport(0, 0, surface_width, surface_height);
	qDebug() << __LINE__ <<"=" << glGetError();
	glMatrixMode(GL_PROJECTION);
	qDebug() << __LINE__ <<"=" << glGetError();
	glLoadIdentity();
	//glOrthof(0.0f, (float) (surface_width) / (float) (surface_height), 0.0f, 1.0f, -1.0f, 1.0f);
	//glOrthof(0.0f, (float) (surface_width) , 0, (float) (surface_height), 0.0f, 1.0f);
	glOrthof(1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// compensate for 1280x1280 control size instead of 1280x768
	// makes drawing (antialiasing) much nicer if the control size is square.
	if (angle==90){
		glTranslatef((1280.0f-768.0f)/1280.0f,0,0);
	}
    glScalef(768.0f/1280.0f, 1.0f, 1.0f);

	// this flips xy, so y is vertical?
	// also makes the x (now horizontal) range [0:1]
	// and vertical range [-1:1]
	if (angle==270) {
		GLfloat flipXY[] = {
					   0.0f,  1.0f,  0.0f,  0.0f,
					  -0.5f,  0.0f,  0.0f,  0.0f,
					   0.0f,  0.0f,  1.0f,  0.0f,
					   0.5f,  0.0f,  0.0f,  1.0f
		};
		glMultMatrixf(flipXY);
	} else if (angle==90) {
		GLfloat flipXY[] = {
					   0.0f, -1.0f,  0.0f,  0.0f,
					   0.5f,  0.0f,  0.0f,  0.0f,
					   0.0f,  0.0f,  1.0f,  0.0f,
					   0.5f,  1.0f,  0.0f,  1.0f
		};
		glMultMatrixf(flipXY);
	}


    // FIXME : TODO REMOVE this when our screen will be sized and positioned
    // correction for full screen overlay:
    const float offset=0.48f;
    glTranslatef(offset,0.0f, 0.0f);
    glScalef(1-offset, 1.0f, 1.0f);
    // correction for 14 pixel offset wrt to tetam_wheel axis
    glTranslatef(0.0f,-14.0f/768, 0.0f);


    //Set clear color to a shade of black for good looks
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	return EXIT_SUCCESS;
}

void TwitterStreamRenderer::render() {
	tick++; // this is the clock (render count) for the animations

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (tick<0){ // if the animation start to quickly thq qml brother are not yet present
		// skip the drawing for this tick
    	bbutil_swap();
    	return;
	}

	// team swapping code.
	if (selectedTeamSet!=TwitterLiveData::teamSetIndex){
		selectedTeamSet=TwitterLiveData::teamSetIndex;
		// restart anim - forget the transition out, we get transition in.
		tick = 0;
	}

	int t1 = selectedTeamSet*2;
	int t2 = selectedTeamSet*2+1;

	int num_bars=30;//nPoints-60;
	int i,j;
	float m1=1,m2=1;
    for (i = 0; i < nPoints; i++) {
    	// this scale both teams independantly
    	if (TwitterLiveData::data(t1)[i]>m1) {m1=TwitterLiveData::data(t1)[i];}
    	if (TwitterLiveData::data(t2)[i]>m2) {m2=TwitterLiveData::data(t2)[i];}
    }
    // scale up a little
	m1/=1.5f;
	m2/=1.5f;

    float flutterBlend=1;
    if (tick%ticksPerScrollAnimation < ticksForFadeInOut){
    	float blend=(tick%ticksPerScrollAnimation)/(float)ticksForFadeInOut;
    	m1/=blend;
    	m2/=blend;
    } else if (tick%ticksPerScrollAnimation > ticksPerScrollAnimation-ticksForFadeInOut){
    	float blend=(ticksPerScrollAnimation-(tick%ticksPerScrollAnimation))/(float)ticksForFadeInOut;
    	m1/=blend;
    	m2/=blend;
    } else {
    	// flutter
    	int flutter = (tick%ticksPerScrollAnimation)-ticksForFadeInOut;
    	float period = ticksPerScrollAnimation-2*ticksForFadeInOut;
    	float fluttersPerPeriod=14;
    	float blend = 1-0.2f*sin( flutter/period * fluttersPerPeriod  *2*M_PI );
    	flutterBlend=blend;
    }

    // interpolation factor
    int ifactor=5;
    GLfloat topstrip[num_bars*ifactor*4];
    GLfloat *tp=topstrip;
    GLfloat botstrip[num_bars*ifactor*4];
    GLfloat *bp=botstrip;
    for (i = 0; i < num_bars; i++) {
        for (j=0;j<ifactor;j++){
        	float offset = (tick%ticksPerScrollAnimation)/(float)ticksPerScrollAnimation*(nPoints-num_bars);
        	float x = i+(float)j/ifactor;
        	float spatialFlutterBlend=flutterBlend*(1+.2f*cos(x/num_bars*2*M_PI*3));

        	float y1 =  TwitterLiveData::spline(t1,x+offset)/m1*spatialFlutterBlend;
        	float y2 = TwitterLiveData::spline(t2,x+offset)/m2*spatialFlutterBlend;

        	float ymid=-(y1-y2)/2.0f; // offset to center stream

        	// now setup the strips vertex arrays
    		*tp++= x/num_bars;
    		*tp++= ymid;
    		*tp++= x/num_bars;
    		*tp++= y1 + ymid;

    		*bp++= x/num_bars;
    		*bp++= ymid;
    		*bp++= x/num_bars;
    		*bp++= -y2 + ymid ;

        }
    }

 	glEnableClientState(GL_VERTEX_ARRAY);

	const float *c1 = TwitterLiveData::color()[t1];
	const float *c2 = TwitterLiveData::color()[t2];

	GLfloat xmargin=0.05f;
	GLfloat ymargin=0.05f;
    glPushMatrix();

    glTranslatef(xmargin,0, 0.0f);
    glScalef(1-2*xmargin,1-2*ymargin, 1.0f);

    glColor4f(c1[0],c1[1],c1[2], 0.8);
	glVertexPointer(2, GL_FLOAT, 0, topstrip);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, num_bars*ifactor*2);

    glColor4f(c2[0],c2[1],c2[2], 0.8);
	glVertexPointer(2, GL_FLOAT, 0, botstrip);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, num_bars*ifactor*2);

    glPopMatrix();


	glDisableClientState(GL_VERTEX_ARRAY);

	//Use utility code to update the screen
	bbutil_swap();
}

void TwitterStreamRenderer::screenEvent(bps_event_t *event) {
	screen_event_t screen_event = screen_event_get_event(event);

	int screen_val;
	screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE, &screen_val);

	switch (screen_val) {
	case SCREEN_EVENT_MTOUCH_TOUCH:
	case SCREEN_EVENT_MTOUCH_RELEASE:
		break;
	case SCREEN_EVENT_MTOUCH_MOVE:
		//glRotatef(0.8f, 0.8f, 1.0f, 0.0f);
		break;
	}
}

void TwitterStreamRenderer::initialize() {
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

void TwitterStreamRenderer::shutdown()
{
    int vis = 0;
    screen_set_window_property_iv(m_screen_win, SCREEN_PROPERTY_VISIBLE, &vis);
}

screen_window_t TwitterStreamRenderer::getWindow()
{
	return m_screen_win;
}
