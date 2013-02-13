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

#include "SoundManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <QDir>
#include <qdebug.h>

// Error message function for ALUT.
static void reportALUTError()
{
    qDebug() << "ALUT reported the following error: " << alutGetErrorString(alutGetError());
}

// Error message function for OpenAL.
static void reportOpenALError()
{
    qDebug() << "OpenAL reported the following error: \n" << alutGetErrorString(alGetError());
}

SoundManager::SoundManager(QString soundDirectory)
{
    QString applicationDirectory;
    QString completeSoundDirectory;
    char cwd[PATH_MAX];
    ALuint bufferID;

    // Initialize the ALUT.
    if (alutInit(NULL, NULL) == false) {
        reportALUTError();
    }

    // Get the complete application directory in which we will load sounds from
    // Convert to QString since it is more convenient when working with directories.
    getcwd(cwd, PATH_MAX);
    applicationDirectory = QString(cwd);

    // Append the assets directory and the actual sounds directory name to the QString.
    completeSoundDirectory = applicationDirectory.append("/app/native/assets/").append(
            soundDirectory);

    // Create OpenAL buffers from all files in the sound directory.
    QDir dir(completeSoundDirectory);

    if (!dir.exists()) {
        qDebug() << "Cannot find the sounds directory." << completeSoundDirectory;
    } else {

        // Set a filter for file listing (only files should be listed).
        dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

        // Get a directory listing.
        QFileInfoList list = dir.entryInfoList();

        // Traverse all the files in the directory.
        for (int i = 0; i < list.size(); ++i) {

            // Attempt to create buffers for all the files.
            QFileInfo fileInfo = list.at(i);
            bufferID = alutCreateBufferFromFile(fileInfo.absoluteFilePath().toStdString().c_str());

            if (alutGetError() == ALUT_ERROR_NO_ERROR) {

                // Add the bufferID to the Hash table with the file name as key.
                mSoundBuffers[fileInfo.fileName()] = bufferID;

            } else {
                reportALUTError();
            }
        }
    }

    // Generate a number of sources used to attach buffers and play the sounds.
    alGenSources(SOUNDMANAGER_MAX_NBR_OF_SOURCES, mSoundSources);

    if (alGetError() != AL_NO_ERROR) {
        reportOpenALError();
    }
}

SoundManager::~SoundManager()
{
    ALuint bufferID = 0;

    // Clear all the sources.
    for (int sourceIndex = 0; sourceIndex < SOUNDMANAGER_MAX_NBR_OF_SOURCES; sourceIndex++) {
        ALuint source = mSoundSources[sourceIndex];
        alDeleteSources(1, &source);

        if (alGetError() != AL_NO_ERROR) {
            reportOpenALError();
        }
    }

    // Clear buffers then iterate through the hash table.
    QHashIterator<QString, ALuint> iterator(mSoundBuffers);

    while (iterator.hasNext()) {
        iterator.next();

        // Get the buffer id and delete it.
        bufferID = mSoundBuffers[iterator.key()];
        if (bufferID != 0) {
            alDeleteBuffers(1, &bufferID);

            if (alGetError() != AL_NO_ERROR) {
                reportOpenALError();
            }
        }
    }

    // Clear the QHash for sound buffer ids.
    mSoundBuffers.clear();

    // Exit the ALUT.
    if (alutExit() == false) {
        reportALUTError();
    }
}

bool SoundManager::play(QString fileName, float pitch, float gain)
{
    static uint sourceIndex = 0;

    // Get the corresponding buffer id set up in the init function.
    ALuint bufferID = mSoundBuffers[fileName];

    if (bufferID != 0) {
        // Increment which source we are using, so that we play in a "free" source.
        sourceIndex = (sourceIndex + 1) % SOUNDMANAGER_MAX_NBR_OF_SOURCES;

        // Get the source in which the sound will be played.
        ALuint source = mSoundSources[sourceIndex];

        if (alIsSource(source) == AL_TRUE) {

            // Attach the buffer to an available source.
            alSourcei(source, AL_BUFFER, bufferID);

            if (alGetError() != AL_NO_ERROR) {
                reportOpenALError();
                return false;
            }

            // Set the source pitch value.
            alSourcef(source, AL_PITCH, pitch);

            if (alGetError() != AL_NO_ERROR) {
                reportOpenALError();
                return false;
            }

            // Set the source gain value.
            alSourcef(source, AL_GAIN, gain);

            if (alGetError() != AL_NO_ERROR) {
                reportOpenALError();
                return false;
            }

            // Play the source.
            alSourcePlay(source);

            if (alGetError() != AL_NO_ERROR) {
                reportOpenALError();
                return false;
            }
        }
    } else {
        // The buffer was not found, so return false.
        return false;
    }

    return true;
}

bool SoundManager::play(QString fileName)
{
    // Play the sound with default gain and pitch values.
    return play(fileName, 1.0f, 1.0f);
}
