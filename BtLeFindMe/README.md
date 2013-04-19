# BtLeFindMe -- Bluetooth Low Energy Application

The purpose of this application is to demonstrate how to use selected BlackBerry 10 Bluetooth Low Energy APIs in the context of a practical real-world use case. The Release History section gives details of those APIs which are covered and any changes as the sample evolves over time.

The use case examines how to interface to a Bluetooth Low Energy device that will, on demand, issue alerts in the form of audible beeps and flashing LEDs. Think of a key fob that that can be activated from your BlackBerry 10 device to allow it to be found again easily.  

The application uses the standard Find Me Profile ([overview available here](http://developer.bluetooth.org/TechnologyOverview/Pages/FMP.aspx)) as specified by the Bluetooth SIG enabling it to interwork with any Bluetooth Smart device that implements this profile.

The sample code for this application is Open Source under the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://bdsc.webapps.blackberry.com/cascades/)
* [BlackBerry Native SDK](http://developer.blackberry.com/native/)

**Author(s)** 

* [John Murray](https://github.com/jcmurray)
* [Martin Woolley](https://github.com/mdwoolley)


**Release History**

* **V1.0** - Initial release

**Dependencies**

1. Minimal requirement for BlackBerry 10 Device Software is **10.0.9**
1. Minimal requirement for BlackBerry 10 Native SDK is **10.0.9**

**How to Build BtLFindMe**

Simply import the project into a workspace in your NDK. Of course you should review the project after it's been imported paying particular attention to the settings in the **bar-descriptor.xml** file. It should be changed to reflect values in attributes, such as the **&lt;author&gt;** and **&lt;authorId&gt;** elements, that are appropriate for you.

**What else will I need?**

You will need a Bluetooth Smart (Low Energy) device that supports the standard [Find Me Profile](http://developer.bluetooth.org/TechnologyOverview/Pages/FMP.aspx) and pair it to the BlackBerry 10 device before using the application.

This application was tested successfully against the following devices:

* Texas Instruments [CC2540 Mini Development Kit](http://www.ti.com/tool/cc2540dk-mini) running the **CC2540MiniDkDemoSlave** firmware.
* BlueGiga [DKBLE112 Development Kit](http://www.bluegiga.com/evaluation_BLE112) running the **find_me** firmware example application.
* Vtag Bluetooth 4.0 Key Finder Tag Locator.

Our use of these these devices for testing shouldn't be regarded as an endorsement, explicit or implied, of them by BlackBerry.

**But I don't want to build it myself!**

If you don't want to build this sample application yourself we've included a pre-built and signed BAR file. You can find it in the folder "**installable-bar-files**" as follows:

* **BtLeFindMe-1\_0\_0\_442.bar** -- Signed BAR file for Version 1.0 (V1.0) of the application (requires BlackBerry 10 Software 10.0.9 or later).

**To contribute code to this repository you must be [signed up as an official contributor](http://blackberry.github.com/howToContribute.html).**

**Known Issues**

* You should ensure that the Bluetooth LE device that you use has been paired with the BlackBerry 10 device before using the application. Dynamic pairing will be added in a later release of this application.

## Contributing Changes

Please see the [README](https://github.com/blackberry/Cascades-Community-Samples/blob/master/README.md) of the Cascades-Community-Samples repository for instructions on how to add new Samples or make modifications to existing Samples.


## Bug Reporting and Feature Requests

If you find a bug in a Sample, or have an enhancement request, simply file an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues) for the Sample and send a message (via github messages) to the Sample Author(s) to let them know that you have filed an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues). If you have a solution to an issue you can contribute your fix and issue a Pull Request to have it reviewed and integrated into the main repo.


## Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR 
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
OTHER DEALINGS IN THE SOFTWARE.