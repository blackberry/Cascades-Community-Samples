This is a fork of HelloVideoCamera from:  
https://github.com/blackberry/Cascades-Community-Samples/tree/master/HelloVideoCamera

Support forum post:  
http://supportforums.blackberry.com/t5/Native-Development/Camera-API-NV12-frame-to-AVFrame-FFmpeg/td-p/1842089

Building FFmpeg:

	$ ./configure --enable-cross-compile --cross-prefix=arm-unknown-nto-qnx8.0.0eabi- --arch=armv7 --disable-debug --enable-optimizations --enable-asm --disable-static --enable-shared --target-os=qnx --disable-ffplay --disable-ffserver --disable-ffprobe --prefix=`pwd`/target  
	$ make install  

-------

HelloVideoCamera is a minimalist Cascades-based video camera app.  It illustrated how to set up a Cascades foreign window and connect the Camera viewfinder to it.  It has functionality for starting and sotpping the camera, changing between front and back cameras, and recording video files which are saved to the camera roll.

Built for the initial Cascades public beta may 2012, if you update or modify this example, please update the running version info as well.
