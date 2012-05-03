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

#include "TwitterStreamGraph.hpp"
#include "qdebug.h"
#include "GLThread.h"
using namespace bb::cascades;

TwitterStreamGraph::TwitterStreamGraph(QObject* parent) : bb::cascades::CustomControl() {
	Q_UNUSED(parent);

	QObject::connect(this, SIGNAL(creationCompleted()), this, SLOT(onCreationCompleted()));
}

TwitterStreamGraph::~TwitterStreamGraph()
{

}

void TwitterStreamGraph::onWindowAttached(unsigned long handle, const QString &group, const QString &id) {
	screen_flush_context(GLThread::instance()->getContext(), SCREEN_WAIT_IDLE);

	// Workaround: Cover up the hole made by the foreign window.
	// This will let the OpenGL window be tied up with the foreign Window and be transparently laid out on the top of the UI.
	m_foreignWindow->setVisible(false);
}

void TwitterStreamGraph::onCreationCompleted()
{
	qDebug() << "creation completed!";
	qDebug() << "Preferred Height: " << preferredHeight();
	qDebug() << "Preferred Width: " << preferredWidth();

	setRoot(Container::create().layout(AbsoluteLayout::create()).preferredSize(preferredWidth(), preferredHeight()).add(m_foreignWindow = ForeignWindow::create().preferredSize(preferredWidth(), preferredHeight())));

	m_renderer = new TwitterStreamRenderer(ForeignWindow::mainWindowGroupId(), "GLgraphThread", preferredWidth(), preferredHeight());

	QObject::connect(m_foreignWindow, SIGNAL(windowAttached(unsigned long,QString,QString)), this, SLOT(onWindowAttached(unsigned long,QString,QString)));

	GLThread::instance()->setRenderer(m_renderer);
}
