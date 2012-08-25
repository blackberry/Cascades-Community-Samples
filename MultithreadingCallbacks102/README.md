# Multithreading 102 

Leverages the REST example to show the proper method of breaking a network call into a separate class and have the interface bring back the response into the UI. 

## Sample Description:

 The Multithreading 102 sample demonstrates how a call is made to perform a network operation in the
 background and display the results back into the UI at a separate time

 The app makes a call to retrieve the latest 10 images from the NASA dashboard on Tumblr. The network
 operation is performed in the background, when the response is retrieved, it shows an activity bar
 in the UI while each image is downloaded and converted into a QImage in a background thread. Each
 thread runs independently and updates the UI as image download and processing completes 

 The Tumblr API key used in this sample is for demonstration purposes only. Please register for your 
 own API if you wish to expand on this sample.

## Feature summary
 - Runs multiple threads each updating the UI independently
 - Calls a REST api and processes network data in the background

## Requirements:

 - BlackBerry Cascades SDK for BlackBerry 10 or later
 - One of the following:
   - BlackBerry 10 alpha device/simulator or later

## Importing a project into the Native SDK:

 1. From the the Sample apps page, download and extract the sample application.
 2. Launch the Native SDK.
 3. On the File menu, click Import.
 4. Expand General, and select Existing Projects into Workspace. Click Next.
 5. Browse to the location where you extracted the sample app, and click OK.
    The sample project should display in the the Projects section.
 6. Click Finish to import the project into your workspace.

 ========================================================================
Requirements:

BlackBerry 10 Native SDK Beta

========================================================================
Running the example:

1. Download the sample from http://github.com/blackberry.
2. Launch BlackBerry 10 Native SDK Beta, and from the File menu, select Import.
3. Expand General, and select Existing Projects into Workspace. Click Next.
4. Browse to the location where you extracted the sample file, and then click OK.
5. The sample project should display in the the Projects section. 
   Click Finish to import the project into your workspace.
6. In the Project Explorer pane, Right-click the the project (for example hellocascades) 
   and select Build Project.
7. In the Project Explorer pane, Right-click the the project (for example hellocascades) 
   and select Run As > BlackBerry C/C++ Application.
8. The application will now install and launch on your device if not you might
   have to set up your environment: 
   http://developer.blackberry.com/cascades/documentation/getting_started/setting_up.html

 
 Disclaimer
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.