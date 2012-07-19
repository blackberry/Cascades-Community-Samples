/*
* Copyright (c) 2012 Research In Motion Limited.
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

#include <assert.h>
#include <bps/bps.h>
#include <bps/event.h>
#include <bps/navigator.h>
#include <bps/screen.h>
#include <fcntl.h>
#include <screen/screen.h>
#include <camera/camera_api.h>
#include <bps/soundplayer.h>


typedef enum {
    STATE_STARTUP = 0,
    STATE_VIEWFINDER,
    STATE_TAKINGPHOTO
} state_t;


static bool shutdown = false;
static screen_context_t screen_ctx;
static screen_window_t vf_win = NULL;
static const char vf_group[] = "viewfinder_window_group";
static state_t state = STATE_STARTUP;
static camera_handle_t handle = CAMERA_HANDLE_INVALID;
static bool shouldmirror = false;
static bool touch = false;
static int photo_done_domain = -1;
static int main_bps_chid = -1;


static void
handle_screen_event(bps_event_t *event)
{
    int screen_val;

    screen_event_t screen_event = screen_event_get_event(event);
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE, &screen_val);

    switch (screen_val) {
    case SCREEN_EVENT_MTOUCH_TOUCH:
        fprintf(stderr,"Touch event\n");
        touch = true;
        break;
    case SCREEN_EVENT_MTOUCH_MOVE:
        fprintf(stderr,"Move event\n");
        break;
    case SCREEN_EVENT_MTOUCH_RELEASE:
        fprintf(stderr,"Release event\n");
        break;
    case SCREEN_EVENT_CREATE:
        if (screen_get_event_property_pv(screen_event, SCREEN_PROPERTY_WINDOW, (void **)&vf_win) == -1) {
            perror("screen_get_event_property_pv(SCREEN_PROPERTY_WINDOW)");
        } else {
            fprintf(stderr,"viewfinder window found!\n");
            // mirror viewfinder if this is the front-facing camera
            int i = (shouldmirror?1:0);
            screen_set_window_property_iv(vf_win, SCREEN_PROPERTY_MIRROR, &i);
            // place viewfinder in front of the black application background window
            // child window z-orders are relative to their parent, not absolute.
            i = +1;
            screen_set_window_property_iv(vf_win, SCREEN_PROPERTY_ZORDER, &i);
            // make viewfinder window visible
            i = 1;
            screen_set_window_property_iv(vf_win, SCREEN_PROPERTY_VISIBLE, &i);
            screen_flush_context(screen_ctx, 0);
            // we should now have a visible viewfinder
            touch = false;
            state = STATE_VIEWFINDER;
        }
        break;
    default:
        break;
    }
}


static void
handle_navigator_event(bps_event_t *event) {
    switch (bps_event_get_code(event)) {
    case NAVIGATOR_SWIPE_DOWN:
        fprintf(stderr,"Swipe down event");
        break;
    case NAVIGATOR_EXIT:
        fprintf(stderr,"Exit event");
        shutdown = true;
        break;
    default:
        break;
    }
    fprintf(stderr,"\n");
}


static void
handle_photo_done_event(bps_event_t *event) {
    // re-arm the viewfinder state
    fprintf(stderr, "received photo-done event\n");
    touch = false;
    state = STATE_VIEWFINDER;
}


static void
handle_event()
{
    int rc, domain;

    bps_event_t *event = NULL;
    rc = bps_get_event(&event, -1);
    assert(rc == BPS_SUCCESS);
    if (event) {
        domain = bps_event_get_domain(event);
        if (domain == navigator_get_domain()) {
            handle_navigator_event(event);
        } else if (domain == screen_get_domain()) {
            handle_screen_event(event);
        } else if (domain == photo_done_domain) {
            handle_photo_done_event(event);
        }
    }
}


static void
shutter_callback(camera_handle_t handle,
                 void* arg)
{
    // THE CAMERA SERVICE DOES NOT PLAY SOUNDS WHEN PICTURES ARE TAKEN OR
    // VIDEOS ARE RECORDED.  IT IS THE APP DEVELOPER'S RESPONSIBILITY TO
    // PLAY AN AUDIBLE SHUTTER SOUND WHEN A PICTURE IS TAKEN AND WHEN VIDEO
    // RECORDING STARTS AND STOPS.  NOTE THAT WHILE YOU MAY CHOOSE TO MUTE
    // SUCH SOUNDS, YOU MUST ENSURE THAT YOUR APP ADHERES TO ALL LOCAL LAWS
    // OF REGIONS WHERE IT IS DISTRIBUTED.  FOR EXAMPLE, IT IS ILLEGAL TO
    // MUTE OR MODIFY THE SHUTTER SOUND OF A CAMERA APPLICATION IN JAPAN OR
    // KOREA.
    // TBD:
    //   RIM will be providing clarification of this policy as part of the
    //   NDK developer agreement and App World guidelines.  A link will
    //   be provided when the policy is publicly available.
    soundplayer_play_sound("event_camera_shutter");
}


static void
still_callback(camera_handle_t handle,
               camera_buffer_t* buf,
               void* arg)
{
    if (buf->frametype == CAMERA_FRAMETYPE_JPEG) {
        fprintf(stderr, "still image size: %lld\n", buf->framedesc.jpeg.bufsize);
        int fd;
        char filename[CAMERA_ROLL_NAMELEN];
        if (camera_roll_open_photo(handle,
                                   &fd,
                                   filename,
                                   sizeof(filename),
                                   CAMERA_ROLL_PHOTO_FMT_JPG) == CAMERA_EOK) {
            fprintf(stderr, "saving: %s\n", filename);
            int index = 0;
            while(index < (int)buf->framedesc.jpeg.bufsize) {
                int rc = write(fd, &buf->framebuf[index], buf->framedesc.jpeg.bufsize-index);
                if (rc > 0) {
                    index += rc;
                } else if (rc == -1) {
                    if ((errno == EAGAIN) || (errno == EINTR)) continue;
                    perror("write");
                    break;
                }
            }
            close(fd);
        }
    }
    // picture-taking is done, so wake up the main thread again via bps.
    // note that we are using the void* arg here as the bps channel to deliver
    // the event on.  this is just to demonstrate data passing between
    // camera_take_photo() and the various callback functions.
    bps_event_t* photo_done_event;
    bps_event_create(&photo_done_event, photo_done_domain, 0, NULL, NULL);
    bps_channel_push_event((int)arg, photo_done_event);
}


static void
run_state_machine()
{
    camera_error_t err;
    // this simple state machine just runs us through starting a viewfinder and taking pictures
    switch(state) {
    case STATE_STARTUP:
        // waiting for viewfinder...
        break;
    case STATE_VIEWFINDER:
        // viewfinder is visible.
        // if the user touches the screen anywhere, take a picture.
        // note, we are passing main_bps_chid as the void* arg which will then
        // be delivered to all callbacks. main_bps_chid is already a global variable,
        // so this isn't necessary, but is just done here to illustrate the convention.
        if (touch) {
            touch = false;
            err = camera_take_photo(handle,
                                    &shutter_callback,
                                    NULL,
                                    NULL,
                                    &still_callback,
                                    (void*)main_bps_chid,
                                    false);
            if (err != CAMERA_EOK) {
                fprintf(stderr, "camera_take_photo() error %d", err);
            } else {
                state = STATE_TAKINGPHOTO;
            }
        }
        break;
    default:
        break;
    }
}


static int
init_camera(camera_unit_t unit)
{
    camera_error_t err;
    // open the specified camera
    err = camera_open(unit,
                      CAMERA_MODE_RW | CAMERA_MODE_ROLL,
                      &handle);
    if (err != CAMERA_EOK) {
        fprintf(stderr, "camera_open() failed: %d\n", err);
        return err;
    }
    err = camera_set_photovf_property(handle,
                                      CAMERA_IMGPROP_WIN_GROUPID, vf_group,
                                      CAMERA_IMGPROP_WIN_ID, "my_viewfinder");
    if (err != CAMERA_EOK) {
        fprintf(stderr, "camera_set_photovf_property() failed: %d\n", err);
    } else {
        err = camera_start_photo_viewfinder(handle,
                                            NULL,
                                            NULL,
                                            NULL);
        if (err != CAMERA_EOK) {
            fprintf(stderr, "camera_start_photo_viewfinder() failed: %d\n", err);
        } else {
            // successfully started viewfinder
            // if it's a front-facing camera, we should mirror the viewfinder once
            // we receive it.
            if (unit == CAMERA_UNIT_FRONT) {
                shouldmirror = true;
            }
            return 0;
        }
    }
    // clean up on error
    camera_close(handle);
    handle = CAMERA_HANDLE_INVALID;
    return err;
}


int
main(int argc, char **argv)
{
    const int usage = SCREEN_USAGE_NATIVE;

    screen_window_t screen_win;
    screen_buffer_t screen_buf = NULL;
    int rect[4] = { 0, 0, 0, 0 };

    // create an application window which will just act as a background
    screen_create_context(&screen_ctx, 0);
    screen_create_window(&screen_win, screen_ctx);
    screen_create_window_group(screen_win, vf_group);
    screen_set_window_property_iv(screen_win, SCREEN_PROPERTY_USAGE, &usage);
    screen_create_window_buffers(screen_win, 1);
    screen_get_window_property_pv(screen_win, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)&screen_buf);
    screen_get_window_property_iv(screen_win, SCREEN_PROPERTY_BUFFER_SIZE, rect+2);

    // fill the window with black
    int attribs[] = { SCREEN_BLIT_COLOR, 0x00000000, SCREEN_BLIT_END };
    screen_fill(screen_ctx, screen_buf, attribs);
    screen_post_window(screen_win, screen_buf, 1, rect, 0);

    // Signal bps library that navigator and screen events will be requested
    bps_initialize();
    main_bps_chid = bps_channel_get_active();
    screen_request_events(screen_ctx);
    navigator_request_events(0);

    // create a custom BPS event that we can use in to let our main thread know
    // that photo-taking is finished
    photo_done_domain = bps_register_domain();

    // open camera and configure viewfinder
    if (init_camera(CAMERA_UNIT_REAR) == EOK) {
        // our main loop just runs a state machine and handles input
        while (!shutdown) {
            run_state_machine();
            // Handle user input
            handle_event();
        }

        if (state == STATE_TAKINGPHOTO) {
            // wait for picture-taking to finish? TBD
            state = STATE_VIEWFINDER;
        }
        if (state == STATE_VIEWFINDER) {
            // clean up camera
            camera_stop_photo_viewfinder(handle);
            camera_close(handle);
        }
    }

    // Clean up
    screen_stop_events(screen_ctx);
    bps_shutdown();
    screen_destroy_window(screen_win);
    screen_destroy_context(screen_ctx);
    return 0;
}

