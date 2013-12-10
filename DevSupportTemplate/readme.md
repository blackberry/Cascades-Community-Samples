# DevelopmentSupport Template project

You can quickly test changes to your UI by using the DevelopmentSupport class. This class sends QML updates from the Momentics IDE for BlackBerry to your BlackBerry 10 device without redeploying your app.

The QML updates that you make in the IDE are sent to your running app where you can instantly view the results. This streamlined update feature makes developing a QML-based UI much faster. If you have code that can't be rendered easily in QML Preview, such as animations or custom controls, you can test changes to these controls with the DevelopmentSupport class. This feature is also useful for testing an app that uses data models that QML Preview can't render.

In this project it is shown how a helper object can enable DevelopmentSupport in a project.

For more details on this feature see [Updating QML live on a target](https://developer.blackberry.com/native/documentation/cascades/getting_started/tools/using_developmentsupport.html)


========================================================================
Requirements:

BlackBerry 10.2 Native SDK

========================================================================
Running the example:

1. From the Sample apps page, download and extract one of the sample applications.
2. Launch BlackBerry 10 Native SDK, and from the File menu, select Import.
3. Expand General, and select Existing Projects into Workspace. Click Next.
4. Browse to the location where you extracted the sample file, and then click OK.
5. The sample project should display in the Projects section. 
   Click Finish to import the project into your workspace.
6. In the Project Explorer pane, Right-click the project (for example hellocascades) 
   and select Build Project.
7. In the Project Explorer pane, Right-click the project (for example hellocascades) 
   and select Run As > BlackBerry C/C++ Application.
8. The application will now install and launch on your device. If it doesent you might
   have to set up your environment: 
   http://developer.blackberry.com/cascades/documentation/getting_started/setting_up.html
