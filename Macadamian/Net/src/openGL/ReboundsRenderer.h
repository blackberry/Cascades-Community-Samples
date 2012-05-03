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

#ifndef REBOUNDSRENDERER_H_
#define REBOUNDSRENDERER_H_
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
#include "Player.h"

class ReboundsRenderer :public IGLRenderer{
public:
    ReboundsRenderer(const QString &group, const QString id, int width,int height);
	virtual ~ReboundsRenderer();

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

    //
    void setTeams(QString teamA, QString teamB);

    screen_window_t getWindow();

    // the animation index
    int m_animIndex;

private:

	int initGL();
	GLfloat* getGeometry(int index);
	GLfloat* getColours();

	GLfloat* getLegendGeometry(int index);
	GLfloat* getLegendColours(int index);

	QString m_group;
	QString m_id;
	int m_width;
	int m_height;
	screen_window_t m_screen_win;
	QList<data::Player> m_teamAStarters;
	QList<data::Player> m_teamBStarters;

	// MAGIC Numbers for layout of Graphs
	const static GLfloat LEFT = 0.104f; // left y axis location of the left graph
	const static GLfloat RIGHT = 0.883f; // right y axis location of the right graph
	const static GLfloat BOTTOM = 0.580f; // x axis location of both graphs

	const static GLfloat X_INC = 0.008884; // the amount we should increment for a stat of exactly 1
	const static GLfloat Y_INC = 0.0554; // the amount we should increment for each player

	const static int MAX_FRAMES = 82;
};

#endif /* REBOUNDSRENDERER_H_ */
