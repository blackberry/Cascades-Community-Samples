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

#ifndef TWITTERSTREAMGRAPH_H_
#define TWITTERSTREAMGRAPH_H_

#include <QObject>
#include <QString>

#include <bb/cascades/Application>
#include <bb/cascades/CustomControl>
#include <bb/cascades/ForeignWindow>

#include "TwitterStreamRenderer.h"


class TwitterStreamGraph : public bb::cascades::CustomControl
{
	Q_OBJECT

public:

	explicit TwitterStreamGraph(QObject* parent = 0);
	virtual ~TwitterStreamGraph();

public slots:

	void onCreationCompleted();
	void onWindowAttached(unsigned long handle, const QString &group, const QString &id);

private:

	bb::cascades::ForeignWindow* m_foreignWindow;
	TwitterStreamRenderer *m_renderer;
};

#endif /* TWITTERSTREAMGRAPH_H_ */
