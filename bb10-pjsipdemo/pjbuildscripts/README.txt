File:		README.txt
Author:		Robert Cripps
Date:		11/10/2012
Contains:	Description of the bash scripts in this folder

"source" the bbdnk bbndk-env.sh

Run in this order:
	
	./getpjsource
	Sync the source from PJSIP and Opencore AMR (zip)
	All the source goes in ./pjsip-checkout folder which other scripts depend on. This folder could be made a parameter to the bash scripts
	
	./buildpjsip
	First builds opencore amr then pjsip libraries in the ./pjsip-build folder. Built for target (Debug) and Simulator (Debug)
	Copies headers and libraries to the SDK for building pjsip dependent applications on this machine 
	
	./createsdkoverlay
	Creates an SDK overlay zip file from the previously created zips to be used by developers of all platforms
	Just unzip this file in $QNX_TARGET. NB: These files are already copied on the PJSIP build machine by the buildpjip bash script


Generic configure script:

./generic-configure-bb10
Used by ./buildpjsip to build opencore AMR libraries
NB: Can be used for any gnu autotools compliant project

