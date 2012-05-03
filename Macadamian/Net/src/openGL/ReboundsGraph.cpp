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

#include <bb/cascades/Application>
#include <bb/cascades/CustomControl>
#include <bb/cascades/ForeignWindow>
#include <bb/cascades/Container>
#include <bb/cascades/AbsoluteLayout>
#include <bb/cascades/ImplicitAnimationController>

#include "ReboundsGraph.hpp"
#include "GLThread.h"

using namespace bb::cascades;

ReboundsGraph::ReboundsGraph(QObject* parent) : bb::cascades::CustomControl() {
	Q_UNUSED(parent);

	QObject::connect(this, SIGNAL(creationCompleted()), this, SLOT(onCreationCompleted()));
}

ReboundsGraph::~ReboundsGraph()
{
}

void ReboundsGraph::onWindowAttached(unsigned long handle, const QString &group, const QString &id) {
	screen_flush_context(GLThread::instance()->getContext(), SCREEN_WAIT_IDLE);

	m_foreignWindow->setVisible(false);
}

void ReboundsGraph::initGLRenderer()
{
	m_renderer->setTeams(m_teamA, m_teamB);
	m_renderer->m_animIndex = 0;

	QObject::connect(m_foreignWindow, SIGNAL(windowAttached(unsigned long,QString,QString)), this, SLOT(onWindowAttached(unsigned long,QString,QString)));
	GLThread::instance()->setRenderer(m_renderer);
}

void ReboundsGraph::stopGLRenderer()
{
	GLThread::instance()->setRenderer(NULL);
}

void ReboundsGraph::onCreationCompleted()
{
	setRoot(Container::create().layout(AbsoluteLayout::create()).preferredSize(preferredWidth(), preferredHeight()).add(m_foreignWindow = ForeignWindow::create().preferredSize(preferredWidth(), preferredHeight())));

	// Create an instance of OpenGLThread where the window initialization takes place.
	m_renderer = new ReboundsRenderer(ForeignWindow::mainWindowGroupId(), "GLgraphThread", preferredWidth(), preferredHeight());
}

QString ReboundsGraph::teamA()
{
    return m_teamA;
}

void ReboundsGraph::setTeamA(QString teamA)
{
	m_teamA = teamA;
}


QString ReboundsGraph::teamB()
{
    return m_teamB;
}

void ReboundsGraph::setTeamB(QString teamB)
{
	m_teamB = teamB;
}
