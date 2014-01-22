Cloudy Pics
===========

By Paul Bernhardt ([@PBernhardt](https://twitter.com/PBernhardt "PBernhardt on Twitter"))

This is a sample app designed to show how to use the BlackBerry Cascades Camera API (https://developer.blackberry.com/cascades/reference/bb__cascades__multimedia__camera.html). It will eventually be a fully featured camera app, with modular features developers can use.

A popular use for this app has been to deploy it in the Work Perimiter so a camera app is available there (which you are welcome to do). To aid with that, Cloudy Pics can handles the same type of invocations as the default camera app.

Currently, you can:
* Control the Camera Roll with CameraRollManager
* Save and restore camera settings with CameraSettingsStore
* Be invoked like the built-in camera app with CardManager
* Control the focus region

Future plans include:
* A UI to view/change all camera settings
* Email photo to a specified address (ie, for Picasa email upload support)

Want it to do something else? Send me a tweet.

CloudyPics has been featured on the BlackBerry Developer Blog several times, where you can see more in-depth documentation on it's features:
http://devblog.blackberry.com/2013/10/cloudy-pics-part-1-the-introduction/
http://devblog.blackberry.com/2013/10/cloudy-pics-part-2-managing-the-camera-roll/
http://devblog.blackberry.com/2013/12/cloudy-pics-part-3-persisting-camera-settings/
http://devblog.blackberry.com/2014/01/cloudy-pics-part-4-cards-and-enterprise/

Control the Camera Roll
-----------------------
CameraRollManager will display a dialog to the user with a list of possible paths for the Camera Roll (ie, folders that new images will be saved to).

To use it, call promptCameraRollPath. If you supply the current camera roll path, if that option is in the list of supported paths, it will be selected.

This will display the camera roll dialog. The user will then be able to select a new path and hit OK, or hit cancel. If the user selects ok, the cameraRollPathUpdated signal is fired with the new path. If the select Cancel, the cameraRollPathNotUpdated signal is fired.

The camera roll path is NOT set for you. When you get the cameraRollPathUpdated signal, you would probably want to apply it to your camera. You may or may not want to do anything on cameraRollPathUpdated, depending on your app.

The options that appear in the Camera Roll Dialog are customizable by the developer, by modifying the cameraroll.json file. There are four properties in each json object:
* name: The name to display to the user
* path: The actual file path for the camera.
* required path: This folder is required to exist for the CameraRollManager to consider the option valid. ie, the Dropbox folder needs to exist in order for Dropbox to be a valid option.
* description: A short description for the option. Not currently used.

__NOTE:__ A relative path will refer to the application's root directory.

Save and Restore Camera Settings
--------------------------------
CameraSettingsStore is responsible for saving and restoring camera settings. There are a number of functions available to save, restore, and delete individual settings should you choose, but you probably want to save and restore everything.

In a slot for your camera's cameraOpened signal, check to see if the CameraSettingsStore.isEmpty. If it is, set up the camera with some defaults, apply it, and save with saveSettings.

If it's not empty, call restoreAndApplySettings, which, as the name suggests, will restore the old settings and apply them to the camera.

Then, when you change the camera settings later, make sure you save them with saveSetting or saveSettings.

Be invoked
----------
CardManager provides some simple functions that mostly control the state of the viewfinder when the Cloudy Pics card is invoked or pooled. If you look in bar-descriptor.xml you can see the various ways to invoke the app. These line up with the ways the built in camera app is invoked.

This would have to be more complicated if we wanted a different UI for when the app is invoked, but we do not.

Control the focus region
------------------------
The user can drag and move the focus indicator to focus on different regions of the viewfinder. 

A bit of math is used to convert screen coordinates to viewfinder coordinates, and keep the focus indicator on the viewscreen.
