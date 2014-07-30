# GattServerSample

The purpose of this project is to demonstrate how to develop an application for BlackBerry 10 which is capable of hosting a Bluetooth Low Enery GATT Service.

The GATT service itself is a simple one that sends a stream of random numbers, one per second, to any client that subscribes to the service's Notifications.

There are two applications in this sample:

* An application that acts as a ***GATT Server***, and, for convenience;
* An application that acts as a ***GATT Client*** to the GATT server application.
 
If you'd rather use some other GATT client application to connect to the sample GATT server then feel free to do so. For example I've successfully tested this GATT Service using ***Texas Instrument's [BTool](http://www.ti.com/tool/cc2541dk-mini)*** application on a PC.

The sample code for this application is Open Source under 
the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://bdsc.webapps.blackberry.com/cascades/)

**Author(s)** 

* [John Murray](https://github.com/jcmurray)

**Release History**

* **V1.0.0** - Initial release

**Known Issues**

* None

**Dependencies**

1. BlackBerry Device Software **10.2.1** and greater
2. There is a small change in the Bluetooth LE APIs in BlackBerry Device Software **10.3** over **10.2.1** where some callbacks have been deprecated and replaced. I've addressed this by including conditional compilation code for the **10.2.1** and **10.3** cases. Moving forward the **10.3** ones should be used.

**How to Build GattServerSample**

Simply Import the project into a workspace in your NDK. 

The **GattServerSample** application consists of two interrelated projects:

* ***GattServerRandom*** -- The application that hosts the GATT Random Number Server (RNS).
* ***SimpleRnsClient*** -- The application that acts as a client to the GATT RNS application.

These can be found under the ***GattServerSample*** top level folder. Copy this folder and all its contents to a location of your choosing then import these four projects into a workspace in your NDK.

You should review the two projects after they have been imported. Each application is independent of the other and they are only grouped together for convenience. Just build each application on its own and install one on one BlackBerry 10 device to act as the server and the other on another BlackBerry 10 device to act as the client.

**But I don't want to build them myself!**

If you don't want to build this sample application yourself we've included a pre-built and signed BAR files. You can find it in the folder "**installable-bar-files**" in each application project folder.
 

**Can I use a GATT client other than the supplied client application?**

***Absolutely!***

Simply connect to the GATT service exposed with the proprietary UUID ***0x3759E92B-4E2C-4EFD-97B5-AABBCCDDEEAA*** using the client of your choice and then write data into the ***Client Characteristic Configuration*** (***0x2902***) attribute associated with proprietary UUID ***0x3759E92B-4E2C-4EFD-97B5-000000000000*** which should appear as service handle ***3*** in the Service Declaration.

Write a value of ***0x0001*** to service handle ***3*** and ***GattServerRandom*** will start sending notifications containing a random byte value in the range 0-255 at 1 second intervals.

Write a value of ***0x0000*** to service handle ***3*** and ***GattServerRandom*** will stop sending notifications.

**To contribute code to this repository you must be [signed up as an 
official contributor](http://blackberry.github.com/howToContribute.html).**

## Contributing Changes

Please see the [README](https://github.com/blackberry/Cascades-Community-Samples/blob/master/README.md) of the Cascades-Community-Samples repository for instructions on how to add new Samples or make modifications to existing Samples.

## Bug Reporting and Feature Requests

If you find a bug in a Sample, or have an enhancement request, simply file an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues) for the Sample and send a message (via github messages) to the Sample Author(s) to let them know that you have filed an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues).


## Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.