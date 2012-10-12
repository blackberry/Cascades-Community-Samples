# A Compendium of NFC Samples

The purpose of this application is to demonstrate how to use selected 
BlackBerry 10 NFC APIs. The Release History section gives details of 
those NFC APIs which are covered.

The sample code for this application is Open Source under 
the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://bdsc.webapps.blackberry.com/cascades/)
* [BlackBerry Native SDK for Tablet OS](https://bdsc.webapps.blackberry.com/native/)

**Author(s)** 

* [John Murray](https://github.com/jcmurray)
* [Martin Woolley](https://github.com/mdwoolley)


**Release History**

* **V4** - Added support for sending ISO7816 APDUs and handling Transaction Events
* **V3.1** - Modified for Beta 3 release compatibility
* **V3** - Added ability to emulate virtual NDEF Tags
* **V2.1** - Refactored to exploit features in BlackBerry Dev Alpha Device 10.0.6
* **V2** - Added transfer of vCard using SNEP
* **V1** - Reading and Writing NDEF tags

**Known Issues**

None.

**Dependencies**

1. BlackBerry Dev Alpha Beta 3 Device Software **10.0.9**
1. BlackBerry 10 Native SDK **10.0.9**

**How to Build NfcTool**

Simply Import the project into a workspace in your NDK. Of course you should review the project after it's been imported paying particular attention to the settings in the **bar-descriptor.xml** file. It should be changed to reflect values in the **&lt;author&gt;** and **&lt;authorId&gt;** elements that are appropriate for you.
 
**But I don't want to build it myself**

If you don't want to build this sample application yourself we've included a 
pre-build and signed BAR files for each version. You can find them in the 
folder **"/installable-bar-files"** as follows:

* **NfcTool-4\_0\_0\_2.bar** -- Signed BAR file for Version 4.0 (V4.0) of the application ( requires BlackBerry Dev Alpha Device Beta 3 Software 10.0.9 )
* **NfcTool-3\_0\_0\_51.bar** -- Signed BAR file for Version 3.0 (V3.0) of the application ( requires BlackBerry Dev Alpha Device Software 10.0.6 )
* **NfcTool-2\_1\_0\_2.bar** -- Signed BAR file for Version 2.1 (V2.1) of the application ( requires BlackBerry Dev Alpha Device Software 10.0.6 )
* **NfcTool-2\_0\_0\_1.bar** -- Signed BAR file for Version 2 (V2) of the application  ( requires BlackBerry Dev Alpha Device Software 10.0.4 )
* **NfcTool-1\_0\_0\_1.bar** -- Signed BAR file for Version 1 (V1) of the application ( requires BlackBerry Dev Alpha Device Software 10.0.4 )

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