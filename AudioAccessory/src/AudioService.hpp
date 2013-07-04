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
#ifndef AUDIOSERVICE_H_
#define AUDIOSERVICE_H_

#include <QtCore/QObject>
#include <QDebug>

#include "AudioPcmExtern.h"

class AudioService : public QObject {

	Q_OBJECT
	Q_PROPERTY(
		bool audioIsRunning
		READ audioIsRunning
		NOTIFY audioRunningSignal
	)

public:
	AudioService(QObject *obj);
	virtual ~AudioService();
	void emitMessage(const QString &text);
	void emitSignalLevel(const int level);

public slots:
    void startAudio();
    void stopAudio();
    void addMessageToQueue(const QString &text);
//    void muteInput();

signals:
	void audioRunningSignal(bool);
	void message(const QVariant &);
	void signalLevel(const QVariant &);
	void audioStarted();
	void audioStopped();

private:
	bool audioIsRunning();
	bool _audioIsRunning;
};

#endif /* AUDIOSERVICE_H_ */
