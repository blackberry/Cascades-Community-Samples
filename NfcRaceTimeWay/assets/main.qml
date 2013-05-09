/* Copyright (c) 2012 Research In Motion Limited.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
import bb.cascades 1.0
import CustomTimer 1.0

NavigationPane {
    id: navigationPane
    Page {
        Container {

		    id: regDetails
		    objectName: "regDetails"

// ======== Properties ========

            property variant launchedByInvoke: _propertyMap.LaunchedByInvoke    

            property variant digit:  [
                "asset:///images/clock/dg0.PNG",
                "asset:///images/clock/dg1.PNG",
                "asset:///images/clock/dg2.PNG",
                "asset:///images/clock/dg3.PNG",
                "asset:///images/clock/dg4.PNG",
                "asset:///images/clock/dg5.PNG",
                "asset:///images/clock/dg6.PNG",
                "asset:///images/clock/dg7.PNG",
                "asset:///images/clock/dg8.PNG",
                "asset:///images/clock/dg9.PNG"
            ]
            property variant colon:   "asset:///images/clock/dgc.PNG"

            property variant menuColour: Color.create ("#262626")

// ======== SIGNAL()s ========

    	    signal startEmulatingTag(string data)
    	    signal updateEmulatingTag(string data)
    	    signal stopEmulatingTag()

// ======== SLOT()s ========

			function tagEmulationSelectEvent() {}
			function tagEmulationReadEvent() {
			    regDetails.background = Color.Green;
			    tagReadTimerEvent.start();
			}
			function tagEmulationLeftEvent() {}

// ======== Local functions ========

            function raceTagDetected(content) {
                logMessage(content);
                if (content === "start" )  {
                    startRace();
                } else if (content === "stop") {
                    stopRace();
                }
            }

            function startRace() {
	            if ( !isRaceInProgress() ) {
                    resetButton.enabled = false;
	                setStartTime();
                    setRaceInProgress(true);
                    setRaceFinished(false);
	                startTagEmulation();
	                logMessage(qsTr("Race clock started and race control notified of start time"));
	                appDisplayTimer.start();
	                
	            } else {
	                logMessage(qsTr("Race is already in progress"));
	            }
	        }

            function stopRace() {
                if ( isRaceInProgress() ){           
                    resetButton.enabled = true;
	                appDisplayTimer.stop();
                    setStopTime();
                    setRaceInProgress(false);
                    setRaceFinished(true);
	                logMessage(qsTr("Race clock stopped and race control notified of finish time"));
	                displayFinalDuration();
	                resetButton.enabled = true;

	            } else {
	                logMessage(qsTr("Race has not yet started"));
	            } 
	        }

			function isLaunchedByInvoke() {
			    return (launchedByInvoke === "true")
			}

			function isRaceInProgress() {
			    return (_app.getValueFor("raceInProgress", "false") === "true")
			}

			function isRaceFinished() {
			    return (_app.getValueFor("raceFinished", "false") === "true")
			}

            function setRaceInProgress(value) {
                var trueOrFalse = (value === true) ? "true" : "false";
                _app.saveValueFor("raceInProgress", trueOrFalse);
            }
            			
            function setRaceFinished(value) {
                var trueOrFalse = (value === true) ? "true" : "false";
                _app.saveValueFor("raceFinished", trueOrFalse);
            }
            			
            function timeNow() {
                return (new Date()).getTime();
            }
            
            function setStartTime() {
                _app.saveValueFor("raceStartTime", timeNow());
            }
            
            function setStopTime() {
                _app.saveValueFor("raceStopTime", timeNow());
            }

            function getStartTime() {
                return _app.getValueFor("raceStartTime", timeNow());
            }
            
            function getStopTime() {
                return _app.getValueFor("raceStopTime", timeNow());
            }
            
            function getFinalDuration() {
                return getStopTime() - getStartTime();
            }

            function getCurrentDuration() {
                return timeNow() - getStartTime();
            }

            function getDurationHours(duration) {
                var oneHour = 1000 * 60 * 60;
                return Math.floor(duration / oneHour);
            }

            function getDurationMinutes(duration) {
                var oneHour   = 1000 * 60 * 60;
                var oneMinute = 1000 * 60;
                return Math.floor( ( duration % oneHour ) / oneMinute );
            }

            function getDurationSeconds(duration) {
                var oneMinute = 1000 * 60;
                var oneSecond = 1000;
                return Math.floor( ( duration % oneMinute ) / oneSecond );
            }

            function displayCurrentDuration() {
                imgHour1.imageSource = regDetails.digit[Math.floor(getDurationHours(getCurrentDuration())/10)];
                imgHour2.imageSource = regDetails.digit[getDurationHours(getCurrentDuration()) % 10];
                imgMin1.imageSource = regDetails.digit[Math.floor(getDurationMinutes(getCurrentDuration())/10)];
                imgMin2.imageSource = regDetails.digit[getDurationMinutes(getCurrentDuration()) % 10];
                imgSec1.imageSource = regDetails.digit[Math.floor(getDurationSeconds(getCurrentDuration())/10)];
                imgSec2.imageSource = regDetails.digit[getDurationSeconds(getCurrentDuration()) % 10];
                counter.visible = true;
            }
            
            function displayFinalDuration () {
                imgHour1.imageSource = regDetails.digit[Math.floor(getDurationHours(getFinalDuration())/10)];
                imgHour2.imageSource = regDetails.digit[getDurationHours(getFinalDuration()) % 10];
                imgMin1.imageSource = regDetails.digit[Math.floor(getDurationMinutes(getFinalDuration())/10)];
                imgMin2.imageSource = regDetails.digit[getDurationMinutes(getFinalDuration()) % 10];
                imgSec1.imageSource = regDetails.digit[Math.floor(getDurationSeconds(getFinalDuration())/10)];
                imgSec2.imageSource = regDetails.digit[getDurationSeconds(getFinalDuration()) % 10];
                counter.visible = true;
            }
            
            function startTagEmulation() {
                var tagData = "00:00:00";            

                if (isRaceInProgress()) {
                    tagData = getCurrentDurationAsString();            
                } else if (isRaceFinished()) {
                    tagData = getFinalDurationAsString();            
                }

    	        regDetails.startEmulatingTag(tagData);
            }
            
            function updateTagEmulation() {
                var tagData = getCurrentDurationAsString();            
                regDetails.updateEmulatingTag(tagData)
            }
            
            function stopTagEmulation() {
                regDetails.stopEmulatingTag()
            }

            function getCurrentDurationAsString() {
                var tagData = "" + Math.floor(getDurationHours(getCurrentDuration())/10)
                                 + (getDurationHours(getCurrentDuration()) % 10)
                                 + ":"
                                 + Math.floor(getDurationMinutes(getCurrentDuration())/10)
                                 + (getDurationMinutes(getCurrentDuration()) % 10)
                                 + ":"
                                 + Math.floor(getDurationSeconds(getCurrentDuration())/10)
                                 + (getDurationSeconds(getCurrentDuration()) % 10);
                return tagData;
            }
                        
            function getFinalDurationAsString() {
                var tagData = "" + Math.floor(getDurationHours(getFinalDuration())/10)
                                 + (getDurationHours(getFinalDuration()) % 10)
                                 + ":"
                                 + Math.floor(getDurationMinutes(getFinalDuration())/10)
                                 + (getDurationMinutes(getFinalDuration()) % 10)
                                 + ":"
                                 + Math.floor(getDurationSeconds(getFinalDuration())/10)
                                 + (getDurationSeconds(getFinalDuration()) % 10);
                 return tagData;
            }

            function logMessage(message) {
                txtMessage.text += ( qsTr("\n") + message );
            }
                                    
            background: Color.create ("#262626")
            
            layout: StackLayout {
            }

            topPadding: 10
            leftPadding: 30
            rightPadding: 30

            Label {
                text: qsTr("Fun Run Timer")
                horizontalAlignment: HorizontalAlignment.Center
                textStyle {
                    base:  SystemDefaults.TextStyles.BigText
                    color: Color.LightGray
                    fontWeight: FontWeight.Bold
                    textAlign: TextAlign.Center                  
                }
            }

            Container {
                layout: DockLayout {
                }

                leftPadding: 20
                bottomPadding: 20
	            preferredWidth: 768
	            preferredHeight: 1280
                horizontalAlignment: HorizontalAlignment.Center

	            Container {
	                layout: StackLayout {
	                }
					Container {
					    layout: StackLayout {
					        orientation: LayoutOrientation.LeftToRight
					    }
		    
			            Button {
			                id: resetButton
			                text: qsTr ("Reset Timer")
			                imageSource: "asset:///images/tag.png"
			                enabled: false
			                layoutProperties: StackLayoutProperties {
			                }
		                    horizontalAlignment: HorizontalAlignment.Center
			                onClicked: {
			                    regDetails.setRaceFinished(false);
				                resetButton.enabled = false;
				                counter.visible = false;
				                logMessage(qsTr("Race clock reset - tap to start race timer."));
				                regDetails.stopTagEmulation();
			                }
			            }
			
			            Button {
			                id: closeButton
			                text: qsTr ("Close App")
			                imageSource: "asset:///images/tag.png"
			                layoutProperties: StackLayoutProperties {
			                }
		                    horizontalAlignment: HorizontalAlignment.Center
			                onClicked: {
			                    if ( regDetails.isRaceInProgress() || regDetails.isRaceFinished() ) {
                                    regDetails.stopTagEmulation();
			                    }
		                        _app.exitApp();
			                }
			            }
			        }

		            Container {
		                layout: StackLayout {
		                        orientation: LayoutOrientation.LeftToRight
			            }
			            id: counter
			            objectName: "counter"
			            visible: false
			            LedDigit {
			                id: imgHour1
			                objectName: "imgHour1"
			            }
			
			            LedDigit {
			                id: imgHour2
			                objectName: "imgHour2"
				        }
			
			            LedColon {
			                id: imgSep1
			                objectName: "imgSep1"
			            }
			
			            LedDigit {
			                id: imgMin1
			                objectName: "imgMin1"
			            }
			
			            LedDigit {
			                id: imgMin2
			                objectName: "imgMin2"
				        }
			
			            LedColon {
			                id: imgSep2
			                objectName: "imgSep2"
			            }
			
			            LedDigit {
			                id: imgSec1
			                objectName: "imgSec1"
			            }
			
			            LedDigit {
			                id: imgSec2
			                objectName: "imgSec2"
				        }
		            }
		
		            // This area just used for debugging -- not visible in "production"
				    TextArea { 
				        id: txtMessage
				        objectName: "txtMessage"
				        editable: false
				        visible: false
				        text: qsTr("")
				        textStyle {
				            base: SystemDefaults.TextStyles.BodyText
				            color: Color.LightGray
				            lineHeight: 1.1
				        }
				    }
	            }

                TextArea {
                    text: qsTr("NFC Race Time Waypoint V1.1.1 written by:\n John Murray (@jcmrim) & Martin Woolley (@mdwrim)\n© 2012 Research In Motion Limited.")
                    editable: false
                    verticalAlignment: VerticalAlignment.Bottom
                    horizontalAlignment: HorizontalAlignment.Center
                    inputMode: TextAreaInputMode.Text
                    input.flags: TextInputFlag.SpellCheckOff
                    content.flags: TextContentFlag.ActiveTextOff | TextContentFlag.EmoticonsOff 
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                        fontWeight: FontWeight.W100
                        color: Color.White
                        fontStyle: FontStyle.Italic
                        textAlign: TextAlign.Center
                        lineHeight: 1.1
                    }
                }
	        }

            Timer {
                id: appDisplayTimer
                interval: 1000
                onTimeout: {
                    appDisplayTimer.stop();
                    regDetails.displayCurrentDuration();
                    regDetails.updateTagEmulation();
                    appDisplayTimer.start();
                }
            }            

            Timer {
                id: tagReadTimerEvent
                interval: 500
                onTimeout: {
                    tagReadTimerEvent.stop();
                    regDetails.background = regDetails.menuColour;
                }
            }            

            onCreationCompleted: {
                if ( isRaceInProgress() && !isRaceFinished()){
                    logMessage(qsTr("Race in progress - curent time displayed. Tap to stop race timer."));
                    resetButton.enabled = false;
                    startTagEmulation();
                    appDisplayTimer.start();

                } else if (!isRaceInProgress() && !isRaceFinished() ) {
                    logMessage(qsTr("Tap to start race timer"));

                } else if ( isRaceFinished() ){
                    logMessage(qsTr("Race finished - finished time displayed"));
                    displayFinalDuration();
                    startTagEmulation();
                    resetButton.enabled = true;
                }
            }
        }
    }
}
