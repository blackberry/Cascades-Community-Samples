# AnnualLeave - BETA - Vacation Approval Application

The purpose of this application is to add some real code to the theory that we outlined in these blog articles and videos. If you haven't read or listened to these then now might be a good time :-). They explain the core architecture upon which this __BETA__ sample application is based.


- [A Flexible Architecture for Enterprise Mobile Applications? - Part 1](http://devblog.blackberry.com/2014/02/a-flexible-architecture-for-enterprise-mobile-applications-part-1/)

- [A Flexible Architecture for Enterprise Mobile Applications? - Part 2](http://devblog.blackberry.com/2014/02/a-flexible-architecture-for-enterprise-mobile-applications-part-2/)

- [A Flexible Project Structure for Enterprise Mobile Applications? - Part 3](http://devblog.blackberry.com/2014/02/a-flexible-project-structure-for-enterprise-mobile-applications-part-three/)

- [VIDEO: A Flexible Architecture for Enterprise Mobile Applications?](http://www.youtube.com/watch?v=UYhbQBp72sQ)

- [VIDEO: A Flexible Project Structure for Enterprise Mobile Applications?](http://www.youtube.com/watch?v=e37vUIv8hMw)

And, yes, it's **BETA** with all that that entails. We took the view that we wanted to get something tangible out to the community as soon as possible even though it has rough edges, is not quite complete and has one or two issues we haven't addressed yet. It will improve and evolve over time.

There is nothing quite like reading the code that implements an architecture to see how things work.

### Background

The model we've implemented is that of a Vacation/Annual Leave request application for BlackBerry 10 devices which could be readily integrated with miscellaneous enterprise annual leave systems (back ends). 

In order to make it easy to demonstrate and test we've written an adapter for this version of the application that simply uses email to communicate requests and approvals between two BlackBerry 10 devices that are using this application. One device could take the role of the Approver ( Manager ) and the other the requestor ( Manager's direct report ).   

We've also made it easy to customise the user interface by separation of the main logic of the visible component of the application into distinct GUI and Model components. The look and feel can be changed without impacting the application logic. 

#### Functionality
Primary functionality includes:

- Request annual leave/vacation

- Modify annual leave request

- Cancel annual leave request/booking

- Receive approval request

- Process approval request

- Provision for initial or periodic synchronisation of requests with an enterprise system.

- Notifications delivered though the Hub

- Ability to add approved requests to the BlackBerry PIM Calendar

- Application available for all BlackBerry 10 devices
 
The sample code for this application is Open Source under 
the [Apache 2.0 License](http://www.apache.org/licenses/LICENSE-2.0.html).

#### Refactoring Performed from Beta 2 to Beta 3

Perhaps the major change between Beta 2 and Beta 3 that's worth describing in more detail has been the refactoring of the main user interface component of the application into two parts:

- A Model component that contains the business logic of the User Interface component of the application and exposes a well defined IModel interface to the GUI component
- A component that concerns itself with only the User Interface elements of the application and interacts with the Model through the IModel interface.

This follows directly from the requirement that user interface elements and look and feel should be decoupled from the business logic of the application to allow flexible customisation and re-skinning  of the user interface. It also follows good design practice by following the **MVC** ( *Model*, *View*, *Controller* ) pattern.
 
### Applies To

- [Cascades for BlackBerry 10](https://bdsc.webapps.blackberry.com/cascades/)
- [BlackBerry Native SDK](http://developer.blackberry.com/native/)

### Author(s) 

- [John Murray](https://github.com/jcmurray)
- [Martin Woolley](https://github.com/mdwoolley)


### Release History

- **Beta 2 (Version 0.0.2)** - Initial Beta Version for public release
- **Beta 3 (Version 0.0.3)** - The main highlighted changes in this Beta Version include:
	- Refactoring of the main application component of the application into a pure GUI and Model components 
	- Request synchronisation with an enterprise system (the GUI and Model components include this ability but the peer-to-peer sample email adapter does not since there is no "back-end server" in the peer-to-peer model from which to synchronise). 
	- Improvements to the configuration of the email adapter settings
	- Fixes to known issues found in the Beta 2 version.

### Dependencies

1. Minimal requrement BlackBerry 10 Device Software **10.2.1 GOLD**
1. Minimal requrement BlackBerry 10 Native SDK **10.2.1 GOLD**

### How to Build AnnualLeave

The AnnualLeave application consists of four interrelated projects:

- __AnnualLeave__ -- the GUI component of the application.  
- __AnnualLeaveModel__ -- This **NEW** component contains all the business specific logic that had been in the *AnnualLeave* component in Beta 2. 
- __AnnualLeaveService__ -- the Headless service component of the application.
- __AnnualLeaveAdapterEmail__ -- a sample adapter, as defined by our architecture, that will allow delivery of annual leave requests and responses to be carried between BlackBerry handsets without the need of a back end system.
- __AnnualLeaveCommon__ -- Location for all the common interface elements used by the other three projects.

These can be found under the __AnnualLeave__ top level folder. Copy this folder and all its contents to a location of your choosing then import these four projects into a workspace in your NDK.

Do **NOT** _copy_ them.You **MUST** ensure that the checkbox entitled __"Copy projects into workspace"__ is **NOT** checked. There are interrelationships between the four constituent projects that will **NOT** be properly preserved if you copy them.

You should review the four projects after they have been imported paying particular attention to the settings in the **bar-descriptor.xml** file in the __AnnualLeave__ project. There is only one **bar-descriptor.xml** for this application and that is the one in the __AnnualLeave__ project. None of the other projects have **bar-descriptor.xml** files! 

To build the application you should build the __AnnualLeave__ project. The other projects will be built automatically as dependent projects.

You should ensure that all four projects have the same settings in terms of __Build Configuration__; __"Device-Debug"__, __"Device-Release"__, etc.

It's good practice to ensure that each of the projects have the correct __"Project References"__ in __"Project Properties"__. 

These should be:

- __AnnualLeave__ references
	- __AnnualLeaveAdapterEmail__,
	- __AnnualLeaveService__,
	- __AnnualLeaveModel__
	- and __AnnualLeaveCommon__,
- __AnnualLeaveModel__ references
	- __AnnualLeaveCommon__
- __AnnualLeaveService__ references
	- __AnnualLEaveAdapterEmail__
	- and __AnnualLeaveCommon__
- __AnnualLeaveAdapterEmail__ references
	- __AnnualLeaveCommon__
- __AnnualLeaveCommon__ has **no** references to other projects

### How to configue the Application

The __Settings__ page can be accessed from the main page of the application by dragging down from the top of the screen.

There are 4 fields that need to be completed - press the __"Save"__ button once you've completed these settings:

- The __"Email Account"__  -- this is the one of the Email account that you have configured for your devcie. Simply choose the appropriate account from the drop-down menu.

- The __"Approver Name"__ field can be anything you want. it's just used as a display name associated with the _Approver's Email Address_ in messages.

- The __"Approver Email Address"__ field -- this is the email address of the _"Manager"_ who will approve Vacation Requests for you. In this model this will be another BlackBerry 10 device that will be running a copy of the same application.

- The __"User Id"__  field -- this field is a display name used to identify you to your _Approver_
That's all there is for configuration!


### But I don't want to build it myself!

If you don't want to build this sample application yourself we've included a 
pre-build and signed BAR file. You can find it in the 
folder "installable-bar-files" as follows:

* **AnnualLeave-0\_0\_3\_4.bar** -- Signed BAR file for Beta 3 (0.0.3) of the application ( minimal requirement BlackBerry 10 Software 10.2.1 )

* **To contribute code to this repository you must be [signed up as an 
official contributor](http://blackberry.github.com/howToContribute.html).**

### Known Issues

This is a __Beta__ version of a sample application that implements the __Flexible Application Architecture__ that we've blogged about. It's not complete and still contains a number of known issues. The purpose of making available such a version is to demonstrate how we approached the implementation of our architecture. We're working on improving this sample application and will release new versions of it as we add new features and make it more rugged. Please recognise the __Beta__ nature of this sample and the spirit in which it has been published.

1. English language text only

1. Outbound emails sent by the application are currently not deleted from the hub.

1. Graphics are not final

1. When running in the work perimeter and the GUI is closed "disco lights" may occasionally be seen on the LED. Headless service continues to run and when GUI is relaunched it continues to function as expected. No core files are seen in the application's "log" folder and slog2 identifies the /bin/cascades binary as having crashed.

1. Sometimes if a POP account is in use, operation messages destined for the application will not be downloaded by the BlackBerry 10 email client. This is an issue with the email client but has the consequence that the application will not receive an update until something prompts the email client to do the download. Sending another message *may* achieve this.

1. Dates are sometimes one day "out" when an approver views an update.
	
	The approver and requester should both be in the same time zone ideally. 

	If they are not then the approver will see dates presented as they would be in their own time zone. Effectively a date range for a holiday has an implicit time of midnight i.e. the very start of the specified date. So if, for example, the approver is in a time zone one or more hours behind the requester, then they will see the dates shifted backwards one day since those one or more hours takes us earlier than midnight and therefore into the day before.

1. It has been noted that the application's headless service may crash and core dump under some circumstances:

		Process 28086283 (AnnualLeaveService) terminated SIGSEGV code=1 fltno=11 ip=78cc6dfa(/apps/com.example.AnnualLeave.testDev_AnnualLeaveb7769d87/native/lib/libAnnualLeaveAdapter.so.1@_ZNK15UserIdCacheItem9getTaskIdEv+0x15) mapaddr=00036dfa. ref=0000010d

	There have been changes to the format of data that is persisted by the Headless Service between Beta 2 and Beta 3.

	You should completely de-install the application and re-install it to remove inconsistent persisted data. Beta 3 identifies the data's format with a verison number so this shouldn't occur in the future.

1. On a device that has both an IMAP and a BES account, adding to the PIM calendar may fail.

	Ensure that the correct default calendar is specified in the BlackBerry 10 "Settings".

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