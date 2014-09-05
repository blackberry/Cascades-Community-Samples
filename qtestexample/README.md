# QtExample Sample

This is a sample app making use of Qt Test framework in order to test some of its components.  This sample is meant to be usable as a starting point for anyone writing their app on how to test their features.

Things this code does:

* provides dummy sample for testing purposes:
   * The XML HTML Info example provides statistics about the content of HTML files.
   * The files are parsed using a QXmlStreamReader object. If the file does
     not contain a well-formed XML document, a description of the error is
     reported.
   * Note: This is the functionality it is testing.
* shows how to integrate and write tests using qttest. The required library('testlib') is already provided on the device.

**Applies To**

* [BlackBerry 10.3 Native SDK](https://developer.blackberry.com/native/downloads/)

**Author(s)** 

* BlackBerry

**Built For BlackBerry**

* Visit the [Built For BlackBerry](https://developer.blackberry.com/builtforblackberry/documentation/overview.html) program overview page.

**Icons**

* N/A

**Contributing**

* To contribute code to this repository you must be [signed up as an official contributor](http://blackberry.github.com/howToContribute.html).


## How To Build

1. Clone the repo to your computer.
2. Launch BlackBerry 10 Native SDK, and from the File menu, select Import.
3. Expand General, and select Existing Projects into Workspace. Click Next.
4. Browse to the location where you extracted the sample file, and then click OK.
5. The sample project should display in the Projects section. 
   Click Finish to import the project into your workspace.
6. In the Project Explorer pane, Right-click the project (for example hellocascades) 
   and select Build Project.
7. In the Project Explorer pane, Right-click the project (for example hellocascades) 
   and select Run As > BlackBerry C/C++ Application.
8. The application will now install and launch on your device. If it doesent you might
   have to [set up your environment](http://developer.blackberry.com/cascades/documentation/getting_started/setting_up.html).


## More Info

* [BlackBerry Cascades & NDK](https://developer.blackberry.com/native) - Downloads, Getting Started guides, samples, code signing keys.
* [Native Development Guides](https://developer.blackberry.com/native/documentation/cascades/)
* [BlackBerry Native Development Forums](https://developer.blackberry.com/native/documentation/cascades/)


## Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.




