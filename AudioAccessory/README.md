# AudioAccessory -- Using the Audio Jack to Transfer Data

The purpose of this application is to demonstrate how to use selected 
BlackBerry 10 Audio APIs in the context of a practical real-world use case. The Release History section gives details of those APIs which are covered and any changes as the sample evolves over time.

The use case examines how to interface to an external device that is connected via the BlackBerry 10 Audio Jack. This mechanism for connecting accessories is used by a number of vendors in the marketplace as an alternative to Bluetooth Low Energy, WiFi or NFC and this example examines the points you need to consider when taking this approach.

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

**Dependencies**

1. Minimal requrement BlackBerry 10 Device Software **10.1**
1. Minimal requrement BlackBerry 10 Native SDK **10.1**

**How to Build AudioAccessory**

Simply Import the project into a workspace in your NDK. Of course you should review the project after it's been imported paying particular attention to the settings in the **bar-descriptor.xml** file.

**What else will I need?**

You will need two BlackBerry 10 devices and suitable audio cables to run the actual demonstration. For this configuration a BlackBerry 10 device needs to be able to believe it has an external microphone plugged in via the audio jack just like an external headset. The audio jack is a 3.5mm 4-pin (TRRS) variant and in order for a BlackBerry 10 device to interpret a device inserted into the socket as a "headset" there should be a specific set of impedances across the various pins.

Using the standard TRRS notation with RING2 as GROUND these should be of the order:

* GROUND/RING2 to SLEEVE = 32  ohm
* GROUND/RING2 to RING1  = 32  ohm
* GROUND/RING2 to TIP    = 2  kohm

We found that you could "cheat" a bit and use a TRSS Y-splitter cable, the sort you can use to share music from a single player to two headsets. We plugged it into the BlackBerry 10 device audio socket with one leg going to the other BlackBerry 10 device and the other leg connected to a standard BlackBerry 10 headset. This achieved values similar enough to the above values to have the external microphone recognised and allow both devices to communicate :-).

**But I don't want to build it myself!**

If you don't want to build this sample application yourself we've included a pre-built and signed BAR file. You can find it in the folder "installable-bar-files" as follows:

* **AudioAccessory-1\_0\_0\_36.bar** -- Signed BAR file for Version 1.0 (V1.0) of the application ( minimal requirement BlackBerry 10 Software 10.1 )

**To contribute code to this repository you must be [signed up as an 
official contributor](http://blackberry.github.com/howToContribute.html).**

**Known Issues**

* None

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