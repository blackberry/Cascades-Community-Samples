# PasswordSample

The purpose of this application is to demonstrate how the BlackBerry 10 Security Builder API together with standard features of Qt and Cascades might be used to implement a basic user ID and password based login screen whilst allowing the user credentials to be stored safely locally (as in this case) or remotely using one-way encryption of the password rendering it unusable to those with access to the credentials store.

In addition to demonstrating a simple application of the Security Builder APIs, this sample also includes:

* A custom control. The Timer class is exposed to QML and used to remove status messages after a 5 second delay.
* Dynamic management of Tabs in a TabbedPane
* Simple animations. Go on.... touch one of the padlock graphics. You know you want to.....
* Covers.
  * Minimise the application when either "logged in" or not logged in and note the image on the minimised application's Active Frame 
* Use of QSettings for simple persistent storage of name/value pairs
 
The sample code for this application is Open Source under 
the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://developer.blackberry.com/cascades/)
* [BlackBerry Native SDK](http://developer.blackberry.com/native/)

**Author(s)** 

* [Martin Woolley](https://github.com/mdwoolley)
* [John Murray](https://github.com/jcmurray)

**Release History**

* **V1.0.1** - Updates for Gold SDK and 720x720 screen on Dev Alpha "C" 
* **V1** - Initial release

**Known Issues**

None.

**Dependencies**

1. BlackBerry 10 Device Software **10.0.9**
1. BlackBerry 10 Native SDK **10.0.9** (Gold)

**How to Build PasswordSample**

Simply Import the project into a workspace in your NDK. Of course you should review the project after it's been imported paying particular attention to the settings in the **bar-descriptor.xml** file. It should be changed to reflect values in the **&lt;author&gt;** and **&lt;authorId&gt;** elements that are appropriate for you.
 
**But I don't want to build it myself**

If you don't want to build this sample application yourself we've included a 
pre-build and signed BAR files for each version. You can find them in the 
folder **"/installable-bar-files"** as follows:

* **PasswordSample-1\_0\_1\_29.bar** -- Signed BAR file for Version 1.0.1 (V1.0.1) of the application ( requires BlackBerry 10 Device Software 10.0.9 )

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