UrbanAirshipClient - The UrbanAirship sample client application for BlackBerry 10. 
                     Written using the Cascades framework.

========================================================================
Sample Description.

This sample demonstrates how to register the BlackBerry 10 device with UrbanAirship 
and the BlackBerry Push Services so that pushes can be sent to the device via 
UrbanAirship. If you already have an UrbanAirship and BlackBerry Push Services 
account then you can use the same values for those to test push on BlackBerry 10. 
If you need to sign up for either service, please find links to do so below.

Sign up for BlackBerry Push Services:
https://developer.blackberry.com/develop/platform_services/push_overview.html

Sign up for UrbanAirship:
http://go.urbanairship.com 


========================================================================
Requirements:

BlackBerry 10 Native SDK Beta 3

========================================================================
Running the example:

1. Download the source code
2. Launch BlackBerry 10 Native SDK Beta, and from the File menu, select Import.
3. Expand General, and select Existing Projects into Workspace. Click Next.
4. Browse to the location of the source code project, and then click OK.
5. The sample project should display in the the Projects section. Click Finish to 
   import the project into your workspace.
6. Open the bar-descriptor.xml file and modify the invoke-target id value to be unique 
   to your application.
7. Open UrbanAirshipAPI.cpp file and enter the values from your UrbanAirship and 
   BlackBerry Push Services account as well as the new value defined for your 
   invoke-target id in the previous step.
8. In the Project Explorer pane, Right-click the the project and select Build Project.
9. In the Project Explorer pane, Right-click the the project and select Run As > 
   BlackBerry C/C++ Application.
10. The application will now install and launch on your device if not you might have to 
    set up your environment: 
	http://developer.blackberry.com/cascades/documentation/getting_started/setting_up.html
11. Once complete you can access your UrbanAirship account and begin sending push messages 
    to the device by specifying its PIN:
	http://go.urbanairship.com 