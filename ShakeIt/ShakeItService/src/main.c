/*
 * Copyright (c) 2013 BlackBerry Limited.
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

#include <bps/bps.h>
#include <bps/navigator.h>
#include <bps/navigator_invoke.h>
#include <bps/led.h>
#include <bps/sensor.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <sys/siginfo.h>

#define UNUSED(x) (void)(x)

#define SAVE_FILE "./data/save.txt"
#define DEFAULT_BLINK_PATTERN "red,1,green,2,blue,3"
#define SHAKE_THRESHOLD 20.0f

typedef struct blink_step_t blink_step_t;

struct blink_step_t {
    led_color_t          color;
    unsigned int         seconds;
    unsigned int         time_spent;
    char                 request_id[50];
    blink_step_t         *next;
};

static struct {
    struct sigevent      sig;
    timer_t              timer;
    int                  request_count;
    blink_step_t         *start_step;
    blink_step_t         *current_step;
} g_app = {
    .request_count = 0,
    .start_step = NULL,
    .current_step = NULL
};

/**
 * Persist the blink instructions to a file.  That way we know the right
 * pattern on system restarts.
 */
static void save_instructions(const char *instructions) {
	FILE *fp = fopen(SAVE_FILE, "w");

    if (!fp) {
        fprintf(stderr, "Error opening save file\n");
        return;
    }

    if (fputs(instructions, fp) < 0) {
        fprintf(stderr, "Error writing to save file: %d\n", ferror(fp));
    }

    fclose(fp);
    return;
}

/**
 * Load persisted blink instructions.  If there are any saved instructions
 * return 0, else return -1.
 */
static int load_instructions(char *buffer, int size) {
    FILE *fp = fopen(SAVE_FILE, "r");

    if (!fp) {
        fprintf(stderr, "Error opening save file\n");
        return -1;
    }

    if (fgets(buffer, size, fp) == NULL) {
        fprintf(stderr, "Error reading save file: %d", ferror(fp));
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}

/**
 * Sets up our callback to be fired after the passed in number of seconds.
 * A passed in value of 0 means the timer is disabled.
 */
static void set_timer(unsigned int seconds) {
    struct itimerspec spec;

    spec.it_value.tv_sec = seconds;
    spec.it_value.tv_nsec = 0;
    spec.it_interval.tv_sec = 0;
    spec.it_interval.tv_nsec = 0;

    timer_settime(g_app.timer, 0, &spec, 0);
}

/**
 * Begin execution of the passed in blink step.
 */
static void run_blink_step(blink_step_t *step) {
    g_app.current_step = step;
    step->time_spent = 0;

    sprintf(step->request_id, "accmon-%d", g_app.request_count++);
    led_request_color(step->request_id, step->color, 1);

    set_timer(1);
}

/**
 * Parse the blink pattern instructions.  They are in the format
 * "([red|green|blue],seconds)(,[red|green|blue],seconds)*"
 * This string will be parsed and a sequence of blink steps will
 * be attached to g_app.
 */
static int create_blink_steps(const char *instructions) {
    blink_step_t *step = NULL;
    char *tokstr = strdup(instructions);

    g_app.start_step = NULL;

    // strip the newline
    char *nl = strrchr(tokstr, '\n');
    if (nl) {
        nl[0] = '\0';
    }

    if (tokstr) {
        char *part = strtok(tokstr, ",");
        int count = 0;
        while (part) {
            if ((count % 2) == 0) {
                // we are looking at color
                if (!g_app.start_step) {
                    step = calloc(1, sizeof(*step));
                    g_app.start_step = step;
                } else {
                    step->next = calloc(1, sizeof(*step));
                    step = step->next;
                }

                if (strcmp("red", part) == 0) {
                    step->color = LED_COLOR_RED;
                } else if (strcmp("green", part) == 0) {
                    step->color = LED_COLOR_GREEN;
                } else if (strcmp("blue", part) == 0) {
                    step->color = LED_COLOR_BLUE;
                }
            } else {
                // we are looking at time
                step->seconds = atoi(part);
            }

            part = strtok(NULL, ",");
            count++;
        }

        free(tokstr);
    }

    if (!g_app.start_step) {
        // Nothing useful was parsed
        return -1;
    }

    return 0;
}

/**
 * The callback that is fired as a result of calling set_timer.  We check if we
 * are still executing the same blink step, or moving on to the next step.
 */
static int timer_fired(void *data) {
    UNUSED(data);

    led_cancel(&g_app.current_step->request_id[0]);

    blink_step_t *step = g_app.current_step;
    step->time_spent++;

    if (step->time_spent < step->seconds) {
        // Blink again
        led_request_color(step->request_id, step->color, 1);
        set_timer(1);
    } else if (step->next) {
        // Move on to the next part of the blink pattern
        run_blink_step(g_app.current_step->next);
    } else {
        // All done the pattern.  Stop blinking.
        g_app.current_step = NULL;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);

    int rc;

    rc = bps_initialize();
    if (rc != BPS_SUCCESS) {
        return EXIT_FAILURE;
    }

    // Request various events we'll need
    navigator_request_events(0);

    // Make sure the sensor is still on even though our headless app runs in
    // the background.
    sensor_set_background(SENSOR_TYPE_ACCELEROMETER, true);

    // Skip duplicate accelerometer events so that we don't waste cycles
    // processing near similar events.
    sensor_set_skip_duplicates(SENSOR_TYPE_ACCELEROMETER, true);

    led_request_events(0);

    // Setup BPS to work with a timer.  This allows us to use a callback with
    // timers firing.
    if (bps_add_sigevent_handler(&g_app.sig, &timer_fired, 0) != BPS_SUCCESS) {
        return EXIT_FAILURE;
    }

    // Hook up a timer with our callback.
    if (timer_create(CLOCK_MONOTONIC, &g_app.sig, &g_app.timer) != 0) {
        return EXIT_FAILURE;
    }

    // Since the headless app starts on system boot, we persist the last user
    // selected blink pattern.  If the user hasn't chosen one yet, we create a
    // default one.
    char instructions[100];
    if (load_instructions(instructions, sizeof(instructions)) == 0) {
        if (create_blink_steps(instructions) == 0) {
            sensor_request_events(SENSOR_TYPE_ACCELEROMETER);
        }
    } else {
        create_blink_steps(DEFAULT_BLINK_PATTERN);
        sensor_request_events(SENSOR_TYPE_ACCELEROMETER);
    }

    bps_event_t *event;

    bool running = true;

    // Start the event loop.  We are looking at both invoke events coming in
    // with new instructions from our UI brother, and navigator lifecycle
    // messages like exit.
    while (running) {
        rc = bps_get_event(&event, -1);

        if (BPS_SUCCESS == rc && NULL != event) {
            int domain = bps_event_get_domain(event);
            int code = bps_event_get_code(event);

            if (domain == navigator_get_domain()) {
                switch (code) {
                case NAVIGATOR_EXIT:
                    // Navigator has asked us to terminate.  So we will.  See
                    // how nice we are?
                    running = false;
                    break;
                case NAVIGATOR_INVOKE_TARGET:
                    {
                    // Read and parse instructions from the UI portion.  These
                    // come in as invokes, which were defined in the
                    // bar-descriptor.xml.
                    const navigator_invoke_invocation_t *invoke;
                    invoke = navigator_invoke_event_get_invocation(event);

                    if (invoke) {
                        const char* action = navigator_invoke_invocation_get_action(invoke);

                        // New blink step instructions.
                        if (g_app.current_step) {
                            // disable the timer.
                            set_timer(0);

                            g_app.current_step = NULL;
                            blink_step_t *step = g_app.start_step;
                            while (step) {
                                blink_step_t *prev = step;
                                step = step->next;
                                free(prev);
                            }
                            g_app.start_step = NULL;
                        }

                        if (strcmp(action, "SetBlinkSteps") == 0) {
                            const char *data = navigator_invoke_invocation_get_data(invoke);
                            unsigned int length = navigator_invoke_invocation_get_data_length(invoke);
                            char instructions[length+1];
                            strcpy(instructions, data);
                            instructions[length] = '\0';
                            if (create_blink_steps(instructions) == 0) {
                                // make sure the accelerometer is on
                                sensor_request_events(SENSOR_TYPE_ACCELEROMETER);
                            }
                            save_instructions(instructions);
                        } else if (strcmp(action, "ClearBlinkSteps") == 0) {
                            // stop the accelerometer since we have nothing to do.
                            sensor_stop_events(SENSOR_TYPE_ACCELEROMETER);
                        }
                    }
                    }
                    break;
                default:
                    break;
                }
            } else if (domain == sensor_get_domain() && code == SENSOR_ACCELEROMETER_READING) {
                // Check the accelerometer and see if the device was shaken
                // sufficiently hard to kick off a new blink sequence.
                float x, y, z;
                if (sensor_event_get_xyz(event, &x, &y, &z) == BPS_SUCCESS) {
                    if (x > SHAKE_THRESHOLD || y > SHAKE_THRESHOLD || z > SHAKE_THRESHOLD) {
                        if (!g_app.current_step && g_app.start_step) {
                            // start the sequence over again
                            run_blink_step(g_app.start_step);
                        }
                    }
                }
            }
        }
    }

    bps_shutdown();

    timer_delete(g_app.timer);

    return EXIT_SUCCESS;
}
