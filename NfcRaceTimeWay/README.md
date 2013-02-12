# NfcRaceTimeWay -- Fun Run Application

The purpose of this application is to demonstrate how to use selected 
BlackBerry 10 NFC APIs in the context of a practical real-world use case. The Release History section gives details of those NFC APIs which are covered an any changes as the sample evolves over time.

The use case examines how to use an NFC Tag to trigger the starting and stopping of a timer in the application that could be used in the context of an event like a "fun run". In addition it demonstrates how to use Virtual Tag Emulation to pass information from the handset to an NFC reader that could be used to support the use of way points during the evvent.

The sample code for this application is Open Source under 
the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://bdsc.webapps.blackberry.com/cascades/)
* [BlackBerry Native SDK](http://developer.blackberry.com/native/)

**Author(s)** 

* [John Murray](https://github.com/jcmurray)
* [Martin Woolley](https://github.com/mdwoolley)


**Release History**

* **V1.1** - Initial release
* **V1.1.1** - Updates to support 720x720 screen size

**Dependencies**

1. BlackBerry 10 Device Software **10.0.9**
1. BlackBerry 10 Native SDK **10.0.9** (Gold)

**How to Build NfcRaceTimeWay**

Simply Import the project into a workspace in your NDK. Of course you should review the project after it's been imported paying particular attention to the settings in the **bar-descriptor.xml** file. It should be changed to reflect values in the **&lt;author&gt;** and **&lt;authorId&gt;** elements that are appropriate for you.

**What else will I need?**

You will also need two NFC tags. One to start the timer and one to stop the timer. They should have the following NDEF content.

1. Tag used to *start* the timer
	1. NDEF TNF = External ( integer value 4 )
	1. Type = "my.rim.com:myrecordtype"
	1. Payload = "start"
1. Tag used to *stop* the timer
	1. NDEF TNF = External ( integer value 4 )
	1. Type = "my.rim.com:myrecordtype"
	1. Payload = "stop"
 
This aplication also emulates a virtual NFC tag with contents that represent the calue of the time currently displayed on the handset. If you want to read this data from another device or NFC reader you need to know the format of the NDEF Message in the virtual tag emulated by the application.

1. Virtual tag emulated by the application
	1. NDEF TNF = External ( integer value 4 )
	1. Type = "my.rim.com:myrecordtype"
	1. Payload = "hh:mm:ss"
		1. That is the time displayed on the handset in:
			1. Hours (hh);
			1. Minutes (mm), and;
			1. Seconds (ss).; 

**But I don't want to build it myself!**

If you don't want to build this sample application yourself we've included a 
pre-build and signed BAR file. You can find it in the 
folder "installable-bar-files" as follows:

* **NfcRaceTime-1\_1\_0\_10.bar** -- Signed BAR file for Version 1.1 (V1.1) of the application ( requires BlackBerry Dev Alpha Beta 3 Device Software (10.0.9) )
* **NfcRaceTime-1\_1\_1\_50.bar** -- Signed BAR file for Version 1.1.1 (V1.1.1) of the application ( requires BlackBerry 10 Device Software (10.0.9) )

**To contribute code to this repository you must be [signed up as an 
official contributor](http://blackberry.github.com/howToContribute.html).**

**Known Issues**

None.

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