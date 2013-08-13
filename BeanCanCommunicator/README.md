# Bean Can Communicator

The purpose of this application is to demonstrate how to use NfcShareManager to send text between two devices using NFC peer to peer mode. It is also intended to entertain due to the blatantly frivolous nature of the app :-) 

To use the application:

* install and run it on two BlackBerry 10 devices
* On one device, touch the text input field at the top of the screen and enter a short message
* Press enter (virtual keyboard) or touch the screen outside the text input area (physical keyboard) and marvel at the way your "note" floats down into the can and along the string
* Touch your device to the other one, back to back so the NFC antennae line up
* The second device will make a noise like an old phone ringing (make sure your volume is turned up) and the text entered on the first device will appear on the second device in a small note which will slide along the string to the can, then float upwards until it is "parked" on the screen near the top


The sample code for this application is Open Source under 
the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://developer.blackberry.com/cascades/)
* [BlackBerry Native SDK](http://developer.blackberry.com/native/)

**Author(s)** 

* [John Murray](https://github.com/jcmurray) - Twitter: @jcmrim
* [Martin Woolley](https://github.com/mdwoolley) - Twitter: @mdwrim


**Release History**

* **V1** - Initial release
* **V1.0.1** - Minor change to text colour

**Known Issues**

When using the app on a BlackBerry 10 device with physical keyboard, it should not be necessary to touch away from the text input field to trigger the sharing action and animation. Pressing the physical enter key should trigger the event in the same way that pressing enter on a virtual keyboard does. This will be addressed in a future release.

**Dependencies**

1. Requires BlackBerry 10 Device Software **10.0.9** and above
1. Requires BlackBerry 10 Native SDK **10.0.9** (Gold) and above

**How to Build BeanCanCommunicator**

Simply Import the project into a workspace in your NDK. Of course you should review the project after it's been imported paying particular attention to the settings in the **bar-descriptor.xml** file. It should be changed to reflect values in the **&lt;author&gt;** and **&lt;authorId&gt;** elements that are appropriate for you.
 
**But I don't want to build it myself**

If you don't want to build this sample application yourself we've included a 
pre-build and signed BAR files for each version. You can find them in the 
folder **"/installable-bar-files"** as follows:

* **BeanCanCommunicator-1\_0\_0\_21.bar** -- Signed BAR file for Version 1 (V1) of the application ( requires BlackBerry 10 Software 10.0.9 )

**To contribute code to this repository you must be [signed up as an 
official contributor](http://blackberry.github.com/howToContribute.html).**

**Attribution**

The photo of a tin can is from [http://en.wikipedia.org/wiki/File:Empty_tin_can2009-01-19.jpg](http://en.wikipedia.org/wiki/File:Empty_tin_can2009-01-19.jpg) and by CC-BY-SA 3.0. 

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