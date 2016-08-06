odatabb10
=========

Open Data Protocol (OData) library for BlackBerry 10 (BB10) Cascades apps.  
This library makes it easy to build native BB10 apps that consume OData services.  
For information about OData, refer to http://www.odata.org/

Building this Sample
--------------------

Import both OData and TestHarness projects into your NDK workspace. The OData project is the library itself and TestHarness is a simple application that shows how you might use the OData library.

Requirements:

+ BASH Shell (On OSX you already have this as your standard shell. On Windows if you installed Git or Github for Windows you also have this)
+ BlackBerry Cascades Native SDK + QDE
+ A BlackBerry 10 Device or Simulator

**Build the Library**

1. Find the moc.sh (or moc_mac.sh if you are on OSX) file and edit the file to point to your bbndk's directory as marked in the comments in that file.

2. From a BASH shell execute the moc*.sh script.

3. Move the moc_ODataListModel.cpp, moc_ODataNetworkManager.cpp, moc_ODataObjectModel.cpp and moc_ODataService.cpp files output to the OData/public/ directory.

4. From the QDE build the .a library archive for the target you need (Device-Debug, Device-Release, Simulator-Debug).

**Build the Application**

1. In order to utilize the writeback functionality of OData you need a writeable source. Visit this link ( http://services.odata.org/(S(readwrite))/OData/OData.svc/ ) to generate a private sandbox to edit. Copy the generated url in the address bar and then place it in the marked location in TestHarness/src/applicationui.cpp. It should look like this http://services.odata.org/(S(xxxxxxxxxxxxxxxxxxxxxxxx))/OData/OData.svc/ where the x's are a set of random letters and numbers.

2. From the QDE build the application for the target you need.

3. Deploy to the target.

How to use For Your Own Application
-----------------------------------

To integrate into your app:

+ Add this to your .pro file

```
LIBS += -lbbdata
```

+ Option 1. Configure the .pro file to import the correct library artifacts from your copy of the library. If you are not editing or debugging the library yourself you may not need to supply a different Debug and Release copy of the library and can simply use a Release build for both.

```
device {
    CONFIG(debug, debug|release) {
        # Device-Debug custom configuration
		INCLUDEPATH += ../../OData/Device-Debug/public
		LIBS += -Bstatic -L../../OData/Device-Debug/ -lOData -Bdynamic
    }

    CONFIG(release, debug|release) {
        # Device-Release custom configuration
		INCLUDEPATH += ../../OData/Device-Release/public
		LIBS += -Bstatic -L../../OData/Device-Release/ -lOData -Bdynamic
    }
}

simulator {
    CONFIG(debug, debug|release) {
        # Simulator-Debug custom configuration
		INCLUDEPATH += ../../OData/Simulator-Debug/public
		LIBS += -Bstatic -L../../OData/Simulator-Debug/ -lOData -Bdynamic
    }
}
```

+ Option 2. You can simply copy the src/ directory from the OData project into a folder in your project and directly build this library into your project. You may need to add the QtNetwork library to your includes/LIBs if you are not using it already.
