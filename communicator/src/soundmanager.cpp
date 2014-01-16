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

#include "soundmanager.h"

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

  // First, we get the complete application directory in which we will load sounds from then
  // we convert to QString since it is more convenient when working with directories.
  getcwd(cwd, PATH_MAX);
  applicationDirectory = QString(cwd);

  // Append the assets directory and the actual sounds directory name.
  completeSoundDirectory = applicationDirectory.append("/app/native/assets/").append(
      soundDirectory);

  // Create OpenAL buffers from all files in the sound directory.
  QDir dir(completeSoundDirectory);

  if (!dir.exists()) {
    qDebug() << "Cannot find the sounds directory." << completeSoundDirectory;
  } else {

    // Set a filter for file listing, only files should be listed.
    dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

    // Get a directory listing.
    QFileInfoList list = dir.entryInfoList();

    // Traverse all the files.
    for (int i = 0; i < list.size(); ++i) {

      // Attempt to create a buffer for each file.
      QFileInfo fileInfo = list.at(i);
      bufferID = alutCreateBufferFromFile(fileInfo.absoluteFilePath().toStdString().c_str());

      if (alutGetError() == ALUT_ERROR_NO_ERROR) {

        // If successful on buffer creation, add the bufferID to the Hash table
        // with the file name as key.
        mSoundBuffers[fileInfo.fileName()] = bufferID;

      } else {
        reportALUTError();
      }
    }
  }

  // Generate a number of sources used to attach buffers and play.
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

  // Clear the buffers and iterate through the hash table.
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

  // Clear the QHash for sound buffer IDs.
  mSoundBuffers.clear();

  // Exit the ALUT.
  if (alutExit() == false) {
    reportALUTError();
  }
}

bool SoundManager::play(QString fileName, float pitch, float gain)
{
  static uint sourceIndex = 0;

  // Get the corresponding buffer ID to the file name.
  ALuint bufferID = mSoundBuffers[fileName];

  if (bufferID != 0) {
    // Increment which source we are using, so that we play in an unused source.
    sourceIndex = (sourceIndex + 1) % SOUNDMANAGER_MAX_NBR_OF_SOURCES;

    // Get the source from which the sound will be played.
    ALuint source = mSoundSources[sourceIndex];

    if (alIsSource(source) == AL_TRUE) {

      // Attach the buffer the available source.
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
    // Otherwise, the buffer was not found.
    return false;
  }

  return true;
}

bool SoundManager::play(QString fileName)
{
  // Play the sound with default gain and pitch values.
  return play(fileName, 1.0f, 1.0f);
}
