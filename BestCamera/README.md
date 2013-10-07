BestCamera
==========
This is a best-practices sample camera app which is written primarily in C++, but with a UI done in QML.  This sample is meant to be usable as a starting point for anyone to write a camera app which uses the advanced features of the native C camera API.

Things this code does:

* provides front/rear camera discovery
* provides photo/video mode discovery
* provides resolution & rotation discovery
* handles orientation changes so viewfinder, ui, and captured photos & videos playback upright
* handles powerdown and resume
* handles resource arbitration with other multimedia components
* keeps screen awake in video mode
* plays shutter sounds
* correctly maintains aspect ratio of viewfinder in parent container (not stretched)
* illustrates aspect-ratio matching between viewfinder and capture settings
* shows how to use a status callback in event-mode
* provides a state machine for camera operating modes

Things this code may do later as time permits:

* aspect ratio and resolution selection
* focus indicators
* more discovery of available features
* probably be turned into a custom control which would make it easier to insert into existing apps
