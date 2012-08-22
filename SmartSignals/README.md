SmartSignals - Handle C-Level BPS events in the Cascades framework

========================================================================
Sample Description.

This sample shows how the AbstractBpsEventHandler interface can be extended to
receive BPS events without needing to create a separate thread. These events
are then made accessible to the QML layer by creating and registering a C++
class, BPSMonitor, so it is known to QML. After registering this type the
developer can then register to receive various events right from QML.

This sample can be tested either on a real device or using a simulator in pair
with controller.exe which can be used to perform sensor manipulations.

 
========================================================================
Requirements:

BlackBerry 10 Native SDK Beta

========================================================================
Running the example:

1. Download the source code
2. Launch BlackBerry 10 Native SDK Beta, and from the File menu, select Import.
3. Expand General, and select Existing Projects into Workspace. Click Next.
4. Browse to the location of the source code project, and then click OK.
5. The sample project should display in the the Projects section. 
   Click Finish to import the project into your workspace.
6. In the Project Explorer pane, Right-click the the project and select Build Project.
7. In the Project Explorer pane, Right-click the the project and select 
   Run As > BlackBerry C/C++ Application.
8. The application will now install and launch on your device if not you might
   have to set up your environment: 
   http://developer.blackberry.com/cascades/documentation/getting_started/setting_up.html