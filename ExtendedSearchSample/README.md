ExtendedSearchSample
====================

A short example showing off how to integrate your app with Extended Search.

Key files:

bar-descriptor.xml - Contains the invoke target you need to register for extended search.

applicationui.h/.cpp - The constructor connects to the InvokeManager's invoked() signal, and shows you how to handle a different startup sequence for your app if it's invoked.
  It also contains the onInvoked method, which sets a "searchString" property in main.qml

main.qml - Creates the searchString property.  You could fill in the onSearchStringChanged method to do what you need to start a search in your app.
  The rest of the file is a dummy UI that is using property binding to fake a search, so you can see what things look like if you test it yourself.
  The property binding approach could work in your app well, depending on your use case.
