GoodCitizenCascades is a Cascades version of the NDK sample app, Good Citizen.  It features best practices for how to properly structure the OpenGL code and link it in with Cascades, how to leverage Actions and the use of NavigatorPanes for multiple panels, as well as how to process touch events and use them to control a 3D scene rendered in OpenGL, using Qt objects and JavaScript.  

Built initially for the Cascades + OpenGL session presented at Blackberry 10 Jam in Orlando, Florida, if you update or modify this example, please update the running version info as well.

New Features in this version of the sample app:

UI Changes:

- while the higher level interface has been streamlined to make it easier to create views with little code all panels have been modified to push them down to the bottom of the screen for easier access

Framework changes:

- OpenGLThread is now a singleton and the developer should not access it directly any more.  All methods for the thread that may need to be externally accessed are masked by an equivalent method in OpenGLView
- the framework now supports adding a view that is rendered in the HDMI display.  Another sample showcasing this feature is coming soon
- the API inAterface has been updated to split out certain setters for more streamlined definition of view settings
- OpenGLThread and OpenGLView no longer allow the developer to trap Navigator requests in the OpenGL layer.  API changes are in progress to add signal handlers to handle the respective signals from the Cascades side.
- the new version of the framework is locked down better to be thread-safe


For more information about our other Open Source projects, visit:

BlackBerry Open Source microsite (http://blackberry.github.com)

To check the Samples Catalog, visit:
Samples Catalog (http://blackberry.github.com/samples)

For more information about Cascades development, visit:
The Cascades microsite (http://developers.blackberry.com/cascades)

