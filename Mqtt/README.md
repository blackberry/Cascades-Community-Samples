# Mqtt - A Proof of Concept MQTT Shared Library

This project implements a Proof of Concept shared library (**libMqtt.so**) for BlackBerry 10 that provides an API to some of the main aspects of the [MQTT](https://www.oasis-open.org/committees/tc_home.php?wg_abbrev=mqtt) (**Message Queuing Telemetry Transport**) protocol stack as defined by [OASIS](http://www.oasis-open.org).

MQTT provides a lightweight publish/subscribe reliable messaging transport protocol suitable for communication in **M2M/IoT** (**Machine to Machine/Internet of Things**) contexts where a small code footprint is required and/or network bandwidth is at a premium.

The sample code for this library is Open Source under the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

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

**How to Build Mqtt**

Simply import the project into a workspace in your NDK. Since this is a shared library building it will create a file called libMqtt.so in either the "Device-Debug" or "Device-Release" sub-folders in the project.

**But I don't want to build it myself!**

If you don't want to build this library application yourself we've included a pre-built version. You can find it in the folder "**prebuilt-files**" as follows:

* **libMqtt.so** -- Shared library for Version 1.0 (V1.0) of the library (requires BlackBerry 10 Software 10.2 or later).

**How to Use Mqtt**

To use the MQTT shared library APIs in your own project you need to add two things to your project:

* The header files in the **"/public"** folder of this project contain the public declarations of the API and need to be accessible in the include search path of your project.
* The shared library, **libMqtt.so**, needs to be accessible to your project at build time to resolve external references, and also at run time on the device you're installing to.

Take a look at the [**TestMqtt**](https://github.com/blackberry/Cascades-Community-Samples/tree/master/TestMqtt) and [**AutomotiveMqttDemo**](https://github.com/blackberry/Cascades-Community-Samples/tree/master/AutomotiveMqttConcept) applications in GitHub to see how we've done this in our samples that use this library.

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
