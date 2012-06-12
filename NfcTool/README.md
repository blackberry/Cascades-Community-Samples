# NFC Tag Reading and Writing Sample

The purpose of this application is to demonstrate how to use selected BlackBerry 10 NFC APIs. The Release History section gives details of those NFC APIs which are covered.

The sample code for this application is Open Source under the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10] (https://bdsc.webapps.blackberry.com/cascades/)
* [BlackBerry Native SDK for Tablet OS](https://bdsc.webapps.blackberry.com/native/)


**Author(s)** 

* [John Murray](https://github.com/jcmurray)
* [Martin Woolley](https://github.com/mdwoolley)


**Release History**
V2 - Added transfer of vCard using SNEP
V1 - Reading and Writing NDEF tags


**Dependencies**

1. BlackBerry Dev Alpha Device Software 10.0.4
2. BlackBerry 10 Native SDK 10.0.4

**How to Build NfcTool**

We took the decision to distribute only the contents of the "src/" and "assets/" directories along with the "NfcTool.pro" file with this example.

We've found it to be the more reliable technique whilst the meta-data associated with an NDK project is still subject to change between versions of the NDK. You are less likely to run into issues with a project if you create it as a new empty project in your workspace and copy files into it rather than import a project with a structure from an older version of the IDE.

Here's what you should do:

* Create a new empty Cascades C++ project in your workspace called "NfcTool"
    * File > New > BlackBerry Cascades C++ Project
    * Enter Project Name: NfcTool > Next
    * Select: Standard empty project > Finish
* You've now got a clean project set up appropriately for your version of the IDE
* Delete the "app.cpp" and "app.hpp" files from the project's "src/" directory -- we don't need them
* Replace contents of the "src/" and "assets/" directories in the project by the contents of "src/" and "assets/" from this repo
* Replace "NfcTool.pro" in the project by the one in this repo
    * This file just ensure that the "libnfc.so" library is available to the build process since it's not included in the library path by default.
* You'll also now have a "bar-descriptor.xml" which is correct for your verion of the IDE.
    * You need to make sure it's populated with your own Debug Token 
    * Also, if you want to ensure that the application has the correct icon and startup splash screen (I'm sure you do!) open the "bar-descriptor.xml" file, select the "Application" tab and choose:
        * "assets/images/generic_tag.png" as the Icon image
        * "assets/images/generic_tag.png" as both the "Landscape" and "Portrait" splash screen images

**But I don't want to build it myself**

If you don't want to build this sample application yourself we've included a pre-build and signed BAR files for each version. You can find them in the folder "installable-bar-files" as follows:

* NfcTool-2_0_0_1.bar -- Signed BAR file for Version 2 (V2) of the application
* NfcTool-1_0_0_1.bar -- Signed BAR file for Version 1 (V1) of the application

**Known Issues**

None

**To contribute code to this repository you must be [signed up as an official contributor](http://blackberry.github.com/howToContribute.html).**


## Contributing Changes

Please see the [README](https://github.com/blackberry/Cascades-Community-Samples/blob/master/README.md) of the Cascades-Community-Samples repository for instructions on how to add new Samples or make modifications to existing Samples.


## Bug Reporting and Feature Requests

If you find a bug in a Sample, or have an enhancement request, simply file an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues) for the Sample and send a message (via github messages) to the Sample Author(s) to let them know that you have filed an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues).


## Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.