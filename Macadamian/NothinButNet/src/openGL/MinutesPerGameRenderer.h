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


#ifndef MINUTESPERGAMERENDERER_H_
#define MINUTESPERGAMERENDERER_H_
#include <assert.h>
#include <screen/screen.h>
#include <bps/navigator.h>
#include <bps/screen.h>
#include <bps/bps.h>
#include <bps/event.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <EGL/egl.h>
#include <GLES/gl.h>


#include "IGLRenderer.h"
#include <QString>

class MinutesPerGameRenderer : public IGLRenderer{
public:
    MinutesPerGameRenderer(const QString &group, const QString id, int width,int height);
	virtual ~MinutesPerGameRenderer();

	/*!
	 * @see IGLRenderer::initialize
	 */
	virtual void initialize();

    /*!
     * @see IGLRenderer::shutdown
     */
	virtual void shutdown();

    /*!
     * @see IGLRenderer::render
     */
	virtual void render();

    /*!
     * @see IGLRenderer::screenEvent
     */
	virtual void screenEvent(bps_event_t *event);

	screen_window_t getWindow();

protected:
    /*!
     * Creates an array of vertices forming a circle.
     *
     * @param sides - the number of vertices in the circle. higher=smoother
     * @param width - floating point value between 0.0-1.0 representing the width
     * @param height - floating point value between 0.0-1.0 representing the height
     * @return a pointer to an array of vertices.
     */
	GLfloat* drawCircle(int sides, GLfloat width, GLfloat height);
	void drawTeamCircles(int sides, GLfloat width, GLfloat height);

    /*!
     * Calculates the color for each vertex in the circle.
     * 0-270deg is gradient white to red
     * 270-360deg is gray
     *
     * @param sides - number of vertices in the circle
     * @return a pointer to an array of vertices.
     */
	void circleGradient(int sides);

private:

	int initBPS();
	int initGL();
	void createScene();

	GLfloat* m_teamACircle;
	GLfloat* m_teamBCircle;
	GLfloat* m_innerCircle;
	GLfloat* m_teamAColors;
	GLfloat* m_teamBColors;
	QString m_group;
	QString m_id;
	int m_width;
	int m_height;
	int m_startPos;
	screen_context_t m_screen_cxt;
	screen_window_t m_screen_win;
};

#endif /* MINUTESPERGAMERENDERER_H_ */
