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
#ifndef AudioPcmExtern_H_
#define AudioPcmExtern_H_

#include "AudioPcmTypes.h"
/*
 * Entry points visible externally
 */
extern "C" {
	void startAudioPcm(audioServiceMessage_cb message_callback, audioServiceSignalLevel_cb signal_level_callback);
	void stopAudioPcm();
	bool addMessage(int message);
}
#endif /* AudioPcmExtern_H_ */
