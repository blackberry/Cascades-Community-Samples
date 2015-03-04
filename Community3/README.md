Community 3

Community 3 is based off of the BlackBerry Community App, which is available in BlackBerry World here:  http://appworld.blackberry.com/webstore/content/46922891/?lang=en&countrycode=CA&CPID=CRM_E_BBW_01_CA_BBW_0114_0000169598_CA20359664-2

Community 3 can be used as a template to create a community application for your own community that uses forums from Lithium Technologies.
Lithium Technologies: http://www.lithium.com/  Customize this template by adding the URL to your forums, blogs (blogs can be hosted anywhere) 
and BBM Channels along with icon imagery that reflects your brand.  Instructions for customizing this template application can be found below.

Community 3 contains imagery that cannot be released under the Apache license.  These in  
app icons have been replaced with placeholders in this project.  They can be downloaded here:
https://developer.blackberry.com/devzone/design/bb10/iconography.html
Placeholder icons available for download using the link above all start with ic_.


Author(s)
Mark Sohm


Requirements
BlackBerry Native SDK 10.2 or higher
BlackBerry 10 OS 10.2 or higher


Importing a project into the Native SDK

Download the project from Github.
Launch the BlackBerry Momentics SDK (download from https://developer.blackberry.com/native/downloads/)
On the File menu, click Import.
Expand General, and select Existing Projects into Workspace. Click Next.
Browse to the location where you extracted the sample app, and click OK. The sample project should display in the the Projects section.
Click Finish to import the project into your workspace.

Customizing Community 3

There are a few things you'll need to update in the project in order to customize the application to point to your own blogs and forums.
The lines you need to change below have "ToDo" comments next to them.  You can perform a search in the project for "ToDo" to easily locate
the code to make the changes below.

File:  bbmChannels.qml
1. text: qsTr("Your Channel Name")  //ToDo:  Enter your channel name
2. uri: "bbmc:C0001F622"  //ToDo: Your Channel ID here

File: main.qml
1. title: qsTr("Your Blog") + Retranslate.onLocaleOrLanguageChanged  //ToDo: Enter your blog name.
2. blogTabDelegate.object.blogURL = "http://blogs.blackberry.com/"  //ToDo:  Enter your blog URL.
3. title: qsTr("Your Forums") + Retranslate.onLocaleOrLanguageChanged  //ToDo:  Enter your forum name
4. developerForumsDelegate.object.top.categoryId = "blackberrydev";  //ToDo: Enter your forum uber category.
5. developerForumsDelegate.object.top.pageTitle = "Your Forums";  //ToDo:  Enter your forum name
If you wish to add multiple blogs and/or multiple uber forum categories, copy the entire Tab block,
change the value used as the id and repeat the steps above.
6. id: blogTab  //ToDo: Ensure this is a unique name if you add multiple tabs.
7. id: forumTab  //ToDo: Ensure this is a unique name if you add multiple tabs.


File: settingsSheet.qml
1. ToDo Add other options if you add more tabs in main.qml.  If you added or renamed tabs in main.qml, add or rename them here too.

File:  applicationui.cpp
1. ToDo: Define your own UUID here. You can generate one here: http://www.guidgenerator.com/
	const QString uuid(QLatin1String(""));
	
File: ApplicationSettings.cpp
1. : mSettings("Your Company", "Community 3")  //ToDo: Enter your company and app name here.  Use same values in Credentials.	
	
File: Credentials.cpp
1. : mSettings("Your Company", "Community 3")  //ToDo: Enter your company and app name here.  Use same values in ApplicationSettings.

File:  URLProvider.cpp
1. return "";  //ToDo:  Enter the root base URL for your REST API access to your forum. 
2. return "";  //ToDo:  Enter the root base URL for web browser access to your forum.

File: bar-descriptor.xml
1. <!-- ToDo: Create your application invoke ID here. -->  
<invoke-target id="enter.your.id.here">

File: images in assets\images
1. Community 3 contains imagery that cannot be released under the Apache license.  These in  
app icons have been replaced with placeholders in this project.  They can be downloaded here:
https://developer.blackberry.com/devzone/design/bb10/iconography.html
Placeholder icons available for download using the link above all start with ic_.
2. Other images bbmBackground.png, BlackBerryChannel.png, BlogIcon.png and ChannelsIcon.png should
be replaced with your own images.

To contribute code to this repository you must be signed up as an official contributor.

Contributing Changes
Please see the README of the Samples-for-Java repository for instructions on how to add new Samples or make modifications to existing Samples.


Bug Reporting and Feature Requests
If you find a bug in a Sample, or have an enhancement request, simply file an Issue for the Sample and send a message (via github messages) to the Sample Author(s) to let them know that you have filed an Issue.


Disclaimer
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.