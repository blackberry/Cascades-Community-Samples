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


#include "soundmanager.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <QDir>
#include <qdebug.h>

// Error message function for ALUT.
static void reportALUTError(ALenum error)
{
    if (error != ALUT_ERROR_NO_ERROR)
    	qDebug() << "ALUT reported the following error: " << alutGetErrorString(error);
}

// Error message function for OpenAL.
static void reportOpenALError(ALenum error)
{
    if (error != AL_NO_ERROR)
    	qDebug() << "OpenAL reported the following error: \n" << alutGetErrorString(error);
}

SoundManager::SoundManager(QString soundDirectory)
{
    QString applicationDirectory;
    QString completeSoundDirectory;
    char cwd[PATH_MAX];

    // Initialize ALUT.
    if (alutInit(NULL, NULL) == false) {
        reportALUTError(alutGetError());
    }

    // Get the complete application directory in which we will load sounds from.
    // We convert to QString since it is more convenient when working with directories.
    getcwd(cwd, PATH_MAX);
    applicationDirectory = QString(cwd);

    // Append the assets directory and the actual sounds directory name.
    completeSoundDirectory = applicationDirectory
                            .append("/app/native/assets/")
                            .append(soundDirectory);

    // Create OpenAL buffers from all files in the sound directory.
    QDir dir(completeSoundDirectory);

    if (!dir.exists()) {
        qDebug() << "Cannot find the sounds directory." << completeSoundDirectory;
    } else {

        // Set a filter for file listing, only files should be listed.
        dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);

        // Get a directory listing.
        QFileInfoList list = dir.entryInfoList();

        // Traverse and load all the audio files into buffers.
        for (int i = 0; i < list.size(); ++i) {

            // Create a file info for each audio file.
        	QFileInfo fileInfo = list.at(i);
        	const char* path = fileInfo.absoluteFilePath().toStdString().c_str();
        	ALenum error;

        	// Generate buffers to hold audio data.
			alGenBuffers(1, &mSoundBuffers[fileInfo.fileName()]);

			error = alGetError();
			if (error != AL_NO_ERROR) {
				reportOpenALError(error);
				break;
			}

			// Load sound file.
			FILE* file = fopen(path, "rb");
			if (!file) {
				qDebug() << "Failed to load audio file " << path;
				break;
			}

			// Read the file header
			char header[12];
			ALuint bufferId = mSoundBuffers[fileInfo.fileName()];
			if (fread(header, 1, 12, file) != 12) {
				qDebug() << "Invalid header for audio file " << path;
				alDeleteBuffers(1, &bufferId);
				goto cleanup;
			}

			// Check the file format & load the buffer with audio data.
			if (memcmp(header, "RIFF", 4) == 0) {
				if (!loadWav(file, bufferId)) {
					qDebug() << "Invalid wav file: " << path;
					alDeleteBuffers(1, &bufferId);
					goto cleanup;
				}
			}
			else if (memcmp(header, "OggS", 4) == 0) {
				if (!loadOgg(file, bufferId)) {
					qDebug() << "Invalid ogg file: " << path;
					alDeleteBuffers(1, &bufferId);
					goto cleanup;
				}
			}
			else {
				qDebug() << "Unsupported audio file: " << path;
				goto cleanup;
			}

		cleanup:
			if (file) {
				fclose(file);
			}
        }
    }

    // Generate a number of sources used to attach buffers and play.
    alGenSources(SOUNDMANAGER_MAX_NBR_OF_SOURCES, mSoundSources);
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        reportOpenALError(error);
    }
}

SoundManager::~SoundManager()
{
    ALuint bufferID = 0;

    // Clear all the sources.
    for (int sourceIndex = 0; sourceIndex < SOUNDMANAGER_MAX_NBR_OF_SOURCES; sourceIndex++) {
        ALuint source = mSoundSources[sourceIndex];
        alDeleteSources(1, &source);

        ALenum error = alGetError();
		if (error != AL_NO_ERROR) {
			reportOpenALError(error);
		}
    }

    // Clear buffers, iterate through the hash
    QHashIterator<QString, ALuint> iterator(mSoundBuffers);

    while (iterator.hasNext()) {
        iterator.next();

        // Get the buffer id and delete it.
        bufferID  = mSoundBuffers[iterator.key()];
        if(bufferID != 0) {
            alDeleteBuffers(1, &bufferID);

            ALenum error = alGetError();
                if (error != AL_NO_ERROR) {
                    reportOpenALError(error);
                }
        }
    }

    // Clear the QHash for sound buffer id's.
    mSoundBuffers.clear();

    // Exit ALUT.
    ALenum error = alGetError();
        if (error != AL_NO_ERROR) {
            reportALUTError(error);
        }
}

bool SoundManager::loadWav(FILE* file, ALuint buffer)
{
	unsigned char stream[12];

	// Verify the wave fmt magic value meaning format.
	if (fread(stream, 1, 8, file) != 8 || memcmp(stream, "fmt ", 4) != 0 ) 	{
		qDebug() << "Failed to verify the magic value for the wave file format.";
		return false;
	}

	unsigned int section_size;
	section_size  = stream[7]<<24;
	section_size |= stream[6]<<16;
	section_size |= stream[5]<<8;
	section_size |= stream[4];

	// Check for a valid pcm format.
	if (fread(stream, 1, 2, file) != 2 || stream[1] != 0 || stream[0] != 1) {
		qDebug() << "Unsupported audio file format (must be a valid PCM format).";
		return false;
	}

	// Get the channel count (16-bit little-endian).
	int channels;
	if (fread(stream, 1, 2, file) != 2) {
		qDebug() << "Failed to read the wave file's channel count.";
		return false;
	}
	channels  = stream[1]<<8;
	channels |= stream[0];

	// Get the sample frequency (32-bit little-endian).
	ALuint frequency;
	if (fread(stream, 1, 4, file) != 4) {
		qDebug() << "Failed to read the wave file's sample frequency.";
		return false;
	}

	frequency  = stream[3]<<24;
	frequency |= stream[2]<<16;
	frequency |= stream[1]<<8;
	frequency |= stream[0];

	// The next 6 bytes hold the block size and bytes-per-second.
	// We don't need that info, so just read and ignore it.
	// We could use this later if we need to know the duration.
	if (fread(stream, 1, 6, file) != 6) {
		qDebug() << "Failed to read past the wave file's block size and bytes-per-second.";
		return false;
	}

	// Get the bit depth (16-bit little-endian).
	int bits;
	if (fread(stream, 1, 2, file) != 2) {
		qDebug() << "Failed to read the wave file's bit depth.";
		return false;
	}
	bits  = stream[1]<<8;
	bits |= stream[0];

	// Now convert the given channel count and bit depth into an OpenAL format.
	ALuint format = 0;
	if (bits == 8) {
		if (channels == 1)
			format = AL_FORMAT_MONO8;
		else if (channels == 2)
			format = AL_FORMAT_STEREO8;
	}
	else if (bits == 16) {
		if (channels == 1)
			format = AL_FORMAT_MONO16;
		else if (channels == 2)
			format = AL_FORMAT_STEREO16;
	}
	else {
		qDebug() << "Incompatible wave file format: ( " << channels << ", " << bits << ")";
		return false;
	}

	// Check against the size of the format header as there may be more data that we need to read.
	if (section_size > 16) {
		unsigned int length = section_size - 16;

		// Extension size is 2 bytes.
		if (fread(stream, 1, length, file) != length) {
			qDebug() << "Failed to read extension size from wave file.";
			return false;
		}
	}

	// Read in the rest of the file a chunk (section) at a time.
	while (true) {
		// Check if we are at the end of the file without reading the data.
		if (feof(file)) {
			qDebug() << "Failed to load wave file; file appears to have no data.";
			return false;
		}

		// Read in the type of the next section of the file.
		if (fread(stream, 1, 4, file) != 4) {
			qDebug() << "Failed to read next section type from wave file.";
			return false;
		}

		// Data chunk.
		if (memcmp(stream, "data", 4) == 0) {
			// Read how much data is remaining and buffer it up.
			unsigned int dataSize;
			if (fread(&dataSize, sizeof(int), 1, file) != 1) {
				qDebug() << "Failed to read size of data section from wave file.";
				return false;
			}

			char* data = new char[dataSize];
			if (fread(data, sizeof(char), dataSize, file) != dataSize) 	{
				qDebug() << "Failed to load wave file; file is missing data.";
				delete data;
				return false;
			}

			alBufferData(buffer, format, data, dataSize, frequency);
			ALenum error = alGetError();
			if (error != AL_NO_ERROR) {
				reportOpenALError(error);
			}
			delete data;

			// We've read the data, so return now.
			return true;
		}
		// Other chunk - could be any of the following:
		// - Fact ("fact")
		// - Wave List ("wavl")
		// - Silent ("slnt")
		// - Cue ("cue ")
		// - Playlist ("plst")
		// - Associated Data List ("list")
		// - Label ("labl")
		// - Note ("note")
		// - Labeled Text ("ltxt")
		// - Sampler ("smpl")
		// - Instrument ("inst")
		else {
			// Store the name of the chunk so we can report errors informatively.
			char chunk[5] = { 0 };
			memcpy(chunk, stream, 4);

			// Read the chunk size.
			if (fread(stream, 1, 4, file) != 4) {
				qDebug() << "Failed to read size of " << chunk << "chunk from wave file.";
				return false;
			}

			section_size  = stream[3]<<24;
			section_size |= stream[2]<<16;
			section_size |= stream[1]<<8;
			section_size |= stream[0];

			// Seek past the chunk.
			if (fseek(file, section_size, SEEK_CUR) != 0) {
				qDebug() << "Failed to seek past " << chunk << "in wave file.";
				return false;
			}
		}
	}

	return true;
}

bool SoundManager::loadOgg(FILE* file, ALuint buffer)
{
	OggVorbis_File ogg_file;
	vorbis_info* info;
	ALenum format;
	int result;
	int section;
	unsigned int size = 0;

	rewind(file);

	if ((result = ov_open(file, &ogg_file, NULL, 0)) < 0) {
		fclose(file);
		qDebug() << "Failed to open ogg file.";
		return false;
	}

	info = ov_info(&ogg_file, -1);

	if (info->channels == 1)
		format = AL_FORMAT_MONO16;
	else
		format = AL_FORMAT_STEREO16;

	// size = #samples * #channels * 2 (for 16 bit).
	unsigned int data_size = ov_pcm_total(&ogg_file, -1) * info->channels * 2;
	char* data = new char[data_size];

	while (size < data_size) {
		result = ov_read(&ogg_file, data + size, data_size - size, 0, 2, 1, &section);
		if (result > 0) {
			size += result;
		}
		else if (result < 0) {
			delete data;
			qDebug() << "Failed to read ogg file; file is missing data.";
			return false;
		}
		else {
			break;
		}
	}

	if (size == 0) {
		delete data;
		qDebug() << "Filed to read ogg file; unable to read any data.";
		return false;
	}

	alBufferData(buffer, format, data, data_size, info->rate);

	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		reportOpenALError(error);
	}

	delete data;
	ov_clear(&ogg_file);

	// ov_clear actually closes the file pointer as well.
	file = 0;

	return true;
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

        if (alIsSource (source) == AL_TRUE) {

            // Attach the buffer to an available source.
            alSourcei(source, AL_BUFFER, bufferID);

            ALenum error = alGetError();
			if (error != AL_NO_ERROR) {
				reportOpenALError(error);
			}

            // Set the source pitch value.
            alSourcef(source, AL_PITCH, pitch);

            error = alGetError();
			if (error != AL_NO_ERROR) {
				reportOpenALError(error);
			}

            // Set the source gain value.
            alSourcef(source, AL_GAIN, gain);

            error = alGetError();
			if (error != AL_NO_ERROR) {
				reportOpenALError(error);
			}

            // Play the source.
            alSourcePlay(source);

            error = alGetError();
			if (error != AL_NO_ERROR) {
				reportOpenALError(error);
			}
        }
    } else {
        // The buffer was not found.
        return false;
    }

    return true;
}

bool SoundManager::play(QString fileName)
{
    // Play the sound with default gain and pitch values.
    return play(fileName, 4.0f, 4.0f);
}
