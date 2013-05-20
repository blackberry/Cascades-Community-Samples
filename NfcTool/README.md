# A Compendium of NFC Samples

The purpose of this application is to demonstrate how to use selected 
BlackBerry 10 NFC APIs. The Release History section gives details of 
those NFC APIs which are covered.

The sample code for this application is Open Source under 
the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://developer.blackberry.com/cascades/)
* [BlackBerry Native SDK](http://developer.blackberry.com/native/)

**Author(s)** 

* [John Murray](https://github.com/jcmurray) - Twitter: @jcmrim
* [Martin Woolley](https://github.com/mdwoolley) - Twitter: @mdwrim


**Release History**

* **V5.0.1** 
	* NDEF data included in Tag Details screens
	* UI text simplified
	* removed URI recognition and automatic capitalisation from input fields

* **V5** 

 Added new functions:
   * Tag Details
   * Emulate Echo Applet
   * Read ISO15693
   * Amsterdam Transport Pass

 Updated:
   * ISO7816 APDU to allow "select only" and interaction with an external contactless card rather than internal secure element in UICC

* **V4** - Added support for sending ISO7816 APDUs and handling Transaction Events
* **V3.1** - Modified for Beta 3 release compatibility
* **V3** - Added ability to emulate virtual NDEF Tags
* **V2.1** - Refactored to exploit features in BlackBerry Dev Alpha Device 10.0.6
* **V2** - Added transfer of vCard using SNEP
* **V1** - Reading and Writing NDEF tags

**Functions**

* **Read NDEF**

    NfcTool is registered as an invoke target for tags containing the following well known types (TNF=1) of NDEF message:
   * Text
   * URI
   * Smart Poster

 The Read NDEF function simply switches to the event log page where details of any tag read will be displayed. Presenting a tag when not at this page will cause the event log to be displayed as if you had manually selected the item from the menu.

* **Tag Details**

 Selecting this function displays the event log page. Present a tag and its details (not content) will be displayed, including the tag's serial number / UID.

* **Write URI**

 Use this function to write a TNF 1 type U NDEF message to a tag.

* **Write Smart Poster**

 Use this function to write a TNF 1 type Sp NDEF message to a tag.

* **Write Text**

 Use this function to write a TNF 1 type T NDEF message to a tag.

* **Write Custom**

 Use this function to write a TNF 4 ("External") NDEF message of a custom type specified by you to a tag.

* **Send vCard (SNEP)**

 Transfer a vCard to another device using NFC P2P mode and the SNEP protocol

* **Emulate Echo Applet**

 Performs virtual target emulation and specifically emulates a commonly used testing applet which echoes back any APDUs it received. Use this function in conjunction with a source of ISO7816-4 command APDUs such as a contactless reader connected to a PC, driven by a GPShell script.

* **ISO7816-4 APDU**

 Lets you select a specified applet by AID and optionally then send a command APDU whose content you may specify. Two applet host types are supported; the secure element in a UICC (SIM) or an external, contactless card presented to the device.

* **Read ISO15693**

 This function should be regarded as "experimental". At this stage all it is able to do is extract attributes of a ISO15693 tag but not read it properly using the relevant ISO15683 READ commands.

* **Amsterdam Transport Pass**

 Reads the available transaction details from the GVB transport tickets used on the trams in Amsterdam.

* **About**

 Displays version and author details. 


**Known Issues**

None.

**Dependencies**

1. Minimal requirement BlackBerry 10 Device Software **10.0.9**
1. Minimal requirement BlackBerry 10 Native SDK **10.0.9**

**How to Build NfcTool**

Simply Import the project into a workspace in your NDK. Of course you should review the project after it's been imported paying particular attention to the settings in the **bar-descriptor.xml** file.
 
**But I don't want to build it myself**

If you don't want to build this sample application yourself we've included a 
pre-build and signed BAR files for each version. You can find them in the 
folder **"/installable-bar-files"** as follows:

* **NfcTool-5\_0\_1\_248.bar** -- Signed BAR file for Version 5.0.1 (V5.0.1) of the application ( requires BlackBerry 10 Software 10.1 )

* **NfcTool-5\_0\_0\_245.bar** -- Signed BAR file for Version 5.0 (V5.0) of the application ( requires BlackBerry 10 Software 10.0.9 )

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