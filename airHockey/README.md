Air Hockey -- 
The purpose of this application is to demonstrate how to use selected BlackBerry 10 APIs in the context of an air hockey playing robot. The Release History section gives details of any changes as the sample evolves over time.

The use case examines how to interface to a Bluetooth device in order to control relays from the device. The application uses the camera to track a puck, and then moves the robot to the correct position using bluetooth.  The setup at my location has a BlackBerry Z30 mounted above the table on a microphone stand.  The Z30 uses its camera to detect the puck (based on the colour) and move the "Goalie" to the correct position.  

It does this: https://www.youtube.com/watch?v=T9ht3vPhCQo

The goalie is controlled via Bluetooth with an Arduino.  The code is based on the "BestCamera" app and the "Råck2000" demo app.  All that the Arduino does is switch the arm left/right/off using an H bridge.  The Z30 takes care of all the rest.

I created this project as part of a blackberry hackathon in Feb 2014.

This code is by no means intended to be for production it does however work.

A bit of background:
I work for Blackberry/TAT Sweden and we had a company hackathon.  I wanted to build something interesting that combined external hardware and a BlackBerry as part of the hackathon.  Its important to note that this code was written over a weekend and is VERY hacky.  The base of this code is two different examples  the first is the bestcam app and the second is the Råck2000 demo.  The project was a fun weekend project and if I have more time I may decide to continue it but right now I'm happy with how it performs and don't plan to do much more.
As is said here in Sweden, "Lagom är bäst".

Demo apps I used:
https://github.com/blackberry/Cascades-Community-Samples/tree/master/BestCamera
https://github.com/blackberry/Presentations/tree/master/2013-BlackBerryJam-Asia/General%20Session/rack2000

To use this code you will need to do the following:
1. Get an air hockey table and mount a BlackBerry Z30 above it
2. Get an arduino and bluetooth board + relay board.
3. Setup said boards like so: https://github.com/blackberry/Presentations/tree/master/2013-BlackBerryJam-Asia/General%20Session/rack2000
4. Hookup the first 2 relays in an "H bridge"  and then give them 12 V
5. Hook up a windshield wiper motor to these two relays outputs (so it will go forward/backwards)
6. Get a piece of aluminum tube for an arm and make it look something like the youtube video above.
7. VERY IMPORTANT: You MUST add limit switches with diodes so that they physically disconnect power from the motor
8. Make sure you have set up number 7 correctly so you don't have a spinning chunk of aluminum.
9. Cover an airhockey puck in green stickers
10. Put it all together and play air hockey against your BlackBerry.


The sample code for this application is Open Source under the Apache 2.0 License.

Applies To

    Cascades for BlackBerry 10
    BlackBerry Native SDK

Author(s)

    Stephen Lau

Release History

    V1.0.0 - Initial release

Dependencies

    Minimal requirement BlackBerry 10 Device Software 10.0.9
    Minimal requirement BlackBerry 10 Native SDK 10.0.9

How to Build Airhockey

Simply Import the project into a workspace in your NDK. Of course you should review the project after it's been imported paying particular attention to the settings in the bar-descriptor.xml file. It should be changed to reflect values in the <author> and <authorId> elements that are appropriate for you.

What else will I need?

You will need an arduino with bluetooth shield and relay shield configured as in the Råck2000 demo.  You will need a windsheild wiper motor, some aluminium, limit switches and some sort of power supply.  It would also help to have an airhockey table.  Also see the instructions above!

To contribute code to this repository you must be signed up as an official contributor.

Known Issues

    You should ensure that the arduino that you use has been paired with the BlackBerry 10 device before using the application.

Contributing Changes

Please see the README of the Cascades-Community-Samples repository for instructions on how to add new Samples or make modifications to existing Samples.
Bug Reporting and Feature Requests

If you find a bug in a Sample, or have an enhancement request, simply file an Issue for the Sample and send a message (via github messages) to the Sample Author(s) to let them know that you have filed an Issue.
Disclaimer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.