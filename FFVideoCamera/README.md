FFVideoCamera is a fork of the [HelloVideoCamera](https://github.com/blackberry/Cascades-Community-Samples/tree/master/HelloVideoCamera) app. Instead of using the native camera encoder to write to an MP4 file, this app will use FFmpeg to write out to an MPEG-2 file. This app currently only records video and not audio.

Related support forum post:  
http://supportforums.blackberry.com/t5/Native-Development/Camera-API-NV12-frame-to-AVFrame-FFmpeg/td-p/1842089

Building the FFmpeg libs for ARM:

	$ ./configure --enable-cross-compile --cross-prefix=arm-unknown-nto-qnx8.0.0eabi- --arch=armv7 --disable-debug --enable-optimizations --enable-asm --disable-static --enable-shared --target-os=qnx --disable-ffplay --disable-ffserver --disable-ffprobe --prefix=`pwd`/target  
	$ make install  

Built using the 10.0.6 BlackBerry 10 NDK, and tested on the BlackBerry Dev Alpha device.