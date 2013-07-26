/* Copyright (c) 2013 Research In Motion Limited.
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
#include "AudioAccessory.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

AudioAccessory::AudioAccessory(bb::cascades::Application *app)
	: QObject(app)
	, _audioService(new AudioService(this))
{
    QmlDocument *_qml = QmlDocument::create("asset:///main.qml").parent(this);
    AbstractPane *_root = _qml->createRootObject<AbstractPane>();

	QObject *mainPage = _root->findChild<QObject*>((const QString) "mainPage");

	QObject::connect(         this,   SIGNAL(message(const QVariant &)),
			              mainPage,     SLOT(message(const QVariant &)));

	QObject::connect(     mainPage,   SIGNAL(startAudio()),
			         _audioService,     SLOT(startAudio()));

	QObject::connect(     mainPage,   SIGNAL(stopAudio()),
			         _audioService,     SLOT(stopAudio()));

	QObject::connect(     mainPage,   SIGNAL(sendMessage(const QString &)),
			         _audioService,     SLOT(addMessageToQueue(const QString &)));

//	qDebug() << "XXXX connecting muteInput() signal";
//	QObject::connect(     mainPage,   SIGNAL(muteInput()),
//			         _audioService,     SLOT(muteInput()));

	QObject::connect(_audioService,   SIGNAL(message(const QVariant &)),
			              mainPage,     SLOT(message(const QVariant &)));

	QObject::connect(_audioService,   SIGNAL(signalLevel(const QVariant &)),
			              mainPage,     SLOT(signalLevel(const QVariant &)));

	QObject::connect(_audioService,   SIGNAL(audioStarted()),
			              mainPage,     SLOT(audioStarted()));

	QObject::connect(_audioService,   SIGNAL(audioStopped()),
			              mainPage,     SLOT(audioStopped()));

	app->setScene(_root);
}

