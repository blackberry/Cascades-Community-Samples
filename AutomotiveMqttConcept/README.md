# AutomotiveMqttConcept -- A demonstration of a concept involving the enterprise, a fleet of vehicles and the IoT protocol MQTT

The purpose of this application is to demonstrate an idea about how data from drivers and vehicles could be collected from sensors via an in-vehicle BlackBerry 10 mobile computer, and sent to the enterprise systems *"in the cloud"* for analysis and exploitation. 

It's just an idea. Most information on the UI is hard coded. The exception is the driver heart rate which is collected, live, from a Bluetooth Smart heart rate monitor and sent via MQTT to an MQTT message broker. In testing we used the free of charge Mosquitto server.

Note that you can set the IP address and port of your MQTT server if you drag down from the top of the screen and go into Settings. 

Note also that you must pair/connect to your Bluetooth Smart heart monitor in Settings-Bluetooth before it will be available to this application.

See [http://youtu.be/60dWJIVJkzI](http://youtu.be/60dWJIVJkzI) which includes this application being demonstrated.


The sample code for this application is Open Source under the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://bdsc.webapps.blackberry.com/cascades/)
* [BlackBerry Native SDK](http://developer.blackberry.com/native/)

**Author** 

* [Martin Woolley](https://github.com/mdwoolley)
* [John Murray](https://github.com/jcmurray)


**Release History**

* **V1.0.0** - Initial release

**Dependencies**

1. Minimal requirement for BlackBerry 10 Device Software is **10.2.0**
1. Minimal requirement for BlackBerry 10 Native SDK is **10.2.0**
1. [Mqtt library](https://github.com/blackberry/Cascades-Community-Samples/tree/master/Mqtt) 1.0.0 (also available from this GitHub repo)

**How to Build AutomotiveMqttConcept**

Simply import the project into a workspace in your NDK. Of course you should review the project after it's been imported; for example, paying attention to the settings in the **bar-descriptor.xml** file.

This project makes use of the [Mqtt library](https://github.com/blackberry/Cascades-Community-Samples/tree/master/Mqtt) *libMqtt.so*. If you've installed the Mqtt project in the same workspace as this project then simply build the Mqtt project first, then perform a "Clean" and "Build" on this project. There is some logic built into the *AutomotiveMqttConcept.pro* file to locate and use the public header files for Mqtt and *libMqtt.so* itself.

**But I don't want to build it myself!**

If you don't want to build this sample application yourself we've included a pre-built and signed BAR file. You can find it in the folder "**installable-bar-files**".

* **AutomotiveMqttConcept-1\_0\_0\_1.bar** -- Signed BAR file for Version 1.0 (V1.0) of the application (requires BlackBerry 10 Software 10.2.0 or later).

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