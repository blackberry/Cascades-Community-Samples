# WakeMeByBeacon

The purpose of this application is to demonstrate how to develop an application for BlackBerry 10 which is capable of running as a Headless application, detecting [iBeacons](http://support.apple.com/kb/HT6048)(TM) in the background and notifying the application's user interface component of iBeacon events. If the user interface component is not running it will be launched via the Hub.

The sample code for this application is Open Source under 
the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://bdsc.webapps.blackberry.com/cascades/)

**Author(s)** 

* [John Murray](https://github.com/jcmurray)

**Release History**

* **V1.0.0** - Initial release
* **V1.1.0** - Support for AltBeacon format

**Known Issues**

* None

**Dependencies**

1. BlackBerry Device Software **10.3 GOLD** and greater

**How to Build WakeMeByBeacon**

Simply Import the project into a workspace in your NDK. 

The **WakeMeByBeacon** application consists of two interrelated projects:

WakeMeByBeacon -- the GUI component of the application.
WakeMeByBeaconService -- the Headless service component of the application.

These can be found under the WakeMeByBeacon top level folder. Copy this folder and all its contents to a location of your choosing then import these four projects into a workspace in your NDK.

When importing these projects do **NOT** copy them. You **MUST** ensure that the checkbox entitled **"Copy projects into workspace"** is **NOT** checked. There are interrelationships between the two constituent projects that will **NOT** be properly preserved if you copy them.

You should review the two projects after they have been imported paying particular attention to the settings in the **bar-descriptor.xml** file in the **WakeMeByBeacon** project. There is only one bar-descriptor.xml for this application and that is the one in the **WakeMeByBeacon** project. The other project has no **bar-descriptor.xml** file!

To build the application you should build the **WakeMeByBeacon** project. The other project will be built automatically as a dependent project.

You should ensure that both projects have the same settings in terms of **Build Configuration**; **"Device-Debug"**, **"Device-Release"**, etc.

**But I don't want to build it myself!**

If you don't want to build this sample application yourself we've included a pre-built and signed BAR file. You can find it in the folder "**installable-bar-files**".
 
**To contribute code to this repository you must be [signed up as an 
official contributor](http://blackberry.github.com/howToContribute.html).**

## Contributing Changes

Please see the [README](https://github.com/blackberry/Cascades-Community-Samples/blob/master/README.md) of the Cascades-Community-Samples repository for instructions on how to add new Samples or make modifications to existing Samples.

## Bug Reporting and Feature Requests

If you find a bug in a Sample, or have an enhancement request, simply file an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues) for the Sample and send a message (via github messages) to the Sample Author(s) to let them know that you have filed an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues).


## Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.