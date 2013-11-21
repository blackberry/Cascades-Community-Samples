ShakeIt

========================================================================
Sample Description:

 ShakeIt is an example of a headless application.  It contains a UI
 application (in ./ShakeIt) and a headless application (in ./ShakeItService).
 The UI portion is written using Cascades.  The headless portion is written
 using BPS.  Once installed, shaking the device will cause the LED to blink in
 a pattern.  This is the headless portion that is monitoring the accelerometer
 and sending LED commands.  The UI is used to give a new LED blink pattern to
 the headless portion.  The key here is that even if the UI is closed, the
 headless portion remains running and monitoring the accelerometer and blinking
 the LED.  Communication between the UI and the headless executables is done
 through the use of the Invoke Framework.

 Since this application has to executables the bar-descriptor.xml is quite a
 bit different than the standard application.  It has two entry points.  One
 is for the UI the other is for the headless portion.

 The build structure is also quite different from a typical application.  From
 the top-level of the directory, import both the ShakeIt and the ShakeItService
 projects.  Both need to be built.

 Feature summary
 - Demonstrating headless/UI communication using Invoke Framework
 - Demonstrating headless/UI definition in the bar-desciptor.xml
 - Demonstrating headless/UI builds in a single project

========================================================================
Requirements:

 - BlackBerry® 10.2 Native SDK
 - One of the following:
   - BlackBerry® 10.2 device

========================================================================
Running the example:

1. Clone the Sample repository.
2. Launch BlackBerry Native SDK, and from the File menu, select Import.
3. Expand General, and select Existing Projects into Workspace. Click Next.
4. Browse to the location of your sample directory, and then click OK.
5. The sample project should display in the Projects section. 
   Click Finish to import the project into your workspace.
6. In the Project Explorer pane, Right-click the project (for example hellocascades) 
   and select Build Project.
7. In the Project Explorer pane, Right-click the project (for example hellocascades) 
   and select Run As > BlackBerry C/C++ Application.
8. The application will now install and launch on your device if not you might
   have to set up your environment: 
   http://developer.blackberry.com/cascades/documentation/getting_started/setting_up.html
