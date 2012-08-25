Multithreading 101 - A simple example showing 4 threads all counting and updating the UI independently

========================================================================
Sample Description:

 The Multithreading 101 sample demonstrates how four different threads can process
 data in a thread and update the UI independently.

 We create a Counter class which will be run a counter in its own thread.  
 In the run loop it increments a counter and emits a signal that is 
 handled by the UI and used to display the current value of the counter. 

 The Counter class exposes these properties which are used to change the frequency of
 the signalling, and the rate at which the counter is incremented. It exposes properties
 that are used in the QML to customize each timer before it is started.

The UI has 4 buttons and 4 counters showing the value of the counters in the threads. 
Threads can be independently paused, or simultaneously paused (for example using two fingers to
touch two "pause" buttons will pause those threads).

 Feature summary
 - Runs multiple threads each updating the UI independently
 - C++ objects instantiated and controlled directly through the QML
 - Shows how to use QMutex and QWaitCondition to pause/resume the threads

========================================================================
Requirements:

 - BlackBerry Cascades SDK for BlackBerry 10 or later
 - One of the following:
   - BlackBerry 10 alpha device/simulator or later

========================================================================
Importing a project into the Native SDK:

 1. From the the Sample apps page, download and extract the sample application.
 2. Launch the Native SDK.
 3. On the File menu, click Import.
 4. Expand General, and select Existing Projects into Workspace. Click Next.
 5. Browse to the location where you extracted the sample app, and click OK.
    The sample project should display in the the Projects section.
 6. Click Finish to import the project into your workspace.

