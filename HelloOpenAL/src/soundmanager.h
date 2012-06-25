/* Copyright (c) 2012 Research In Motion Limited.
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


#ifndef _SOUNDMANAGER_H
#define _SOUNDMANAGER_H

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <vorbis/vorbisfile.h>

#include <QtCore/qstring.h>
#include <qhash.h>

// The number of max number of sound sources.
#define SOUNDMANAGER_MAX_NBR_OF_SOURCES 32

/**
 * SoundManager
 *
 * A very basic sound manager class for playing sounds using OpenAL.
 */
class SoundManager
{
public:

    /**
     * Constructor, initializes the sound manager. This function sets up OpenAL
     * and loads all sounds from the specified directory. This directory  should
     * be a folder only containing valid sounds.
     *
     * @param soundDirectory directory where all sounds are kept (have to be located in the assets/ folder)
     */
    SoundManager(QString soundDirectory);

    /**
     * Destructor destroys all buffers and sources.
     */
    ~SoundManager();

    /**
     * Called by the constructor, loads all sounds from the specified directory.
     *
     * @param soundDirectory directory where all sounds are kept (have to be located in the assets/ folder)
     */
    bool init(QString soundDirectory);

    /**
     * Plays a sound.
     *
     * @param fileName the name of the file in the soundDirectory.
     */
    bool play(QString fileName);

    /**
     * Plays a sound, with modified pitch and gain.
     *
     * @param fileName the name of the file in the soundDirectory
     * @param pitch specifies the pitch to be applied to a sound Range: [0.5-2.0]
     * @param gain sound gain (volume amplification) Range: ]0.0-  ]
     */
    bool play(QString fileName, float pitch, float gain);

private:

    // Load the .wav files
    bool loadWav(FILE* file, ALuint buffer);

    // Load the .ogg files
    bool loadOgg(FILE* file, ALuint buffer);

    // Sound buffers.
    QHash<QString, ALuint> mSoundBuffers;

    // Sound sources.
    ALuint mSoundSources[SOUNDMANAGER_MAX_NBR_OF_SOURCES];
};

#endif //_SOUNDMANAGER_H
