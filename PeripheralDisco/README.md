# Peripheral Disco

BlackBerry 10.2 introduced the peripheral discovery API. With the Z30 also came USB host mode. This sample shows
you how to use the peripheral discovery API - AND - talk to a serial USB accessory.

This application was originally written as a demo for BlackBerry Jam Asia 2013 (JAM804) - the USB 'accessory' 
you need for this demo is an Arduino and servo. An Arduino sketch is included.

## Hardware Requirements

* BlackBerry Z30
  The BlackBerry Z30 is currently the only inmarket device supporting USB host mode.

* USB OTG Cable
  There is a pin in an OTG cable that signals to the BlackBerry device to become a host.

* Arduino Uno (or similar)
  If you're expecting the Z30 to drive the Arduino and your power hungry motors - then you should probably be
  using an external power supply for the Arduino.

* Small Servo
  Or other output device of your choosing (blinky?) that can demonstrate data is
  correctly being exchanged between the BlackBerry and the USB serial accessory.

## Supported BlackBerry Devices

The BlackBerry Z30 is currently the only inmarket device supporting USB host mode.

## Supported Serial Devices

There is a bug (omission?) in BlackBerry 10.2.0 which means that only a very specific set of USB devices are supported.
This does include FTDI serial adaptors. 

However - generic serial support will not be available until 10.2.1. 

This means if you want to use a generic serial device (such as an Arduino Uno R3) you will need either:

* To wait for 10.2.1

* An FTDI cable to connect into the TX/RX pins of the Arduino.

## Info

**License**

The sample code for this application is Open Source under 
the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://developer.blackberry.com/cascades/)
* [BlackBerry Native SDK](http://developer.blackberry.com/native/)

**Author(s)** 

* [Rob Williams](https://github.com/robbieDubya) - Twitter: @robbieDubya

**Release History**

* **V1** - Initial release

**Known Issues**

None

**Dependencies**

1. BlackBerry 10 Device Software **10.2**
1. BlackBerry 10 Native SDK **10.2** 

**How to Build Peripheral Disco**

Import the project into a workspace inside Momentics.
 
**To contribute code to this repository you must be [signed up as an 
official contributor](http://blackberry.github.com/howToContribute.html).**

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
