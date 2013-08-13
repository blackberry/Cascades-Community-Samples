# Sharing Data and Files using NFC and Bluetooth

The purpose of this application is to demonstrate how to use selected 
BlackBerry 10 NFC APIs focussing particularly on those that allow you to share data and files either directly using NFC or using NFC to negotiate a Bluetooth connection which is then used to transfer the data as defined by the **NFC Forum** *Connection Handover* standard. The Release History section gives details of those NFC APIs which are covered.

The sample code for this application is Open Source under 
the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://developer.blackberry.com/cascades/)
* [BlackBerry Native SDK](http://developer.blackberry.com/native/)

**Author(s)** 

* [John Murray](https://github.com/jcmurray)
* [Martin Woolley](https://github.com/mdwoolley)


**Release History**

* **V1.0.0** - Inital version.
* **V1.0.1** - Fix to minor race condition.
* **V1.1.0** - Brought into line with Gold NDK and 720x720 screen size.
* **V1.1.1** - Minor fix to File Share Checkboxes and checked on 10.2

**Known Issues**

None.

**Dependencies**

1. Requires BlackBerry 10 Device Software **10.0.9** or above
1. Requires BlackBerry 10 Native SDK **10.0.9** (Gold) or above

**How to Build NfcSharing**

Simply Import the project into a workspace in your NDK. Of course you should review the project after it's been imported paying particular attention to the settings in the **bar-descriptor.xml** file. It should be changed to reflect values in the **&lt;author&gt;** and **&lt;authorId&gt;** elements that are appropriate for you.
 
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