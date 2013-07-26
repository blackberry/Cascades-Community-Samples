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
#include "AudioService.hpp"
#include <stdint.h>
#include <audio/audio_manager_volume.h>

static AudioService *audioServiceInstance = 0;

void messageCallBack(const char *message)
{
	if (audioServiceInstance) {
		audioServiceInstance->emitMessage(message);
    }
}

void signalLevelCallBack(const int level)
{
	if (audioServiceInstance) {
		audioServiceInstance->emitSignalLevel(level);
    }
}

AudioService::AudioService(QObject *obj)
	: QObject(obj)
	, _audioIsRunning(false)
{
	qDebug() << "XXXX AudioService::AudioService - entered";
	audioServiceInstance = this;
}

AudioService::~AudioService()
{
	qDebug() << "XXXX AudioService::~AudioService - entered";
}

void AudioService::startAudio()
{
	startAudioPcm(messageCallBack, signalLevelCallBack);
	_audioIsRunning = !_audioIsRunning;
	emit audioStarted();
}

void AudioService::stopAudio()
{
	stopAudioPcm();
	_audioIsRunning = !_audioIsRunning;
	emit audioStopped();
}

bool AudioService::audioIsRunning()
{
	return _audioIsRunning;
}

void AudioService::emitMessage(const QString &text)
{
    emit message(text);
}

void AudioService::emitSignalLevel(const int level)
{
    emit signalLevel(QVariant(level));
}

void AudioService::addMessageToQueue(const QString &text)
{
	for (int i = 0; i < text.length(); i++) {
		addMessage(text.at(i).toUpper().toAscii());
	}
}

//void AudioService::muteInput()
//{
//	qDebug() << "XXXX muteInput()";
//	int rc=0;
//	audio_manager_device_t *dev;
//	rc = audio_manager_get_default_device(dev);
//	qDebug() << "XXXX audio_manager_get_default_device rc=" << rc;
//	rc =  audio_manager_set_input_mute(*dev,true);
//	qDebug() << "XXXX "
//			""
//			"audio_manager_set_input_mute rc=" << rc;
//}
//
