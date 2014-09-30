# HostCardEmulation

The purpose of this application is to demonstrate how to develop an application for BlackBerry 10 which that uses Near Field Communication (**NFC**) Host Card Emulation (**HCE**) that is capable of responding to a subset of **EMV**(tm) commands from a Point of Sale Terminal.

The sample code for this application is Open Source under 
the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://bdsc.webapps.blackberry.com/cascades/)

**Author(s)** 

* [John Murray](https://github.com/jcmurray)

**Release History**

* **V1.0.0** - Initial release
* **V1.0.1** - Correct the AID registration code for 10.3
* **V1.0.2** - Fix logic around button enabling/disabling with HCE in 10.3
* **V1.0.3** - Fix logic on invocation via HCE in 10.3

**Known Issues**

* There is an issue with running this on devices running, specifically, ***10.3 GOLD*** (***10.3.0.698***). This has been fixed for subsequent ***10.3*** versions. I'd recommend running this on version ***10.2.1*** initially. 

**Dependencies**

1. BlackBerry Device Software **10.2.1** and greater

**How to Build HostCardEmulation**

Simply Import the project into a workspace in your NDK. 

**But I don't want to build it myself!**

If you don't want to build this sample application yourself we've included a pre-built and signed BAR file. You can find it in the folder "**installable-bar-files**".
 
**To contribute code to this repository you must be [signed up as an 
official contributor](http://blackberry.github.com/howToContribute.html).**

## Contributing Changes

Please see the [README](https://github.com/blackberry/Cascades-Community-Samples/blob/master/README.md) of the Cascades-Community-Samples repository for instructions on how to add new Samples or make modifications to existing Samples.

## Bug Reporting and Feature Requests

If you find a bug in a Sample, or have an enhancement request, simply file an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues) for the Sample and send a message (via github messages) to the Sample Author(s) to let them know that you have filed an [Issue](https://github.com/blackberry/Cascades-Community-Samples/issues).


## Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.