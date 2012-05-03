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

#ifndef GLTHREAD_H_
#define GLTHREAD_H_
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

#include <QWaitCondition>
#include <QMutex>
#include <QStack>
#include "IGLRenderer.h"

#include <QThread>
class GLThread :public QThread{

	enum RunState{Pause,Run};
public:
	static GLThread* instance();
	static GLThread* initInstance(int width, int height);

	void run();
	int resize(int width, int height);
	screen_context_t getContext();

	void setRenderer(IGLRenderer * renderer);
	bool m_isRunning;
	void previousRenderer();

private:
	static GLThread* m_instance;
	GLThread(int width,int height);
	virtual ~GLThread();

private:

	int initBPS();

	RunState m_runState;

	int m_width;
	int m_height;
	screen_context_t m_screen_cxt;

	QWaitCondition m_rendererReady;
	QWaitCondition m_rendererShutdown;
	QMutex m_mutex;
	IGLRenderer* m_renderer;
	QStack<IGLRenderer*> m_renderStack;
};

#endif /* GLTHREAD_H_ */
