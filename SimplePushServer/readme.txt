SimplePushServer

========================================================================
Sample Description.

This sample provides 2 basic functions of a BlackBerry Push Initiator. Namely it 
will support:
1) Send a message to one or more recipients
2) Request a list of currently active push subscribers

This sample makes use of pre-defined template files which follow the conventions 
expected by the push service. As a developer you should be able to add extra 
functionality by adding additional templates then following the same flow used by 
the app to swap out information to make the request relevant to the desired call.

Note: This sample was designed to work in tandem with the SimplePushClient app 
though it could easily be modified to push to any client.
 
========================================================================
Requirements:

BlackBerry 10 Native SDK Gold

========================================================================
Running the example:

1. Download the source code
2. Launch BlackBerry 10 Native SDK, and from the File menu, select Import.
3. Expand General, and select Existing Projects into Workspace. Click Next.
4. Browse to the location of the source code project, and then click OK.
5. The sample project should display in the the Projects section. 
   Click Finish to import the project into your workspace.
6. Open src/SimplePushServer.cpp and modify the 4 QString variables at the top 
   by replacing with your Push Service account values.
7. In the Project Explorer pane, Right-click the the project and select Build 
   Project.
8. In the Project Explorer pane, Right-click the the project and select 
   Run As > BlackBerry C/C++ Application.
9. The application will now install and launch on your device if not you might
   have to set up your environment: 
   http://developer.blackberry.com/cascades/documentation/getting_started/setting_up.html

