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

// This is the note-component template that is used to setup the three notes in the msgMaker application.

Container {
    id: noteContainer
    maxWidth: 317
    property alias msg: msg_part.text
    // signals
    signal messageTextRecvd(string message)

    // All notes use a DockLayout for centering the text.
    layout: DockLayout {
    }
    animations: [
        TranslateTransition {
            id: hide_send
            toY: -400
            duration: 500
            easingCurve: StockCurve.CubicOut
        },
        TranslateTransition {
            id: show_send
            fromX: 0
            toX: 0
            duration: 500
            easingCurve: StockCurve.QuarticOut
        },
        SequentialAnimation {
            id: send_message
            animations: [
                // float over top of can
                TranslateTransition {
                    toX: 240
                    toY: 30
                    duration: 500
                    easingCurve: StockCurve.SineIn
                },
                // spin round so vertically orientated
                RotateTransition {
                    toAngleZ: 90
                    duration: 250
                },
                // drop into top of can
                TranslateTransition {
                    toY: 90
                    duration: 500
                    easingCurve: StockCurve.SineIn
                },
                // disappear (it's inside the can)
                FadeTransition {
                    duration: 1000
                    fromOpacity: 1.0
                    toOpacity: 0.0
                },
                // invisibly move to start of string
                TranslateTransition {
                    toY: 500
                    toX: 500
                    duration: 100
                },
                // and rotate to horizontal orientation
                RotateTransition {
                    toAngleZ: 0
                    duration: 250
                },
                // and make smaller
                ScaleTransition {
                    duration: 100
                    toX: 0.5
                    fromX: 1.0
                    toY: 0.5
                    fromY: 1.0
                },
                // reappear
                FadeTransition {
                    duration: 500
                    fromOpacity: 0.0
                    toOpacity: 1.0
                },
                // slide along the string and off screen
                TranslateTransition {
                    toX: 900
                    duration: 500
                    easingCurve: StockCurve.SineIn
                },
                // reset to full size for next time
                ScaleTransition {
                    duration: 100
                    toX: 1.0
                    toY: 1.0
                }
            ]
        },
        SequentialAnimation {
            id: hide_recv
            animations: [
                TranslateTransition {
                    toX: 900
                    toY: 500
                    duration: 500
                    easingCurve: StockCurve.CubicOut
                },
                ScaleTransition {
                    duration: 100
                    toX: 1.0
                    toY: 1.0
                }
            ]
        },
        SequentialAnimation {
            id: recv_message
            onEnded: {
                console.log("XXXX recv_message onEnded - calling onMessageTextRecvd");
                talk.onMessageTextRecvd(msg);
            }
            animations: [
                TranslateTransition {
                    fromX: 900
                    toX: 500
                    fromY: 500
                    toY: 500
                    duration: 500
                    easingCurve: StockCurve.QuarticOut
                },
                FadeTransition {
                    duration: 10
                    toOpacity: 1.0
                },
                ScaleTransition {
                    duration: 250
                    toX: 0.5
                    fromX: 1.0
                    toY: 0.5
                    fromY: 1.0
                },
                // disappear
                FadeTransition {
                    duration: 500
                    fromOpacity: 1.0
                    toOpacity: 0.0
                },
                // spin round so vertically orientated
                RotateTransition {
                    toAngleZ: 90
                    duration: 10
                },
                // move to vertical centre of can
                TranslateTransition {
                    toX: 240
                    duration: 100
                    easingCurve: StockCurve.SineIn
                },
                // move to top of can
                TranslateTransition {
                    toY: 50
                    duration: 250
                    easingCurve: StockCurve.SineIn
                },
                // reappear
                FadeTransition {
                    duration: 500
                    toOpacity: 1.0
                },
                // spin back to horizontal orientation
                RotateTransition {
                    toAngleZ: 0
                    duration: 250
                },
                // resize
                ScaleTransition {
                    duration: 250
                    toX: 1.0
                    toY: 1.0
                },
                // disappear
                FadeTransition {
                    duration: 2000
                    fromOpacity: 1.0
                    toOpacity: 0.0
                }            ]
        }
    ]

    // The note background image
    ImageView {
        id: noteBackground
        imageSource: "asset:///images/short_note.png"
    }

    // The msg part, text is set via the msg property on the Note component
    Container {
        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center
        rightPadding: 10
        leftPadding: 10
        Label {
            id: msg_part
            text: ""
            multiline: true
            textStyle {
                color: Color.Black
            }
        }
    }

    // This is the implicit-animation controller for the Note.
    // We need this to set the Note to a new position before showing it without
    // triggering implicit animations (see showNote function below).
    attachedObjects: [
        ImplicitAnimationController {
            id: offScreenController
            propertyName: "translationY"
            enabled: false
        }
    ]

    // This function that triggers the show animation on a Note.
    function showNote() {
        // Implicit animations are turned off for the translationY property,
        // so the movement of a note to align it will happen immediately.
        translationY = 0;
        show_send.play();
    }
    function hideSendNote() {
        hide_send.play();
    }
    function hideRecvNote() {
        hide_recv.play();
    }
    function sendMessage() {
        console.log("XXXX sendMessage");
        send_message.play();
    }
    function preRecv() {
        console.log("XXXX preRecv");
        console.log("XXXX preRecv.hide_recv");
        hide_recv.play();
        //        console.log("XXXX preRecv.show_recv");
        //        show_recv.play();
    }
    function recvMessage() {
        console.log("XXXX recvMessage");
        recv_message.play();
    }
}
