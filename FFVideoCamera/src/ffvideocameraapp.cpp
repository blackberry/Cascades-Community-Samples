/* Copyright (c) 2012 Martin M Reed
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
#include <bb/cascades/Application>
#include <bb/cascades/ForeignWindow>
#include <bb/cascades/Container>
#include <bb/cascades/StackLayout>
#include <bb/cascades/DockLayout>
#include <bb/cascades/DockLayoutProperties>
#include <bb/cascades/Button>
#include <bb/cascades/Label>
#include <bb/cascades/Page>

#include <camera/camera_api.h>
#include <screen/screen.h>
#include <bps/soundplayer.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <libgen.h>

#include "ffvideocameraapp.hpp"

using namespace bb::cascades;

#define SECOND 1000000
#define VIDEO_WIDTH 288
#define VIDEO_HEIGHT 512
//#define VIDEO_WIDTH 1080
//#define VIDEO_HEIGHT 1920
#define FILENAME (char*)"/accounts/1000/shared/camera/VID_TEST.mpg"

void vf_callback(camera_handle_t handle, camera_buffer_t* buf, void* arg);
void* start_encoding_thread(void* arg);

FFVideoCameraApp::FFVideoCameraApp()
        : mCameraHandle(CAMERA_HANDLE_INVALID), record(false)
{
    qDebug() << "FFVideoCameraApp";

    // create our foreign window
    // Using .id() in the builder is equivalent to mViewfinderWindow->setWindowId()
    mViewfinderWindow = ForeignWindow::create().id(QString("cameraViewfinder"));

    // NOTE that there is a bug in ForeignWindow in 10.0.6 whereby the
    // SCREEN_PROPERTY_SOURCE_SIZE is updated when windows are attached.
    // We don't want this to happen, so we are disabling WindowFrameUpdates.
    // What this means is that if the ForeignWindow geometry is changed, then
    // the underlying screen window properties are not automatically updated to
    // match.  You will have to manually do so by listening for controlFrameChanged
    // signals.  This is outside of the scope of this sample.
    mViewfinderWindow->setWindowFrameUpdateEnabled(false);

    QObject::connect(mViewfinderWindow, SIGNAL(windowAttached(unsigned long,
                    const QString &, const QString &)), this, SLOT(onWindowAttached(unsigned long,
                    const QString &,const QString &)));

    // NOTE that there is a bug in ForeignWindow in 10.0.6 whereby
    // when a window is detached, it's windowHandle is not reset to 0.
    // We need to connect a detach handler to implement a workaround.
    QObject::connect(mViewfinderWindow, SIGNAL(windowDetached(unsigned long,
                    const QString &, const QString &)), this, SLOT(onWindowDetached(unsigned long,
                    const QString &,const QString &)));

    // create a bunch of camera control buttons
    // NOTE: some of these buttons are not initially visible
    mStartFrontButton = Button::create("Front Camera");
    mStartRearButton = Button::create("Rear Camera");
    mStopButton = Button::create("Stop Camera");
    mStopButton->setVisible(false);
    mStartStopButton = Button::create("Record Start");
    mStartStopButton->setVisible(false);

    // connect actions to the buttons
    QObject::connect(mStartFrontButton, SIGNAL(clicked()), this, SLOT(onStartFront()));
    QObject::connect(mStartRearButton, SIGNAL(clicked()), this, SLOT(onStartRear()));
    QObject::connect(mStopButton, SIGNAL(clicked()), this, SLOT(onStopCamera()));
    QObject::connect(mStartStopButton, SIGNAL(clicked()), this, SLOT(onStartStopRecording()));
    mStatusLabel = Label::create("filename");
    mStatusLabel->setVisible(false);

    // using dock layout mainly.  the viewfinder foreign window sits in the center,
    // and the buttons live in their own container at the bottom.
    // a single text label sits at the top of the screen to report recording status.
    Container* container = Container::create().layout(DockLayout::create())
            .add(Container::create().layoutProperties(DockLayoutProperties::create()
            .horizontal(HorizontalAlignment::Center)
            .vertical(VerticalAlignment::Center))
            .add(mViewfinderWindow))
            .add(Container::create().layoutProperties(DockLayoutProperties::create()
            .horizontal(HorizontalAlignment::Left).vertical(VerticalAlignment::Top))
            .add(mStatusLabel))
            .add(Container::create().layoutProperties(DockLayoutProperties::create()
            .horizontal(HorizontalAlignment::Center).vertical(VerticalAlignment::Bottom))
            .layout(StackLayout::create().direction(LayoutDirection::LeftToRight))
            .add(mStartFrontButton)
            .add(mStartRearButton)
            .add(mStartStopButton)
            .add(mStopButton));

    Application::setScene(Page::create().content(container));

    pthread_mutex_init(&reading_mutex, 0);
    pthread_cond_init(&read_cond, 0);
}

FFVideoCameraApp::~FFVideoCameraApp()
{
    delete mViewfinderWindow;

    pthread_mutex_destroy(&reading_mutex);
    pthread_cond_destroy(&read_cond);
}

void FFVideoCameraApp::onWindowAttached(unsigned long handle, const QString &group, const QString &id)
{
    qDebug() << "onWindowAttached: " << handle << ", " << group << ", " << id;
    screen_window_t win = (screen_window_t) handle;

    // set screen properties to mirror if this is the front-facing camera
    int i = (mCameraUnit == CAMERA_UNIT_FRONT);
    screen_set_window_property_iv(win, SCREEN_PROPERTY_MIRROR, &i);

    // put the viewfinder window behind the cascades window
    i = -1;
    screen_set_window_property_iv(win, SCREEN_PROPERTY_ZORDER, &i);

    // make the window visible.  by default, the camera creates an invisible
    // viewfinder, so that the user can decide when and where to place it
    i = 1;
    screen_set_window_property_iv(win, SCREEN_PROPERTY_VISIBLE, &i);

    // There is a bug in ForeignWindow in 10.0.6 which defers window context
    // flushing until some future UI update.  As a result, the window will
    // not actually be visible until someone flushes the context.  This is
    // fixed in the next release.  For now, we will just manually flush the
    // window context.
    screen_context_t ctx;
    screen_get_window_property_pv(win, SCREEN_PROPERTY_CONTEXT, (void**) &ctx);
    screen_flush_context(ctx, 0);
}

void FFVideoCameraApp::onWindowDetached(unsigned long handle, const QString &group, const QString &id)
{
    // There is a bug in ForeignWindow in 10.0.6 whereby the windowHandle is not
    // reset to 0 when a detach event happens.  We must forcefully zero it here
    // in order for a re-attach to work again in the future.
    mViewfinderWindow->setWindowHandle(0);
}

int FFVideoCameraApp::createViewfinder(camera_unit_t cameraUnit, const QString &group, const QString &id)
{
    if (mCameraHandle != CAMERA_HANDLE_INVALID) return EBUSY;

    mCameraUnit = cameraUnit;

    if (camera_open(mCameraUnit, CAMERA_MODE_RW, &mCameraHandle) != CAMERA_EOK) return EIO;

    // configure viewfinder properties so our ForeignWindow can find the resulting screen window
    camera_set_videovf_property( mCameraHandle, CAMERA_IMGPROP_WIN_GROUPID, group.toStdString().c_str());
    camera_set_videovf_property( mCameraHandle, CAMERA_IMGPROP_WIN_ID, id.toStdString().c_str());

    camera_set_videovf_property( mCameraHandle,
            CAMERA_IMGPROP_WIDTH, VIDEO_WIDTH,
            CAMERA_IMGPROP_HEIGHT, VIDEO_HEIGHT);

    camera_set_video_property( mCameraHandle,
            CAMERA_IMGPROP_WIDTH, VIDEO_WIDTH,
            CAMERA_IMGPROP_HEIGHT, VIDEO_HEIGHT);

    if (camera_start_video_viewfinder(mCameraHandle, vf_callback, NULL, this) != CAMERA_EOK)
    {
        camera_close(mCameraHandle);
        mCameraHandle = CAMERA_HANDLE_INVALID;
        return EIO;
    }

    mStartFrontButton->setVisible(false);
    mStartRearButton->setVisible(false);
    mStopButton->setVisible(true);
    mStartStopButton->setText("Start Recording");
    mStartStopButton->setVisible(true);
    mStartStopButton->setEnabled(true);
    return EOK;
}

void FFVideoCameraApp::onStartFront()
{
    if (!mViewfinderWindow) return;
    const QString windowGroup = mViewfinderWindow->windowGroup();
    const QString windowId = mViewfinderWindow->windowId();
    createViewfinder(CAMERA_UNIT_FRONT, windowGroup, windowId);
}

void FFVideoCameraApp::onStartRear()
{
    if (!mViewfinderWindow) return;
    const QString windowGroup = mViewfinderWindow->windowGroup();
    const QString windowId = mViewfinderWindow->windowId();
    createViewfinder(CAMERA_UNIT_REAR, windowGroup, windowId);
}

void FFVideoCameraApp::onStopCamera()
{
    if (mCameraHandle == CAMERA_HANDLE_INVALID) return;

    // NOTE that closing the camera causes the viewfinder to stop.
    // When the viewfinder stops, it's window is destroyed and the
    // ForeignWindow object will emit a windowDetached signal.
    camera_close(mCameraHandle);
    mCameraHandle = CAMERA_HANDLE_INVALID;

    // reset button visibility
    mStartStopButton->setVisible(false);
    mStopButton->setVisible(false);
    mStartFrontButton->setVisible(true);
    mStartRearButton->setVisible(true);
}

void FFVideoCameraApp::onStartStopRecording()
{
    if (mCameraHandle == CAMERA_HANDLE_INVALID) return;

    if (record)
    {
        record = false;

        pthread_cond_signal(&read_cond);

        qDebug() << "stop requested";
        mStartStopButton->setText("Start Recording");
        mStopButton->setEnabled(true);
        mStatusLabel->setVisible(false);

        return;
    }

    qDebug() << "start requested";

    enum CodecID codec_id = CODEC_ID_MPEG2VIDEO;
    codec = avcodec_find_encoder(codec_id);

    if (!codec)
    {
        av_register_all();
        codec = avcodec_find_encoder(codec_id);

        if (!codec)
        {
            fprintf(stderr,"codec not found\n");
            exit(1);
            return;
        }
    }

    pVideoCodec = avcodec_alloc_context3(codec);
    pVideoCodec->pix_fmt = PIX_FMT_YUV420P;
    pVideoCodec->width = VIDEO_WIDTH;
    pVideoCodec->height = VIDEO_HEIGHT;
    pVideoCodec->bit_rate = 400000;
    pVideoCodec->time_base.num = 1;
    pVideoCodec->time_base.den = 30;
    pVideoCodec->ticks_per_frame = 2;
    pVideoCodec->gop_size = 15;
    pVideoCodec->colorspace = AVCOL_SPC_SMPTE170M;
    pVideoCodec->thread_count = 2;

    if (avcodec_open2(pVideoCodec, codec, NULL) < 0)
    {
        fprintf(stderr,"could not open codec\n");
        exit(1);
        return;
    }

    struct stat buf;
    if (stat(FILENAME, &buf) != -1)
    {
        fprintf(stderr, "deleting old file...\n");
        remove(FILENAME);
    }

    file = fopen(FILENAME, "wb");
    fd = fileno(file);

    if (fd < 0)
    {
        fprintf(stderr, "could not open %s: %d: %s\n", FILENAME, errno,strerror(errno));
        exit(1);
        return;
    }

    pthread_t pthread;
    pthread_create(&pthread, 0, &start_encoding_thread, this);

    write_frames = 0;
    record = true;

    mStartStopButton->setText("Stop Recording");
    mStopButton->setEnabled(false);
    mStatusLabel->setText(basename(FILENAME));
    mStatusLabel->setVisible(true);
}

void vf_callback(camera_handle_t handle, camera_buffer_t* buf, void* arg)
{
    if (buf->frametype != CAMERA_FRAMETYPE_NV12) return;

    FFVideoCameraApp* app = (FFVideoCameraApp*) arg;
    app->video_encode_example(buf);
}

ssize_t write_all(int fd, uint8_t *buf, int size)
{
    ssize_t i = 0;
    do
    {
        ssize_t j = write(fd, buf + i, size - i);
        if (j < 0) return j;
        i += j;
    }
    while (i < size);
    return i;
}

void* start_encoding_thread(void* arg)
{
    FFVideoCameraApp* app = (FFVideoCameraApp*) arg;
    app->encoding_thread();
}

void FFVideoCameraApp::encoding_thread()
{
    qDebug() << "starting encoding thread";

    int nSizeVideoEncodeBuffer = 10000000;
    uint8_t *pVideoEncodeBuffer = (uint8_t *) av_malloc(nSizeVideoEncodeBuffer);

    AVFrame *frame = avcodec_alloc_frame();

    AVPacket pkt;
    int got_packet;
    int success;

    while (record || !frames.empty())
    {
        if (frames.empty())
        {
            pthread_mutex_lock(&reading_mutex);
            pthread_cond_wait(&read_cond, &reading_mutex);
            pthread_mutex_unlock(&reading_mutex);
            continue;
        }

        camera_buffer_t *buf = frames.front();
        frames.pop_front();

        int frame_position = write_frames++;

        int64_t uv_offset = buf->framedesc.nv12.uv_offset;
        uint32_t height = buf->framedesc.nv12.height;
        uint32_t width = buf->framedesc.nv12.width;

        frame->pts = frame_position;

        frame->linesize[0] = width;
        frame->linesize[1] = width / 2;
        frame->linesize[2] = width / 2;

        frame->data[0] = buf->framebuf;
        frame->data[1] = &buf->framebuf[uv_offset];
        frame->data[2] = &buf->framebuf[uv_offset + ((width * height) / 4)];

        // reset the AVPacket
        av_init_packet(&pkt);
        pkt.data = pVideoEncodeBuffer;
        pkt.size = nSizeVideoEncodeBuffer;

        got_packet = 0;
        success = avcodec_encode_video2(pVideoCodec, &pkt, frame, &got_packet);

        if (success == 0 && got_packet > 0)
        {
            int size = write_all(fd, pkt.data, pkt.size);
            if (size == pkt.size) printf("write frame %d (size=%5d)\n", frame_position, pkt.size);
            else printf("FAILED write frame %d (size=%5d)\n", frame_position, pkt.size);
        }
        else printf("skipped write frame: %d\n", frame_position);
        fflush(stdout);

        free(buf->framebuf);
        free(buf);
        buf = NULL;
    }

    av_free(frame);
    frame = NULL;

    qDebug() << "stopped recording";

    do
    {
        int frame_position = write_frames++;

        // reset the AVPacket
        av_init_packet(&pkt);
        pkt.data = pVideoEncodeBuffer;
        pkt.size = nSizeVideoEncodeBuffer;

        got_packet = 0;
        success = avcodec_encode_video2(pVideoCodec, &pkt, NULL, &got_packet);

        if (success == 0 && got_packet > 0)
        {
            int size = write_all(fd, pkt.data, pkt.size);
            if (size == pkt.size) printf("write (delayed) frame %d (size=%5d)\n", frame_position, pkt.size);
            else printf("FAILED write (delayed) frame %d (size=%5d)\n", frame_position, pkt.size);
            fflush(stdout);
        }
    }
    while (got_packet > 0);

    fclose(file);
    file = NULL;

    fd = 0;

    avcodec_close(pVideoCodec);
    av_free(pVideoCodec);
    pVideoCodec = NULL;

    av_free(pVideoEncodeBuffer);
    pVideoEncodeBuffer = NULL;
    nSizeVideoEncodeBuffer = 0;

    qDebug() << "closing encoding thread";
}

void FFVideoCameraApp::update_fps(camera_buffer_t* buf)
{
    fps.push_back(buf->frametimestamp);
    while (fps.back() - fps.front() > SECOND)
    {
        fps.pop_front();
    }
}

void FFVideoCameraApp::print_fps(camera_buffer_t* buf)
{
    static int64_t frametimestamp = 0;
    if (!frametimestamp || buf->frametimestamp - frametimestamp >= SECOND)
    {
        frametimestamp = buf->frametimestamp;
        qDebug() << "fps[" << fps.size() << "]";
    }
}

void FFVideoCameraApp::video_encode_example(camera_buffer_t* buf)
{
    update_fps(buf);
    print_fps(buf);

    if (!record) return;

    int64_t uv_offset = buf->framedesc.nv12.uv_offset;
    uint32_t height = buf->framedesc.nv12.height;
    uint32_t width = buf->framedesc.nv12.width;
    uint32_t stride = buf->framedesc.nv12.stride;

    uint32_t _stride = width; // remove stride
    int64_t _uv_offset = _stride * height; // recompute and pack planes

    camera_buffer_t* _buf = (camera_buffer_t*) malloc(buf->framesize);
    memcpy(_buf, buf, buf->framesize);
    _buf->framedesc.nv12.stride = _stride;
    _buf->framedesc.nv12.uv_offset = _uv_offset;
    _buf->framebuf = (uint8_t*) malloc(width * height * 3 / 2);

    for (uint32_t i = 0; i < height; i++)
    {
        int64_t doff = i * _stride;
        int64_t soff = i * stride;
        memcpy(&_buf->framebuf[doff], &buf->framebuf[soff], width);
    }

    uint8_t *srcuv = &buf->framebuf[uv_offset];
    uint8_t *destu = &_buf->framebuf[_uv_offset];
    uint8_t *destv = &_buf->framebuf[_uv_offset + ((width * height) / 4)];

    for (uint32_t i = 0; i < height / 2; i++)
    {
        uint8_t* curuv = srcuv;
        for (uint32_t j = 0; j < width / 2; j++)
        {
            *destu++ = *curuv++;
            *destv++ = *curuv++;
        }
        srcuv += stride;
    }

    frames.push_back(_buf);

    pthread_cond_signal(&read_cond);
}
