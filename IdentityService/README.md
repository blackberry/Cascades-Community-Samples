# Identity Service Sample

The purpose of this sample is to show how to use the native (pure C) identity service API.

There will be a different sample showing how to use the Qt identity service API.

### Description of BlackBerry Identity Service
BlackBerry Identity Service is a standardized interface for accessing identifiable information about the user
across multiple Identity Providers or IDPs. Supported Identity providers effectively make it possible for 
developers to pull out data about a user from multiple registered sources, based on the consent of the user. 

### Feature Summary: BlackBerry ID Identity Provider
With BlackBerry ID you are using the built in device authentication to authenticate with your backend. All you 
have to do is link your existing authentication system with BlackBerry Identity Service and use BBID authentication
to identify a user. This would require a 3rd party application to consume the UID that BlackBerry ID provides and
map it to your existing account system.


## Approach

If you're a proficient C coder - you might find this demo convoluted. Please, for your own sanity, avert your eyes.

Using Cascades is the easiest way to get a good UI! Try and ignore the UI classes, just have a look at:
* IDSWrapper
* BlackBerryID
* BlackBerryProfile

The author finds [RAII](http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization) to be one of the strongest 
assets of the c++ language. To this end, the contexts required by the BlackBerry identity service API are allocated and deallocated in
constructors and destructors.

**License**

The sample code for this application is Open Source under 
the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

**Applies To**

* [Cascades for BlackBerry 10](https://developer.blackberry.com/cascades/)
* [BlackBerry Native SDK](http://developer.blackberry.com/native/)

**Author(s)** 

* [Rob Williams](https://github.com/robbieDubya) - Twitter: @robbieDubya

**Release History**

* **V1** - Initial release

**Known Issues**

Inserting a space in a profile key appears to fail at the back end.

**Dependencies**

1. BlackBerry 10 Device Software **10.2**
1. BlackBerry 10 Native SDK **10.2** 

With minimal massaging it will compile on 10.0.

**How to Build Identity Service Demo**

Import the project into a workspace inside Momentics.
 
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
