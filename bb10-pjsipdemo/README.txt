Author: 	Robert Cripps - author of the PJSIP BB10 Audio Driver 
Date:		25/11/2012

Project Overview:
A very quickly put together demonstration project for integrating PJSIP into
a BB10 cascades application.
The project contains the main demo application which can be built on any
BB10 SDK platform, however, there is a Linux dependency (OSX should work too)
for building the PJSIP libraries. The instructions for this are in the
pjbuildscripts folder. The result of executing these scripts as described in
the pjbuildscripts/README.txt is an sdkfiles.zip file which can be unzipped
over an SDK on any platform, enabling developers to develop the client side
of a PJSIP application on Windows, MAC or Linux. The project was constructed
from an "empty cascades project"
My recommended way to do this would be to build the libraries into a shared
library wrapper to make the cascades application less cumbersome.
I will be adding functionality to this sample in due course but this is 
sufficient to get a developer up and running.

NB: Although SIP registration will work on the Simulator, making calls will
not as the BB10 PJMEDIA audio driver requires real hardware.
I've not had a chance to investigate but you should be able to build PJSIP
with null audio so you can at least test the signalling on the simulator.

1) Basics
1.1) Build PJSIP
As mentioned above, build the PJSIP static libraries and make sure all the
headers and libraries are overlayed on the SDK by unzipping sdkfiles.zip in
$QNX_TARGET

1.2) Import the project into the IDE
Import general project

1.3) Build
To make sure everything is correct below the bonnet, build for Simulator Debug
and Device Debug.
Providing all the files in the sdkfiles.zip are unzipped to $QNX_TARGET, it
should build for both.

2) Files
An overview of the important modified files
2.1) Project File
pjsipdemo.pro extensively modified to point to the PJSIP includes, an added
DEFINE PJ_AUTOCONF and the PJSIP libraries for three of the build
configurations.

2.2) BAR descriptor
bar-descriptor.xml
Extensively modified to include the PJSIP static libraries as assets for three
of the build configurations.

2.3) assets
The standard main.xml is just a Page with an as yet unused call button and
destination number text entry box

2.4) cpp and hpp
PjsipEngine.cpp
Adapted code from the PJSIP project sample. SIP registers on construction and
deregisters on destruction.

 