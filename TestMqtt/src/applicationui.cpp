/* Copyright (c) 2013 BlackBerry Limited.
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
#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

ApplicationUI::ApplicationUI(bb::cascades::Application *app)
	: QObject(app)
	, _pubSubClient(new PubSubClient(this))
{
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    AbstractPane *root = qml->createRootObject<AbstractPane>();


	QObject *mainPage = root->findChild<QObject*>((const QString) "mainPage");

	QObject::connect(     mainPage,   SIGNAL(mqttConnect(const QString &)),
					 _pubSubClient,     SLOT(mqttConnect(const QString &)));

	QObject::connect(     mainPage,   SIGNAL(mqttDisconnect()),
					 _pubSubClient,     SLOT(mqttDisconnect()));

	QObject::connect(     mainPage,   SIGNAL(mqttPublish(const QString &, const QString &)),
					 _pubSubClient,     SLOT(mqttPublish(const QString &, const QString &)));

	QObject::connect(     mainPage,   SIGNAL(mqttSubscribe(const QString &)),
					 _pubSubClient,     SLOT(mqttSubscribe(const QString &)));

	QObject::connect(     mainPage,   SIGNAL(mqttUnSubscribe(const QString &)),
					 _pubSubClient,     SLOT(mqttUnSubscribe(const QString &)));

	QObject::connect(_pubSubClient,   SIGNAL(logMessage(const QVariant &)),
			              mainPage,     SLOT(message(const QVariant &)));

	QObject::connect(_pubSubClient,   SIGNAL(mqttConnected()),
			              mainPage,     SLOT(mqttConnected()));

	QObject::connect(_pubSubClient,   SIGNAL(mqttDisconnected()),
			              mainPage,     SLOT(mqttDisconnected()));

	app->setScene(root);

	_pubSubClient->logApiVersion();
}

