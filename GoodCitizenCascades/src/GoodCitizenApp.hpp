/*
 * Copyright (c) 2011, 2012 Research In Motion Limited.
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

#ifndef GOODCITIZENAPP_HPP
#define GOODCITIZENAPP_HPP

#include <QtCore/QObject>
#include <QtCore/QMetaType>

#include "GoodCitizen.hpp"
#include "OpenGLThread.hpp"

#include <bb/cascades/Event>
#include <bb/cascades/UiObject>
#include <bb/cascades/Control>
#include <bb/cascades/ForeignWindow>
#include <bb/cascades/NavigationPane>

using namespace bb::cascades;

class GoodCitizenApp : public QObject
{
    Q_OBJECT

public Q_SLOTS:

	void onWindowAttached(unsigned long handle, const QString &group, const QString &id);
	void onAttachedChanged(bool changed);
	void onTimeout();

public:
	GoodCitizenApp();

	unsigned long createForeignWindow(const QString &group,
			const QString &id,
			int width, int height);

private:

	NavigationPane  *m_navPane;
    ForeignWindow *m_pForeignWindow;
	GoodCitizen *m_pGoodCitizen;
	OpenGLThread *m_pOpenGLThread;

};

#endif // ifndef GOODCITIZENAPP_HPP
