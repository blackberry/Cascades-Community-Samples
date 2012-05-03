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

#ifndef REBOUNDSGRAPH_H_
#define REBOUNDSGRAPH_H_

#include <QObject>
#include <QString>

#include <bb/cascades/Application>
#include <bb/cascades/CustomControl>
#include <bb/cascades/ForeignWindow>

#include "ReboundsRenderer.h"


class ReboundsGraph : public bb::cascades::CustomControl
{
	Q_OBJECT
	Q_PROPERTY(QString teamA READ teamA WRITE setTeamA)
	Q_PROPERTY(QString teamB READ teamB WRITE setTeamB)

public:

	explicit ReboundsGraph(QObject* parent = 0);
	virtual ~ReboundsGraph();
	Q_INVOKABLE void initGLRenderer();
	Q_INVOKABLE void stopGLRenderer();
	void setTeamA(QString teamA);
	QString teamA();
	void setTeamB(QString teamB);
	QString teamB();

public slots:
	void onCreationCompleted();
	void onWindowAttached(unsigned long handle, const QString &group, const QString &id);

private:

	bb::cascades::ForeignWindow* m_foreignWindow;
	ReboundsRenderer *m_renderer;
	QString m_teamA;
	QString m_teamB;
};

#endif /* REBOUNDSGRAPH_H_ */
