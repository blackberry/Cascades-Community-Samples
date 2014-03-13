# HeartMonitor -- Bluetooth Low Energy Application

The purpose of this application is to demonstrate how to use selected 
BlackBerry 10 Bluetooth Low Energy APIs in the context of a practical real-world use case. The Release History section gives details of those APIs which are covered and any changes as the sample evolves over time.

The use case examines how to interface to a Bluetooth Low Energy Heart Rate Monitor device in order to acquire and display data from the device. The application uses the standard Heart Rate Service ([overview available here](http://developer.bluetooth.org/TechnologyOverview/Pages/HRS.aspx)) as specificied by the Bluetooth SIG enabling it to interwork with any Bluetooth Smart device that implements this profile.

The sample code for this application is Open Source under 
the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://bdsc.webapps.blackberry.com/cascades/)
* [BlackBerry Native SDK](http://developer.blackberry.com/native/)

**Author(s)** 

* [John Murray](https://github.com/jcmurray)
* [Martin Woolley](https://github.com/mdwoolley)


**Release History**

* **V1.0.0** - Initial release
* **V1.0.1** - OS Version support for Device Software **10.1.***
* **V1.0.3** - Workaround for problem caused by NULL event_data in early 10.2 builds. Ignore false GATT connect errno 16 errors.
* **V1.0.4** - Updated FLOT to version 0.8.2. 
               Removed scan timer (redundant) 
               Trigger device scanning on start now performed in C++ instead of QML as this was blocking completion of QML loading/rendering at 10.2.1.
               Added ActivityIndicator to indicate scanning activity

**Dependencies**

1. Minimal requrement BlackBerry 10 Device Software **10.0.9**
1. Minimal requrement BlackBerry 10 Native SDK **10.0.9**

**How to Build HeartMonitor**

Simply Import the project into a workspace in your NDK. Of course you should review the project after it's been imported paying particular attention to the settings in the **bar-descriptor.xml** file. It should be changed to reflect values in the **&lt;author&gt;** and **&lt;authorId&gt;** elements that are appropriate for you.

**What else will I need?**

You will need a Bluetooth Smart ( Low Energy) heart rate monitor that supports the standard [Heart Rate Service](http://developer.bluetooth.org/TechnologyOverview/Pages/HRS.aspx) and pair it to the BlackBerry 10 device before using the application.

**But I don't want to build it myself!**

If you don't want to build this sample application yourself we've included a 
pre-build and signed BAR file. You can find it in the 
folder "installable-bar-files" as follows:

* **HeartMonitor-1\_0\_1\_203.bar** -- Signed BAR file for Version 1.0.1 (V1.0.1) of the application ( minimal requirement BlackBerry 10 Software 10.0.9 )
* **HeartMonitor-1\_0\_0\_202.bar** -- Signed BAR file for Version 1.0 (V1.0) of the application ( minimal requirement BlackBerry 10 Software 10.0.9 )

**To contribute code to this repository you must be [signed up as an 
official contributor](http://blackberry.github.com/howToContribute.html).**

**Devices we've tested with**

* Wahoo Blue HR
* Polar H7
* Zephyr HxM Smart

**Known Issues**

* You should ensure that the heart rate monitor that you use has been paired with the BlackBerry 10 device before using the application. Dynamic pairing will be added as a later release of this application.

## Contributing Changes

Please see the [README](https://github.com/blackberry/Cascades-Community-Samples/blob/master/README.md) 
of the Cascades-Community-Samples repository for instructions on how to add new Samples or 
make modifications to existing Samples.


## Bug Reporting and Feature Requests

If you find a bug in a Sample, or have an enhancement request, simply file 
an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues) for 
the Sample and send a message (via github messages) to the Sample Author(s) to let 
them know that you have filed an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues).


## Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR 
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
OTHER DEALINGS IN THE SOFTWARE.