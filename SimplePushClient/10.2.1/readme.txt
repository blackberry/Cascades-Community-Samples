SimplePushClient

========================================================================
Sample Description.

This sample allows for receiving push messages at various priority levels;
the level will determine how intrusive the notification to the end user will
be. The app also integrates with the SceneCover to provide details on app
activity to the end user even when the application is thumbnailed.

The 10.2 version handles all push messages and channel using the new headless
triggers, so the app does not need to be launched by the user for it to begin 
receiving push messages.

Note: This sample was designed to work in tandem with the SimplePushServer app 
      though it could easily be modified to push to any client.
 
========================================================================
Requirements:

BlackBerry 10.2.1 Native SDK 

========================================================================
Running the example:

1. Download the source code
2. Launch BlackBerry 10 Native SDK, and from the File menu, select Import.
3. Expand General, and select Existing Projects into Workspace. Click Next.
4. Browse to the location of the source code project, and then click OK.
5. The sample project should display in the the Projects section. 
   Click Finish to import the project into your workspace.
6. Open the bar-descriptor.xml file and modify the invoke-target element's id 
   value to be something specific to your app:
   ex com.yourcompany.myfirstpushapp
7. Open SimplePushService/src/service.cpp and modify the 3 QString variables 
   at the top by replacing with your Push Service account values and the 
   invoke-target id specified in step 6.
8. In the Project Explorer pane, Right-click the the project and select Build 
   Project.
9. In the Project Explorer pane, Right-click the the project and select 
   Run As > BlackBerry C/C++ Application.
10. The application will now install and launch on your device if not you might
   have to set up your environment: 
   http://developer.blackberry.com/cascades/documentation/getting_started/setting_up.html

