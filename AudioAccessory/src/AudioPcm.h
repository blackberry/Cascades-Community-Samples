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
#ifndef AudioPcm_H_
#define AudioPcm_H_

#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <gulliver.h>
#include <math.h>
#include <time.h>
#include <sys/asoundlib.h>
#include <audio/audio_manager_routing.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <sys/timeb.h>

#include "AudioPcmTypes.h"
/*
 * Useful for logging
 */
#define LOG(...)                             \
	do {                                     \
		fprintf(stderr, "XXXX " __VA_ARGS__);\
		fflush(stderr);                      \
	} while (0)

#define EMIT_MESSAGE(__msg)                  \
	do {                                     \
		g_message_cb(__msg);                 \
	} while (0)

#define EMIT_SIGNAL_LEVEL(__arg)             \
	do {                                     \
		g_signal_level_cb(__arg);            \
	} while (0)

#define _BV(__bit) (1 << (__bit))

typedef struct circular_buffer {
	int length;
	int head;
	int tail;
	int size;
	char *buffer;
	pthread_mutex_t mutex;
} circular_buffer_t;

typedef struct outbound_message {
    struct outbound_message *prev;
    struct outbound_message *next;
    int message;
} outbound_message_t;

/*
 * Global constants
 */
#define CIRCULAR_BUFFER_NUMBER_FRAMES (30)
#define PREFERRED_FRAME_SIZE (640)
#define ACQUIRE_AUDIO_BUFFER_SIZE (16000)
#define SAMPLING_FREQUENCY (44100)
#define SAMPLING_TIME (1.0/SAMPLING_FREQUENCY)
#define FREQUENCY_HIGH (3150)
#define FREQUENCY_LOW (1575)
/*
 * High bit is >15 samples/peak; Low bit is >7 samples/peak
 * Total message format = 166(bits) = 155(high) + 1(low startbit) + 8(data) + 1(stopbit) + 1(endbit)
 */
#define HIGH_BIT_N_PEAKS (15)
#define LOW_BIT_N_PEAKS (7)
#define CARRIER_MIN_HIGH_BITS (12)
#define PEAK_THRESHOLD_DIFF (2)
#define SAMPLES_PER_BIT (136)
#define ENCODING_SAMPLES_PER_BIT (SAMPLES_PER_BIT/2)
#define SLOTS_PER_BIT (4)
#define SAMPLES_PER_SLOT (SAMPLES_PER_BIT/SLOTS_PER_BIT)
#define SILENCE_LENGTH (10)
#define CARRIER_LENGTH (5*28)
#define DATA_BITS (8)
#define STOP_BIT (1)
#define END_BIT (1)
#define START_BIT (1)
#define HIGH_BIT (1)
#define PEAK_AMPLITUDE_TRESHOLD (60)
#define NUMBER_SAMPLES_PEAK (4)
#define MINUMUM_NPEAKS (100)
#define BIT_HIGH_SYMBOL (2)
#define BIT_LOW_SYMBOL (1)
#define BIT_NONE_SYMBOL (0)
#define SOUND_AMPLITUDE (31000)
#define MINIMUM_BUFFER (4)
#define MAXIMUM_BUFFER (6*2)
#define PARITY_BITS_ERROR (-1000)
#define PARITY_EVEN (64)
#define PARITY_ODD (32)
#define SUMMARY_REPORT_INTERVAL (100)
#define INTER_MESSAGE_INTERVAL_MS (200)
#define PARITY_BITS_MASK (0b00011111)
/*
 * Global variables
 */
static bool g_audio_processing_active = true;
static circular_buffer_t* g_circular_buffer;
static unsigned int g_audio_manager_handle_c;
static unsigned int g_audio_manager_handle_p;
static int g_card_p;
static int g_device_p;
static int g_format_p;
static int g_sample_frequency_p;
static int g_frag_samples_p;
static int g_frame_size_p;
static pthread_attr_t g_attr_d;
static pthread_attr_t g_attr_m;
static pthread_t g_capturethread;
static pthread_t g_demodulatethread;
static pthread_t g_generatethread;
static snd_pcm_t *g_pcm_handle_c;
static snd_pcm_t *g_pcm_handle_p;
static int g_frame_size_c;
static int g_frame_size_d;
static bool g_capture_ready = false;
static bool g_carrier_detected = false;
static pthread_mutex_t g_demodulateMutex;
static pthread_cond_t g_condvar_datatodemodulate;
static pthread_mutex_t g_messageQueueMutex;
static pthread_mutex_t g_messagetogenerateMutex;
static pthread_cond_t g_condvar_messagetogenerate;
static int g_outbound_message_count;
static outbound_message_t *g_outbound_messages = NULL;
static unsigned int g_total_messages_accepted;
static unsigned int g_total_messages_frame_errors;
static unsigned int g_total_messages_parity_errors;
static _uint64 currentMillis();

/*
 * Call backs
 */
static audioServiceMessage_cb g_message_cb;
static audioServiceSignalLevel_cb g_signal_level_cb;
/*
 * Forward declarations
 */
static circular_buffer_t *createCircularBuffer();
static void destroyCircularBuffer(circular_buffer_t *circular_buffer);
static void writeByteToCircularBuffer(circular_buffer_t *circular_buffer, char val);
static char readByteFromCircularBuffer(circular_buffer_t *circular_buffer);
static bool writeToCircularBuffer(circular_buffer_t *circular_buffer, char *buffer, int number_bytes);
static bool readFromCircularBuffer(circular_buffer_t *circular_buffer, char buffer[], int number_bytes);
static void captureSetup();
static void deModulateSetup();
static void generateMessageSetup();
static void *captureThread(void *arg);
static void *deModulateThread(void *arg);
static void capture(circular_buffer_t* circular_buffer);
static int deModulate(circular_buffer_t* circular_buffer);
static bool carrierDetected(int16_t *sound, int length);
static bool carrierPresent(int16_t *sound, int length);
static int countPeaksInInterval(int16_t *sound, int start_index, int end_index);
static void decodeSound(int16_t *sound, int length);
static int fskDecode(int16_t *sound, int length);
static int validateParityBits(int message);
static int calculateParityBits(int number);
static int baudotEncoding(int number);
static void scanForPeaks(int16_t *sound, int length, int **n_peaks, int *n_peaks_length);
static void scanForBits(int *n_peaks, int n_peaks_length, int **bits, int *bits_length);
static void decodeMessage(int *bits, int bits_length, int *n_peaks, int n_peaks_length, int *message);
static int extractMessage(int *n_peaks, int n_peaks_length, int start_bit);
static int findNextNonZero(int *peaks, int peaks_length, int start_index);
static int findStartBit(int *bits, int bits_length, int start_index);
static void *generateMessageThread(void *arg);
static int generateMessages();
static void modulateAndTransmit(int char_to_send);
static void generateTone(int16_t *start, int length, int frequency, bool phase_shift);
static void writeTone(int16_t *start, int length);
static bool thisPhase(int previous_frequency, bool previous_phase_change, int this_frequency);
static void setVolume(int level);

#endif /* AudioPcm_H_ */
