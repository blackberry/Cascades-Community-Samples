# VideoPlayerSample

This is a simple video player application. It uses the new MediaPlayer API and showcases all the fundamentals to make other full fledged video player applications.


**Feature Summary**

- Use of MediaPlayer API
- Play, Pause, Seek functionalities (with a progress slider)
- Use of ForeignWindow for video playback
- Zooming and panning of the ForeignWindow
- Gesture handling - Pinch
- Orientation handling
- Making use of lots of containers and layouts
- Implicit animation

This application is built for BlackBerry 10 Beta2 SDK (June 2012)


**Author (s)**

[Shadab Rashid](https://github.com/surashid)
[@surashid](http://twitter.com/surashid)
(based on an application by Brian Vanpee & Sulman Choudhry)


**Current issue(s)**

Auto orientation is not fully functional in the BlackBerry 10 Beta2 SDK. However, this application still implements auto orientation and shows how to use it. As a result, the following is an unavoidable issue with the auto orientation of this application at the moment:

- If the video is not playing, on every other orientation change, the screen’s pixels become illegible. This issue is not present if the video is actually playing through the ForeignWindow when the orientation change occurred.
