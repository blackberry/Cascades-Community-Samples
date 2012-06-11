# NFC Tag Reading and Writing Sample

The purpose of this application is to demonstrate how to read and write various sorts of NDEF messages from and to NFC tags using the BlackBerry 10 NFC APIs.

The sample code for this application is Open Source under the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10] (https://bdsc.webapps.blackberry.com/cascades/)
* [BlackBerry Native SDK for Tablet OS](https://bdsc.webapps.blackberry.com/native/)


**Author(s)** 

* [John Murray](https://github.com/jcmurray)
* [Martin Woolley](https://github.com/mdwoolley)


**Dependencies**

1. BlackBerry Dev Alpha Device Software 10.0.4
2. BlackBerry 10 Native SDK 10.0.4

**How to Build NfcTool**

We took the decision to distribute only the contents of the "src/" and "assets/" directories along with the "NfcTool.pro" and "bar-descriptor.xml" files with this example.

We've found it to be the more reliable technique whilst the meta-data associated with an NDK project is still subject to change between versions of the NDK. You are less likely to run into issues with a project if you create it as a new empty project in your workspace and copy files into it rather than import a project with a structure from an older version of the IDE.

Here's what you should do:

* Create a new empty Cascades C++ project in your workspace called "NfcTool"
    * File > New > BlackBerry Cascades C++ Project
    * Enter Project Name: NfcTool > Next
    * Select: Standard empty project > Finish
* Replace contents of the "src/" and "assets/" directories in the project by the contents of "src" and "assets" in this repo
* Replace "NfcTool.pro" and "bar-descriptor.xml" in the project by the ones in this repo
    * These files just ensure that the "libnfc.so" library is available to the build process since it's not included in the library path by default; and that the correct images are used for the application icon and splash screen.

**Known Issues**

None

**To contribute code to this repository you must be [signed up as an official contributor](http://blackberry.github.com/howToContribute.html).**


## Contributing Changes

Please see the [README](https://github.com/blackberry/Cascades-Community-Samples/blob/master/README.md) of the Cascades-Community-Samples repository for instructions on how to add new Samples or make modifications to existing Samples.


## Bug Reporting and Feature Requests

If you find a bug in a Sample, or have an enhancement request, simply file an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues) for the Sample and send a message (via github messages) to the Sample Author(s) to let them know that you have filed an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues).


## Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.