# NFC Chicken

The purpose of this application is to demonstrate how to use an NFC tag to trigger an action. In this case, our lovely NFC Chicken lays a Cascades flavoured, animated egg right on screen, whist emitting an endearing clucking sound when the BlackBerry 10 device comes into proximity with an NFC tag which contains suitable NDEF content.

Specifically, to trigger NFC Chicken into action, you must present a tag that contains the following NDEF content:

* TNF=4
* Type=my.rim.com:chicken
* Payload=

Note that in this version of the app, the payload field is ignored. 


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

**Known Issues**

None

**Dependencies**

1. BlackBerry 10 Device Software **10.0.9**
1. BlackBerry 10 Native SDK **10.0.9** (Gold)

**How to Build NFC Chicken**

Simply Import the project into a workspace in your NDK. Of course you should review the project after it's been imported paying particular attention to the settings in the **bar-descriptor.xml** file. It should be changed to reflect values in the **&lt;author&gt;** and **&lt;authorId&gt;** elements that are appropriate for you.
 
**But I don't want to build it myself**

If you don't want to build this sample application yourself we've included a 
pre-build and signed BAR files for each version. You can find them in the 
folder **"/installable-bar-files"** as follows:

* **NfcChicken-1\_0\_0\_19.bar** -- Signed BAR file for Version 1 (V1) of the application ( requires BlackBerry 10 Device Software 10.0.9 )

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