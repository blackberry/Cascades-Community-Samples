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
#include "AudioPcm.h"
/*
 * Externally visible function
 */
void startAudioPcm(audioServiceMessage_cb message_callback, audioServiceSignalLevel_cb signal_level_callback)
{
	g_message_cb = message_callback;
	g_signal_level_cb = signal_level_callback;
	LOG("startAudioPcm() - entered\n");

	g_audio_processing_active = true;
	g_circular_buffer = createCircularBuffer();

    captureSetup();
    deModulateSetup();
    generateMessageSetup();

    int policy;
    struct sched_param param;

    pthread_attr_init(&g_attr_d);
    pthread_attr_setdetachstate(&g_attr_d, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setinheritsched(&g_attr_d, PTHREAD_EXPLICIT_SCHED);

    pthread_attr_init(&g_attr_m);
    pthread_attr_setdetachstate(&g_attr_m, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setinheritsched(&g_attr_m, PTHREAD_EXPLICIT_SCHED);

    pthread_getschedparam(pthread_self(), &policy, &param);

    param.sched_priority=12;

    pthread_attr_setschedparam(&g_attr_d, &param);
    pthread_attr_setschedpolicy(&g_attr_d, SCHED_RR);

    pthread_attr_setschedparam(&g_attr_m, &param);
    pthread_attr_setschedpolicy(&g_attr_m, SCHED_RR);

    pthread_create(&g_capturethread, NULL, &captureThread, g_circular_buffer);
    pthread_create(&g_demodulatethread, &g_attr_d, &deModulateThread, g_circular_buffer);
    pthread_create(&g_generatethread, &g_attr_m, &generateMessageThread, NULL);

	LOG("startAudioPcm() - exit\n");
}
/*
 * Externally visible function
 */
void stopAudioPcm()
{
	LOG("stopAudioPcm() - entered\n");

	pthread_mutex_lock(&g_demodulateMutex);
	g_audio_processing_active = false;
	pthread_mutex_unlock(&g_demodulateMutex);
	pthread_cond_signal(&g_condvar_datatodemodulate);
    pthread_cond_signal(&g_condvar_messagetogenerate);

	LOG("stopAudioPcm() - joining message generate thread=%d\n", pthread_join(g_generatethread, NULL));
	LOG("stopAudioPcm() - joining de-modulate thread=%d\n", pthread_join(g_demodulatethread, NULL));
	LOG("stopAudioPcm() - joining capture thread=%d\n", pthread_join(g_capturethread, NULL));

	pthread_cond_destroy(&g_condvar_messagetogenerate);
	pthread_mutex_destroy(&g_messagetogenerateMutex);

    pthread_mutex_destroy(&g_messageQueueMutex);

	pthread_cond_destroy(&g_condvar_datatodemodulate);
    pthread_mutex_destroy(&g_demodulateMutex);

	destroyCircularBuffer(g_circular_buffer);

	LOG("stopAudioPcm() - exit\n");
}
/*
 * Externally visible function
 */
bool addMessage(int message)
{
	outbound_message_t *new_message = NULL;
	bool message_created = false;

	pthread_mutex_lock(&g_messageQueueMutex);

	new_message = malloc(sizeof(outbound_message_t));

	if (new_message != NULL) {
		memset(new_message, 0, sizeof(outbound_message_t));

		new_message->message = message;
		new_message->next = NULL;

		g_outbound_message_count++;
		message_created = true;

		if (g_outbound_messages == NULL) {
			g_outbound_messages = new_message;
			new_message->prev = NULL;
		} else {

			outbound_message_t *list_message;

			for(list_message = g_outbound_messages; list_message != NULL; list_message = list_message->next) {
				if (list_message->next == NULL) {
					new_message->prev = list_message;
					list_message->next = new_message;
					break;
				}
			}
 		}
	}

	pthread_mutex_unlock(&g_messageQueueMutex);

	if (message_created) {
		pthread_cond_signal(&g_condvar_messagetogenerate);
	}

	return new_message;
}
/*
 * Internal (static) functions only after this point
 */
static void captureSetup()
{
	LOG("captureSetup() - entered\n");

	snd_pcm_channel_setup_t setup;
	int rc;
	snd_pcm_channel_info_t pcm_channel_info;
	snd_mixer_group_t mixer_group;
	snd_pcm_channel_params_t pcm_channel_params;
	int card = setup.mixer_card;

	if ((rc = audio_manager_snd_pcm_open_name(AUDIO_TYPE_DEFAULT,
											  &g_pcm_handle_c,
											  &g_audio_manager_handle_c,
											  (char *) "default",
											  SND_PCM_OPEN_CAPTURE)) < 0) {
		LOG("captureSetup() - audio_manager_snd_pcm_open_name failed rc=%d %s\n", rc, strerror(rc));
		return;
	}

	if ((rc = snd_pcm_plugin_set_disable(g_pcm_handle_c, PLUGIN_DISABLE_MMAP)) < 0) {
		LOG("captureSetup() - snd_pcm_plugin_set_disable failed: %s\n", snd_strerror(rc));
		return;
	}

	if ((rc = snd_pcm_plugin_set_enable(g_pcm_handle_c, PLUGIN_ROUTING)) < 0) {
		LOG("captureSetup() - snd_pcm_plugin_set_enable: %s\n", snd_strerror(rc));
		return;
	}

	memset(&pcm_channel_info, 0, sizeof(pcm_channel_info));
	pcm_channel_info.channel = SND_PCM_CHANNEL_CAPTURE;

	if ((rc = snd_pcm_plugin_info(g_pcm_handle_c, &pcm_channel_info)) < 0) {
		LOG("captureSetup() - snd_pcm_plugin_info failed: %s\n", snd_strerror(rc));
		return;
	}

	LOG("captureSetup() - capture minimum rate = %d\n", pcm_channel_info.min_rate);

	memset(&pcm_channel_params, 0, sizeof(pcm_channel_params));

	LOG("captureSetup() - capture minimum fragment size = %d\n",pcm_channel_info.min_fragment_size);

	pcm_channel_params.mode = SND_PCM_MODE_BLOCK;
	pcm_channel_params.channel = SND_PCM_CHANNEL_CAPTURE;
	pcm_channel_params.stop_mode = SND_PCM_STOP_ROLLOVER;
	pcm_channel_params.start_mode = SND_PCM_START_FULL;
	pcm_channel_params.buf.block.frag_size = PREFERRED_FRAME_SIZE;
	pcm_channel_params.buf.block.frags_max = 5;
	pcm_channel_params.buf.block.frags_min = 1;
	pcm_channel_params.format.interleave = 1;
	pcm_channel_params.format.rate = SAMPLING_FREQUENCY;
	pcm_channel_params.format.voices = 1;
	pcm_channel_params.format.format = SND_PCM_SFMT_S16_LE;

	if ((rc = snd_pcm_plugin_params(g_pcm_handle_c, &pcm_channel_params)) < 0) {
		LOG("captureSetup() - snd_pcm_plugin_params failed: %s\n", snd_strerror(rc));
		return;
	}
	memset(&setup, 0, sizeof(setup));
	memset(&mixer_group, 0, sizeof(mixer_group));

	setup.channel = SND_PCM_CHANNEL_CAPTURE;
	setup.mixer_gid = &mixer_group.gid;

	if ((rc = snd_pcm_plugin_setup(g_pcm_handle_c, &setup)) < 0) {
		LOG("captureSetup() - snd_pcm_plugin_setup failed: %s\n", snd_strerror(rc));
		return;
	}

	LOG("captureSetup() - capture format %s card = %d\n", snd_pcm_get_format_name(setup.format.format), card);
	LOG("captureSetup() - capture rate %d \n", setup.format.rate);

	g_frame_size_c = setup.buf.block.frag_size;

	if (mixer_group.gid.name[0] == 0) {
		LOG("captureSetup() - Mixer Pcm Group [%s] Not Set \n", mixer_group.gid.name);
		LOG("captureSetup() - ***>>>> Input Gain Controls Disabled <<<<*** \n");
	} else {
		LOG("captureSetup() - Mixer Pcm Group [%s]\n", mixer_group.gid.name);
	}

	g_frame_size_c = setup.buf.block.frag_size;

	LOG("captureSetup() - capture frame size = %d\n", g_frame_size_c);

	if ((rc = snd_pcm_plugin_prepare(g_pcm_handle_c, SND_PCM_CHANNEL_CAPTURE)) < 0) {
		LOG("captureSetup() - snd_pcm_plugin_prepare failed: %s\n", snd_strerror(rc));
	}
	LOG("captureSetup() - exit\n");
}

static void deModulateSetup()
{
	LOG("deModulateSetup() - entered\n");

	g_frame_size_d = ACQUIRE_AUDIO_BUFFER_SIZE;

	g_total_messages_accepted = 0;
	g_total_messages_frame_errors = 0;
	g_total_messages_parity_errors = 0;

    pthread_mutex_init(&g_demodulateMutex, NULL);
    pthread_cond_init(&g_condvar_datatodemodulate, NULL);

	LOG("deModulateSetup() - exited\n");
}

static void generateMessageSetup()
{
	LOG("generateMessageSetup() - entered\n");

	g_outbound_messages = NULL;
	g_outbound_message_count = 0;

	snd_mixer_group_t mixer_group;
	snd_pcm_channel_params_t pcm_channel_params;
	snd_pcm_channel_info_t pcm_channel_info;
    snd_pcm_channel_setup_t pcm_channel_setup;

    int rc;

	if ((rc = audio_manager_snd_pcm_open_name(AUDIO_TYPE_DEFAULT,
											  &g_pcm_handle_p,
											  &g_audio_manager_handle_p,
											  (char *) "default",
											  SND_PCM_OPEN_PLAYBACK)) < 0) {
		LOG("generateMessageSetup() - audio_manager_snd_pcm_open_name failed rc=%d %s\n", rc, strerror(rc));
		return;
	}

	if ((rc = snd_pcm_plugin_set_disable(g_pcm_handle_p, PLUGIN_DISABLE_MMAP)) < 0) {
		LOG("generateMessageSetup() - snd_pcm_plugin_set_disable failed: %s\n", snd_strerror(rc));
        snd_pcm_close(g_pcm_handle_p);
		return;
	}

	if ((rc = snd_pcm_plugin_set_enable(g_pcm_handle_p, PLUGIN_ROUTING)) < 0) {
		LOG("generateMessageSetup() - snd_pcm_plugin_set_enable: %s\n", snd_strerror(rc));
        snd_pcm_close(g_pcm_handle_p);
		return;
	}

	memset(&pcm_channel_info, 0, sizeof(pcm_channel_info));
	pcm_channel_info.channel = SND_PCM_CHANNEL_PLAYBACK;

	if ((rc = snd_pcm_channel_info(g_pcm_handle_p, &pcm_channel_info)) < 0) {
		LOG("generateMessageSetup() - snd_pcm_channel_info failed: %s\n", snd_strerror(rc));
        snd_pcm_close(g_pcm_handle_p);
		return;
	}

	memset(&pcm_channel_params, 0, sizeof(pcm_channel_params));

	pcm_channel_params.mode = SND_PCM_MODE_BLOCK;
	pcm_channel_params.channel = SND_PCM_CHANNEL_PLAYBACK;
	pcm_channel_params.start_mode = SND_PCM_START_FULL;
	pcm_channel_params.stop_mode = SND_PCM_STOP_STOP;
	pcm_channel_params.buf.block.frag_size = pcm_channel_info.max_fragment_size;
	pcm_channel_params.buf.block.frags_max = 3;
	pcm_channel_params.buf.block.frags_min = 1;
	pcm_channel_params.format.format =  SND_PCM_SFMT_S16_LE;
	pcm_channel_params.format.rate = SAMPLING_FREQUENCY;
	pcm_channel_params.format.voices = 1;

    memset(&pcm_channel_setup, 0, sizeof(pcm_channel_setup) );
    memset(&mixer_group, 0, sizeof(mixer_group));

    pcm_channel_setup.channel = SND_PCM_CHANNEL_PLAYBACK;
    pcm_channel_setup.mixer_gid = &mixer_group.gid;

    strcpy(pcm_channel_params.sw_mixer_subchn_name, "voice_ringtone");

    if ((rc = snd_pcm_plugin_params( g_pcm_handle_p, &pcm_channel_params)) < 0) {
		LOG("generateMessageSetup() - snd_pcm_plugin_params: %s\n", snd_strerror(rc));
        snd_pcm_close(g_pcm_handle_p);
		return;
    }

    if ((rc = snd_pcm_plugin_prepare (g_pcm_handle_p, SND_PCM_CHANNEL_PLAYBACK)) < 0) {
		LOG("generateMessageSetup() - snd_pcm_plugin_prepare: %s\n", snd_strerror(rc));
        snd_pcm_close(g_pcm_handle_p);
		return;
    }

    if ((rc = snd_pcm_plugin_setup (g_pcm_handle_p, &pcm_channel_setup)) < 0) {
		LOG("generateMessageSetup() - snd_pcm_plugin_setup: %s\n", snd_strerror(rc));
        snd_pcm_close(g_pcm_handle_p);
		return;
    }

    g_card_p = pcm_channel_setup.mixer_card;
    g_device_p = pcm_channel_setup.mixer_device;
    g_sample_frequency_p = pcm_channel_setup.format.rate;
    g_frag_samples_p = pcm_channel_setup.buf.block.frag_size / 4;
    g_frame_size_p = pcm_channel_setup.buf.block.frag_size;
    g_format_p = pcm_channel_params.format.format;

	if (mixer_group.gid.name[0] == 0) {
		LOG("generateMessageSetup() - Mixer Pcm Group [%s] Not Set \n", mixer_group.gid.name);
		LOG("generateMessageSetup() - ***>>>> Input Gain Controls Disabled <<<<*** \n");
	} else {
		LOG("generateMessageSetup() - Mixer Pcm Group [%s]\n", mixer_group.gid.name);
	}

	pthread_mutex_init(&g_messageQueueMutex, NULL);
	pthread_mutex_init(&g_messagetogenerateMutex, NULL);
    pthread_cond_init(&g_condvar_messagetogenerate, NULL);

    LOG("generateMessageSetup() - Playback Format %s g_card_p=%d\n", snd_pcm_get_format_name (pcm_channel_setup.format.format), g_card_p);
    LOG("generateMessageSetup() - Playback preferred g_frame_size_p %d \n", pcm_channel_info.max_fragment_size);
    LOG("generateMessageSetup() - Playback frame_size_p %d \n", pcm_channel_setup.buf.block.frag_size);
    LOG("generateMessageSetup() - Playback frame_samples_p %d \n", g_frag_samples_p);
    LOG("generateMessageSetup() - Playback Rate %d \n", pcm_channel_setup.format.rate);

    setVolume(85);

	LOG("generateMessageSetup() - exited\n");
}

static void *captureThread(void *arg)
{
	LOG("captureThread() - entered - thread=%d\n", pthread_self());
	capture((circular_buffer_t *)arg);
	return (0);
}

static void *deModulateThread(void *arg)
{
	LOG("deModulateThread() - entered - thread=%d\n", pthread_self());
	deModulate((circular_buffer_t *)arg);
	return (0);
}

static void *generateMessageThread(void *arg)
{
	(void)arg; //suppress warning message

	LOG("generateMessageThread() - entered - thread=%d\n", pthread_self());
	generateMessages();
	return (0);
}

static void capture(circular_buffer_t* circular_buffer)
{
	LOG("capture() - entered\n");

	char *record_buffer;
	record_buffer = (char*) malloc(g_frame_size_c);
	memset(record_buffer, 0, g_frame_size_c);

	int failed = 0;
	int total_read = 0;
	snd_pcm_channel_status_t status;
	status.channel = SND_PCM_CHANNEL_CAPTURE;

	while (g_audio_processing_active) {
		int read = snd_pcm_plugin_read(g_pcm_handle_c, record_buffer, g_frame_size_c);

		if (read < 0 || read != g_frame_size_c) {
			failed++;

			LOG("capture() - capture failure: snd_pcm_plugin_read: %d requested = %d\n", read, g_frame_size_c);

			if (snd_pcm_plugin_status(g_pcm_handle_c, &status) < 0) {
				LOG("capture() - capture channel status error: %d\n", status.status);
			} else {
				if (status.status == SND_PCM_STATUS_READY   ||
					status.status == SND_PCM_STATUS_OVERRUN ||
				    status.status == SND_PCM_STATUS_ERROR) {

					LOG("capture() - capture failure: snd_pcm_plugin_status: = %d \n", status.status);
					if (snd_pcm_plugin_prepare(g_pcm_handle_c, SND_PCM_CHANNEL_CAPTURE) < 0) {
						LOG("capture() - capture channel prepare error %d\n", status.status);
						exit(1);
					}
				}
			}
		} else {
			total_read += read;
		}
		g_capture_ready = true;

		if (!writeToCircularBuffer(circular_buffer, record_buffer, g_frame_size_c)) {
			failed++;
		}
		pthread_cond_signal(&g_condvar_datatodemodulate);
	}

	LOG("capture() - exiting\n");

	(void) snd_pcm_plugin_flush(g_pcm_handle_c, SND_PCM_CHANNEL_CAPTURE);
	(void) snd_pcm_close(g_pcm_handle_c);

	audio_manager_free_handle(g_audio_manager_handle_c);

	LOG("capture() - exit - Total Bytes read = %d failed = %d\n", total_read, failed);

	free(record_buffer);
}

static int deModulate(circular_buffer_t* circular_buffer)
{
	LOG("deModulate() - entered\n");

	char *demodulate_buffer = (char *) malloc(g_frame_size_d);
	memset(demodulate_buffer, 0, g_frame_size_d);

	int16_t *sound_buffer = 0;

	int total_processed = 0;
	int capture_not_ready = 0;

	while (g_audio_processing_active) {

		pthread_mutex_lock(&g_demodulateMutex);
		pthread_cond_wait(&g_condvar_datatodemodulate, &g_demodulateMutex);
		pthread_mutex_unlock(&g_demodulateMutex);

		if (!g_capture_ready) {
			++capture_not_ready;
			continue;
		}

		int processed;

		while (readFromCircularBuffer(circular_buffer, demodulate_buffer, g_frame_size_d)) {
			processed = g_frame_size_d;
			total_processed += processed;

			int sound_length = g_frame_size_d / 2;
			sound_buffer = (int16_t *)demodulate_buffer;

			int i;
			int max_value = 0;
			for (i=0; i<sound_length; i++) {
				if (sound_buffer[i] > max_value) max_value = sound_buffer[i];
			}

			EMIT_SIGNAL_LEVEL(max_value);

			/*
			 * Only decode sound if we believe there is a signal present on audio input
			 */
			if (carrierDetected(sound_buffer, sound_length)) {
				decodeSound(sound_buffer, sound_length);
			}
		}
	}

	LOG("deModulate() - Demodulate exiting\n");

	free(demodulate_buffer);

	LOG("deModulate() - Demodulate exited - Total Bytes processed = %d capture_not_ready %d\n",
			total_processed, capture_not_ready);

	return 0;
}

static int generateMessages()
{
	LOG("generateMessages() - entered\n");

	outbound_message_t *message_to_generate = NULL;
	int message_to_send = 0;
	bool got_message_to_send = false;
	_uint64 last_message_processed = currentMillis();

	while (g_audio_processing_active) {

		pthread_mutex_lock(&g_messagetogenerateMutex);

		if (g_outbound_message_count == 0) {
			pthread_cond_wait(&g_condvar_messagetogenerate, &g_messagetogenerateMutex);
		}
		if (g_outbound_message_count == 0) {
			pthread_mutex_unlock(&g_messagetogenerateMutex);
			continue;
		}
		pthread_mutex_unlock(&g_messagetogenerateMutex);

		/*
		 * Throttle the reading of messages (characters) to be sent so we can see
		 * and demonstrate what's happening
		 */
		if ((currentMillis() - last_message_processed) < (_uint64)INTER_MESSAGE_INTERVAL_MS) {
			int rc = nanosleep((struct timespec[]){{0, INTER_MESSAGE_INTERVAL_MS * 1000000}}, NULL);
			if (rc < 0) {
				LOG("generateMessages() - nanosleep() - return code %d\n", rc);
			}
		}

		pthread_mutex_lock(&g_messageQueueMutex);

		got_message_to_send = false;

		message_to_generate = g_outbound_messages;

		LOG("generateMessages() - message_to_generate=%d\n", (int)message_to_generate);

		if (message_to_generate != NULL) {
			got_message_to_send = true;

			message_to_send = message_to_generate->message;

			LOG("generateMessages() - message_to_send=%d\n", message_to_send);

			g_outbound_messages = message_to_generate->next;
			message_to_generate->prev = NULL;
			g_outbound_message_count--;

		} else {
			LOG("generateMessages() - no message de-queued\n");
			g_outbound_message_count = 0;
		}

		if (got_message_to_send) {
			modulateAndTransmit(baudotEncoding(message_to_send));
			last_message_processed = currentMillis();
			free(message_to_generate);
		}
		pthread_mutex_unlock(&g_messageQueueMutex);
	}

	LOG("generateMessages() - Generate Messages exiting\n");

	outbound_message_t *message_to_free = NULL;

	pthread_mutex_lock(&g_messageQueueMutex);

	for(message_to_free = g_outbound_messages; message_to_free != NULL; message_to_free = message_to_free->next) {

		g_outbound_messages = message_to_free->next;
		g_outbound_messages->prev = NULL;
		free(message_to_free);
		g_outbound_message_count--;
	}

	pthread_mutex_unlock(&g_messageQueueMutex);
	LOG("generateMessages() - Generate Messages exited\n");

	return 0;
}

static circular_buffer_t *createCircularBuffer()
{
	circular_buffer_t *circular_buffer = (circular_buffer_t *) malloc(sizeof(circular_buffer_t));
	memset(circular_buffer, 0, sizeof(circular_buffer_t));
	circular_buffer->size = CIRCULAR_BUFFER_NUMBER_FRAMES * PREFERRED_FRAME_SIZE;
	circular_buffer->buffer = (char *) malloc(circular_buffer->size);
	pthread_mutex_init(&circular_buffer->mutex, NULL);
	return circular_buffer;
}

static void destroyCircularBuffer(circular_buffer_t *circular_buffer)
{
	pthread_mutex_destroy(&circular_buffer->mutex);
	free(circular_buffer->buffer);
	free(circular_buffer);
}

static void writeByteToCircularBuffer(circular_buffer_t* circular_buffer, char val)
{
	++circular_buffer->length;
	circular_buffer->buffer[circular_buffer->tail] = val;
	++circular_buffer->tail;
	circular_buffer->tail %= circular_buffer->size;
}

static char readByteFromCircularBuffer(circular_buffer_t* circular_buffer)
{
	--circular_buffer->length;
	char val = circular_buffer->buffer[circular_buffer->head];
	++circular_buffer->head;
	circular_buffer->head %= circular_buffer->size;
	return val;
}

static bool writeToCircularBuffer(circular_buffer_t* circular_buffer, char* buffer, int number_bytes)
{
	if (circular_buffer->length + number_bytes > circular_buffer->size) {
		return false;
	}
	pthread_mutex_lock(&circular_buffer->mutex);
	int i;
	for (i = 0; i < number_bytes; ++i) {
		writeByteToCircularBuffer(circular_buffer, buffer[i]);
	}
	pthread_mutex_unlock(&circular_buffer->mutex);
	return true;
}

static bool readFromCircularBuffer(circular_buffer_t* circular_buffer, char buffer[], int number_bytes)
{
	if (circular_buffer->length < number_bytes) {
		return false;
	}
	int i;
	pthread_mutex_lock(&circular_buffer->mutex);
	for (i = 0; i < number_bytes; ++i) {
		buffer[i] = readByteFromCircularBuffer(circular_buffer);
	}
	pthread_mutex_unlock(&circular_buffer->mutex);
	return true;
}

static int baudotEncoding(int number)
{
	int character_to_send = number - 0x30; // map ascii "0" to 0x00, etc. primitive encoding scheme
	int parity = calculateParityBits(character_to_send);
	int message = character_to_send + parity;
	LOG("createMessage() - character_to_send=%d, parity=%d, message=%d\n", character_to_send, parity, message);
	return message;
}

static void modulateAndTransmit(int char_to_send)
{
	LOG("modulateAndTransmit() - entered - char_to_send=%d\n", char_to_send);

	int bit_size = ENCODING_SAMPLES_PER_BIT*2;
	int silence_size = SILENCE_LENGTH * bit_size;
	int carrier_size = CARRIER_LENGTH * bit_size;
	int start_size = START_BIT * bit_size;
	int data_size = DATA_BITS * bit_size;
	int stop_size = STOP_BIT * bit_size;
	int end_size = END_BIT * bit_size;

	int16_t *silence = malloc(silence_size * sizeof(int16_t));
	int16_t *carrier = malloc(carrier_size * sizeof(int16_t));
	int16_t *start_bit = malloc(start_size * sizeof(int16_t));
	int16_t *data_bits = malloc(data_size * sizeof(int16_t));
	int16_t *stop_bit = malloc(stop_size * sizeof(int16_t));
	int16_t *end_bit = malloc(end_size * sizeof(int16_t));

	memset(silence, 0, silence_size * sizeof(int16_t));
	memset(carrier, 0, carrier_size * sizeof(int16_t));
	memset(start_bit, 0, start_size * sizeof(int16_t));
	memset(data_bits, 0, data_size * sizeof(int16_t));
	memset(stop_bit, 0, stop_size * sizeof(int16_t));
	memset(end_bit, 0, end_size * sizeof(int16_t));

	int previous_frequency = FREQUENCY_HIGH;
	bool previous_phase_change = false;

	int this_frequency = FREQUENCY_HIGH;

	bool this_phase_change = thisPhase(previous_frequency, previous_phase_change, this_frequency);
	generateTone(carrier, carrier_size, this_frequency, this_phase_change);

	previous_frequency = this_frequency;
	previous_phase_change = false;

	this_frequency = FREQUENCY_LOW;
	this_phase_change = thisPhase(previous_frequency, previous_phase_change, this_frequency);
	generateTone(start_bit, start_size, this_frequency, this_phase_change);

	previous_frequency = this_frequency;
	previous_phase_change = this_phase_change;

	int data_bit;
	for (data_bit = 0; data_bit < DATA_BITS; data_bit++) {

		if (char_to_send & _BV(data_bit)) {
			this_frequency = FREQUENCY_HIGH;
		} else {
			this_frequency = FREQUENCY_LOW;
		}
		this_phase_change = thisPhase(previous_frequency, previous_phase_change, this_frequency);
		generateTone(data_bits + data_bit*bit_size, bit_size, this_frequency, this_phase_change);

		previous_frequency = this_frequency;
		previous_phase_change = this_phase_change;
	}

	this_frequency = FREQUENCY_HIGH;
	this_phase_change = thisPhase(previous_frequency, previous_phase_change, this_frequency);
	generateTone(stop_bit, stop_size, this_frequency, this_phase_change);

	previous_frequency = this_frequency;
	previous_phase_change = this_phase_change;

	this_frequency = FREQUENCY_HIGH;
	this_phase_change = thisPhase(previous_frequency, previous_phase_change, this_frequency);
	generateTone(end_bit, end_size, FREQUENCY_HIGH, this_phase_change);

	// Asynchronous Serial Data Transmission
	writeTone(silence, silence_size);
	writeTone(carrier, carrier_size);
	writeTone(start_bit, start_size);
	writeTone(data_bits, data_size);
	writeTone(stop_bit, stop_size);
	writeTone(end_bit, end_size);
	writeTone(silence, silence_size);

	free(silence);
	free(carrier);
	free(start_bit);
	free(data_bits);
	free(stop_bit);
	free(end_bit);

	LOG("modulateAndTransmit() - exited\n");
}

static bool thisPhase(int previous_frequency, bool previous_phase_change, int this_frequency) {
	bool this_phase_change = false;

	if (this_frequency == FREQUENCY_HIGH) {
		if (previous_frequency == FREQUENCY_HIGH) {
			if (previous_phase_change) {
				this_phase_change = false;
			} else {
				this_phase_change = true;
			}
		} else {
			if (previous_phase_change) {
				this_phase_change = true;
			} else {
				this_phase_change = false;
			}
		}
	} else {
		if (previous_frequency == FREQUENCY_HIGH) {
			if (previous_phase_change) {
				this_phase_change = false;
			} else {
				this_phase_change = true;
			}
		} else {
			if (previous_phase_change) {
				this_phase_change = true;
			} else {
				this_phase_change = false;
			}
		}
	}
	return this_phase_change;
}

static void generateTone(int16_t *start, int length, int frequency, bool phase_shift) {
	LOG("generateTone() - start=%d, length=%d, frequency=%d\n", (int)start, length, frequency);

	double current_angle;
	double amplitude = SOUND_AMPLITUDE;
	double phase;
	if (phase_shift) {
		phase = -1.0;
	} else {
		phase = 1.0;
	}

    int index;
    for (index = 0; index < length; index ++) {
		current_angle = fmod(((double)index * 2.0 * M_PI * ((double)frequency/(double)SAMPLING_FREQUENCY)), 2.0 * M_PI);
		start[index] = (int16_t)(phase * amplitude * sin(current_angle));
    }
}

static void writeTone(int16_t *start, int length) {

	int size = length * sizeof(int16_t);

	snd_pcm_channel_status_t status;

	int error = snd_pcm_plugin_write(g_pcm_handle_p, start, size);
	if( error != size ) {
		memset (&status, 0, sizeof(status));

	   status.channel = SND_PCM_CHANNEL_PLAYBACK;

	   if ((error = snd_pcm_plugin_status(g_pcm_handle_p, &status)) < 0) {
			LOG("writeTone() - retrieving audio interface status failed (%s)\n", snd_strerror (error));

	   } else if (status.status == SND_PCM_STATUS_READY ||
				   status.status == SND_PCM_STATUS_UNDERRUN ||
				   status.status == SND_PCM_STATUS_CHANGE) {

		   if ((error = snd_pcm_plugin_prepare (g_pcm_handle_p, SND_PCM_CHANNEL_PLAYBACK)) < 0) {
			   LOG("writeTone() - cannot prepare audio interface for use (%s)\n", snd_strerror (error));
			}
		} else {
			LOG("writeTone() - non-underrun write failure (%s)\n", snd_strerror (error));
		}
		error = snd_pcm_plugin_write (g_pcm_handle_p, start, size);
	}
	if( error != size ) {
		LOG("writeTone() - write to audio interface failed (%s) %d\n", snd_strerror (error), error);
	}
}

static bool carrierDetected(int16_t *sound, int length)
{
	g_carrier_detected = carrierPresent(sound,length);
	return g_carrier_detected;
}

static bool carrierPresent(int16_t *sound, int length)
{
	int number_points = SAMPLES_PER_SLOT;
	int number_parts = length / number_points;
	int number_peaks = 0;
	int start_index = 0;
	int i = 0;

	do {
		int end_index = start_index + number_points;
		int n = countPeaksInInterval(sound, start_index, end_index);
		number_peaks += n;
		i++;
		start_index = end_index;
		if (number_peaks > MINUMUM_NPEAKS) {
			return true;
		}
	} while (i < number_parts);

	return false;
}

static int countPeaksInInterval(int16_t *sound, int start_index, int end_index)
{
	int index = start_index;
	int sign_change_counter = 0;
	int number_samples_greater_thresdhold = 0;
	int sign = 0;

	do {
		int16_t value = sound[index];

		if (abs(value) > PEAK_AMPLITUDE_TRESHOLD) {
			number_samples_greater_thresdhold++;
		}

		if ((sign == 0) & (number_samples_greater_thresdhold > 0)) {
			sign = 1;
			if (value != 0) {
				sign = (int) (value / abs(value));
			}
		}

		bool sign_changed = false;

		if ((sign < 0) & (value > 0)) sign_changed = true;
		if ((sign > 0) & (value < 0)) sign_changed = true;

		if (sign_changed & (number_samples_greater_thresdhold > NUMBER_SAMPLES_PEAK)) {
			sign_change_counter++;
			sign = -sign;
		}

		index++;

	} while (index < end_index);

	return sign_change_counter;
}

static void decodeSound(int16_t *sound, int length)
{
	int message = fskDecode(sound, length);
	message = validateParityBits(message);

	if (message > 0) {
		char buffer[100];
		g_total_messages_accepted++;
		message += 0x30; // map back to ascii character 0x00 -> "0" etc
		sprintf(buffer, "Messsage received: %c", message);
		EMIT_MESSAGE(buffer);                    \
		LOG("decodeSound() - %s\n", buffer);
	} else {
		g_total_messages_parity_errors++;
	}
}

static int fskDecode(int16_t *sound, int length)
{
	int *number_peaks = 0;
	int number_peaks_length = 0;
	int *bits = 0;
	int bits_length = 0;
	int message = 0;

	scanForPeaks(sound, length, &number_peaks, &number_peaks_length);
	if (number_peaks_length == 0) return -1;
	scanForBits(number_peaks, number_peaks_length, &bits, &bits_length);
	decodeMessage(bits, bits_length, number_peaks, number_peaks_length, &message);
	free(bits);
	free(number_peaks);

	return message;
}

static int validateParityBits(int message)
{
	int mask = PARITY_BITS_MASK;
	int number = message & mask;
	int check = message - number;

	int parity_bits = calculateParityBits(number);

	if ( parity_bits != check) {
		return PARITY_BITS_ERROR;
	} else {
		return number;
	}
}

static int calculateParityBits(int number)
{
	unsigned int i;
	int counter = 0;

	for (i = 0; i < (8 * sizeof(number)); i++) {
		if (number & _BV(i)) counter++;
	}
	if (counter % 2 == 0) {
		return PARITY_EVEN;
	} else {
		return PARITY_ODD;
	}
}

static void scanForPeaks(int16_t *sound, int length, int **n_peaks, int *n_peaks_length)
{
	int number_points = SAMPLES_PER_SLOT;
	int number_parts = length / number_points;

	int number_parts_length = number_parts * sizeof(int);
	int *number_parts_data = (int *) malloc(number_parts_length);
	memset(number_parts_data, 0, number_parts_length);

	*n_peaks_length = number_parts;
	*n_peaks = number_parts_data;

	int start_index = 0;
	int i = 0;
	int peak_counter = 0;

	do {
		int end_index = start_index + number_points;
		int n = countPeaksInInterval(sound, start_index, end_index);
		number_parts_data[i] = n;
		peak_counter += n;
		i++;
		start_index = end_index;

	} while (i < number_parts);

	if (peak_counter < MINUMUM_NPEAKS) {
		*n_peaks_length = 0;
		*n_peaks = 0;
		free(number_parts_data);
	}
}

static void scanForBits(int *peaks, int peaks_length, int **bits, int *bits_length)
{
	int i = 0;
	int low_counter = 0;
	int high_counter = 0;
	int number_bits = peaks_length / SLOTS_PER_BIT;

	int n_bits_length = number_bits * sizeof(int);
	int *bits_p = (int *) malloc(n_bits_length);
	memset(bits_p, 0, n_bits_length);

	*bits_length = number_bits;
	*bits = bits_p;

	i = findNextNonZero(peaks, peaks_length, i);

	if (i + SLOTS_PER_BIT >= peaks_length) {
		return;
	}

	do {
		int number_peaks = 0;
		int j;

		for (j = 0; j < SLOTS_PER_BIT; j++) {
			number_peaks += peaks[i+j];
		}
		int position = i / SLOTS_PER_BIT;
		bits_p[position] = BIT_NONE_SYMBOL;

		if (number_peaks >= LOW_BIT_N_PEAKS) {
			bits_p[position] = BIT_LOW_SYMBOL;
			low_counter++;
		}
		if (number_peaks >= HIGH_BIT_N_PEAKS ) {
			bits_p[position] = BIT_HIGH_SYMBOL;
			high_counter++;
		}
		i += SLOTS_PER_BIT;

	} while (i + SLOTS_PER_BIT < peaks_length);
}

static int findNextNonZero(int *peaks, int peaks_length, int start_index)
{
	int index = start_index;
	int value = 1;
	do {
		value = peaks[index];
		index++;
	} while ((value == 0) && (index < (peaks_length - 1)));
	return (index - 1);
}

static int findStartBit(int *bits, int bits_length, int start_index)
{
	/*
	 * 	Total message format = >23(bits) = >12(high) + 1(low startbit) + 8(data) + 1(stopbit) + 1(endbit)
	 * 	so need to find first low bit after a long run of high bits -- need CARRIER_MIN_HIGH_BITS before start bit
	 */
	int index = start_index;
	int high_counter = 0;
	bool start_bit_detected = false;

	do {
		int bit = bits[index];
		switch (bit) {
		case BIT_HIGH_SYMBOL:
			high_counter++;
			break;

		case BIT_LOW_SYMBOL:
			if (high_counter > CARRIER_MIN_HIGH_BITS) {
				start_bit_detected = true;
			} else {
				high_counter = 0;
			}
			break;

		case BIT_NONE_SYMBOL:
			high_counter = 0;
			break;
		}
		index++;
		if (index >= bits_length) return -1;
	} while (!start_bit_detected);

	return index;
}

static void decodeMessage(int *bits, int bits_length, int *n_peaks, int n_peaks_length, int *message)
{
	/*
	 * 	Total message format = 166(bits) = 155(high) + 1(low startbit) + 8(data) + 1(stopbit) + 1(endbit)
	 * 	find the start bit
	 */
	int index = findStartBit(bits, bits_length, 0);

	if (index == -1) {
		*message = -1;
		return;
	}
	/*
	 * The resudual part of the message should be: 8(data) + 1(stopbit) + 1(endbit)
	 * check that we don't go over end of range in our search
	 */
	if (index + DATA_BITS + STOP_BIT + END_BIT > bits_length) {
		*message = -2;
		return;
	}

	int extracted_message = extractMessage(n_peaks, n_peaks_length, index);
	*message = extracted_message;
}

static int extractMessage(int *n_peaks, int n_peaks_length, int start_bit)
{
	(void)n_peaks_length; //suppress warning message

	int message_frame = 0;
	int message_data = 0;
	int index = (start_bit + DATA_BITS + STOP_BIT + END_BIT + 2) * SLOTS_PER_BIT;
	int i, j;
	/*
	 * Work back to locate start of message
	 */
	for (i = 0; i < index; i++) {
		int number_peaks_2 = n_peaks[index-i];
		int number_peaks_1 = n_peaks[index-i-1];
		if ((number_peaks_1-number_peaks_2) > PEAK_THRESHOLD_DIFF) {
			index -= (i+1);
			break;
		}
	}

	/*
	 * index should be pointing to slot +1 after end bit
	 */
	int bits_length = HIGH_BIT + START_BIT + DATA_BITS + STOP_BIT + END_BIT + 1;
	int bits[bits_length];

	for (i = 0; i < bits_length; i++) {
		int peak_counter = 0;
		for (j = 0; j < 4; j++) {
			peak_counter += n_peaks[index-j];
		}
		if (peak_counter > LOW_BIT_N_PEAKS)  {
			bits[i] = BIT_LOW_SYMBOL;
		}

		if (peak_counter > HIGH_BIT_N_PEAKS)  {
			bits[i] = BIT_HIGH_SYMBOL;
			message_frame |= _BV(i);
		}
		index -= 4;
	}

	/*
	 * Extract the message data from frame if it's valid ( correct high, start, stop and end bits)
	 */
	if ((message_frame & _BV(0)) && (message_frame & _BV(1)) && (message_frame & _BV(11)) && !(message_frame & _BV(10))) {
		for (i = HIGH_BIT + START_BIT; i < HIGH_BIT + START_BIT + DATA_BITS; i++) {
			if ( bits[i] == BIT_HIGH_SYMBOL) {
				message_data |= _BV((DATA_BITS-1)-(i-(HIGH_BIT + START_BIT)));
			}
		}
		char buffer[100];
		sprintf(buffer, "Candidate frame: ");
		for (i = 0; i < (HIGH_BIT + START_BIT + HIGH_BIT + START_BIT + DATA_BITS); i++) {
			strcat(buffer, ((message_frame & _BV(HIGH_BIT + START_BIT + HIGH_BIT + START_BIT + DATA_BITS-1-i)) ? "1" : "0"));
		}
		LOG("extractMessage() - %s accepted\n", buffer);
	} else {
		g_total_messages_frame_errors++;
		message_data = -3;
	}

	return message_data;
}

static void setVolume(int level)
{
	LOG("setVolume() - level=%d\n", level);

	int volume;
    snd_mixer_t *mixer_handle;
	snd_mixer_group_t mixer_group;

	if (snd_mixer_open (&mixer_handle, g_card_p, g_device_p) < 0) {
    	LOG("setVolume() - Unable to open mixer\n");
        return;
    }

    snd_mixer_group_read(mixer_handle, &mixer_group);

    volume = (float)(mixer_group.max - mixer_group.min) * ( level / 100.0) + mixer_group.min;

	LOG("setVolume() - mixer volume=%d\n", volume);

	if (mixer_group.channels & SND_MIXER_CHN_MASK_FRONT_LEFT)
    	mixer_group.volume.names.front_left = volume;
    if (mixer_group.channels & SND_MIXER_CHN_MASK_REAR_LEFT)
    	mixer_group.volume.names.rear_left = volume;
    if (mixer_group.channels & SND_MIXER_CHN_MASK_FRONT_CENTER)
    	mixer_group.volume.names.front_center = volume;
    if (mixer_group.channels & SND_MIXER_CHN_MASK_FRONT_RIGHT)
    	mixer_group.volume.names.front_right = volume;
    if (mixer_group.channels & SND_MIXER_CHN_MASK_REAR_RIGHT)
    	mixer_group.volume.names.rear_right = volume;
    if (mixer_group.channels & SND_MIXER_CHN_MASK_WOOFER)
    	mixer_group.volume.names.woofer = volume;
    snd_mixer_group_write(mixer_handle, &mixer_group);

    snd_mixer_close(mixer_handle);
}

static _uint64 currentMillis()
{
	_uint64 millis = 0;
	struct timeb time;

	if (!ftime(&time)) {
		millis = (_uint64)time.time * (_uint64)1000 + (_uint64)time.millitm;
	}
	return millis;
}
