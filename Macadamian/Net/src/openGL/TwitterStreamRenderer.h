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


#ifndef TWITTERSTREAMRENDERER_H_
#define TWITTERSTREAMRENDERER_H_
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

class TwitterStreamRenderer :public IGLRenderer{
public:
    TwitterStreamRenderer(const QString &group, const QString id, int width,int height);
	virtual ~TwitterStreamRenderer();

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

private:

	int initGL();
	float synthData(int i, int zzz, int which);
	void bar(GLfloat x,GLfloat y1,GLfloat y2,float w,float alpha);

	QString m_group;
	QString m_id;
	int m_width;
	int m_height;
	screen_context_t m_screen_cxt;
	screen_window_t m_screen_win;
};

#endif /* TWITTERSTREAMRENDERER_H_ */
