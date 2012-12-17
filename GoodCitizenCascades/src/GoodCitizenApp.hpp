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

#include <bb/cascades/Application>
#include <bb/cascades/Event>
#include <bb/cascades/UiObject>
#include <bb/cascades/Control>
#include <bb/cascades/ForeignWindowControl>
#include <bb/cascades/LayoutUpdateHandler>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/OrientationSupport>
#include <bb/cascades/UIOrientation>

using namespace bb::cascades;

class GoodCitizenApp : public QObject
{
    Q_OBJECT

public:
	GoodCitizenApp();

public Q_SLOTS:
	// handle layout changes from device rotation
	void onLayoutFrameChanged(const QRectF &layoutFrame);

	// custom shutdown handler
	void shutdown();

private:
	bool m_fwBound;
	NavigationPane  *m_navPane;
    ForeignWindowControl *m_pForeignWindowControl;
	GoodCitizen *m_pGoodCitizen;
};

#endif // ifndef GOODCITIZENAPP_HPP
