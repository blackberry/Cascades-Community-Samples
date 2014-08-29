# BtChickenGuardian

The purpose of this application is to demonstrate how to develop an application for BlackBerry 10 which works with the Bluetooth Smart (Low Energy) Proximity Profile.

The sample code for this application is Open Source under 
the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://bdsc.webapps.blackberry.com/cascades/)

**Author(s)** 

* [Martin Woolley](https://github.com/mdwoolley)
* [John Murray](https://github.com/jcmurray)

**Release History**

* **V1.0.0** - Initial release
* **V1.1.0** - Added control over RSSI fluctuation sensitivity
* **V1.1.1** - Minor bug fix release (misnamed image file)

**Known Issues**

* Application is not using the TX Power Level characteristic in proximity calculations per the specification. This is because the Philips AEA1000 device which it was developed against does not allow this characteristic to be read for some reason.
* On reconnecting after link loss, the proximity sensor will alarm. Right now to re-establish a link, you should restart the application.
* Currently only works with Bluetooth devices that support all three services defined for the Proximity Profile, namely TX Power, Link Loss and Immediate Alert. A device that supports only a subset of these services (which is permitted per the profile definition) will not be listed in the Scan page.

**Dependencies**

1. BlackBerry Dev Alpha Beta 3 Device Software **10.2.0**

**How to Build BtChickenGuardian**

Simply Import the project into a workspace in your NDK. 

**But I don't want to build it myself!**

If you don't want to build this sample application yourself we've included a pre-built and signed BAR file. You can find it in the folder "**installable-bar-files**".
 
**To contribute code to this repository you must be [signed up as an 
official contributor](http://blackberry.github.com/howToContribute.html).**

## Contributing Changes

Please see the [README](https://github.com/blackberry/Cascades-Community-Samples/blob/master/README.md) of the Cascades-Community-Samples repository for instructions on how to add new Samples or 
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