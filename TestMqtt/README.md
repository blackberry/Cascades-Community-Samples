# TestMqtt -- An application to test MQTT

The purpose of this application is to help explore the use of [MQTT](https://www.oasis-open.org/committees/tc_home.php?wg_abbrev=mqtt) (**Message Queuing Telemetry Transport**) as defined by [OASIS](http://www.oasis-open.org). It makes use of a [shared library](https://github.com/blackberry/Cascades-Community-Samples/tree/master/Mqtt) (**libMqtt.so**) we've written, as a Proof of Concept, that implements some of the main aspects of the API needed by an MQTT client.

MQTT provides a lightweight publish/subscribe reliable messaging transport protocol suitable for communication in **M2M/IoT** (**Machine to Machine/Internet of Things**) contexts where a small code footprint is required and/or network bandwidth is at a premium.

The sample code for this application is Open Source under the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://bdsc.webapps.blackberry.com/cascades/)
* [BlackBerry Native SDK](http://developer.blackberry.com/native/)

**Author** 

* [John Murray](https://github.com/jcmurray)
* [Martin Woolley](https://github.com/mdwoolley)

**Release History**

* **V1.0.0** - Initial release

**Dependencies**

1. Minimal requirement for BlackBerry 10 Device Software is **10.2.0**
1. Minimal requirement for BlackBerry 10 Native SDK is **10.2.0**
1. [Mqtt library](https://github.com/blackberry/Cascades-Community-Samples/tree/master/Mqtt) 1.0.0 (also available from this GitHub repo)


**How to Build TestMqtt**

Simply import the project into a workspace in your NDK. Of course you should review the project after it's been imported; for example, paying attention to the settings in the **bar-descriptor.xml** file.

This project makes use of the [Mqtt library](https://github.com/blackberry/Cascades-Community-Samples/tree/master/Mqtt) *libMqtt.so*. If you've installed the Mqtt project in the same workspace as this project then simply build the Mqtt project first, then perform a "Clean" and "Build" on this project. There is some logic built into the *TestMqtt.pro* file to locate and use the public header files for Mqtt and *libMqtt.so* itself.

**But I don't want to build it myself!**

If you don't want to build this sample application yourself we've included a pre-built and signed BAR file. You can find it in the folder "**installable-bar-files**" as follows:

* **TestMqtt-1\_0\_0\_254.bar** -- Signed BAR file for Version 1.0 (V1.0) of the application (requires BlackBerry 10 Software 10.2.0 or later).

**To contribute code to this repository you must be [signed up as an official contributor](http://blackberry.github.com/howToContribute.html).**

**Known Issues**

* None

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