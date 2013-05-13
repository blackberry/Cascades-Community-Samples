/* Copyright (c) 2013 BlackBerry.
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

/**
 * This item is used in the HelpSheet. In short if the user clicks the item its
 * body expands to display more info and a button is added to allow the user to
 * select the body to conclude usage of the info, condensing the item. The item 
 * will also condense if clicked anywhere, but is not considered "answered" in 
 * that case.
 */

Container {
    property alias headerText: labelHeader.text //The header for the item, always visible
    property alias bodyText: bodyText.text //The body, visible when expanded
    property alias answerButtonText: answerButton.text //The answer button, visible when expanded
    property alias bodyVisible: body.visible //The bool to determine if the body is visible
    signal answered() //Signal denoting the answerButton has been clicked

    topPadding: 10
    leftPadding: 10
    rightPadding: 10

    Label {
        id: labelHeader
        multiline: true
        text: "default header"
        textStyle.fontSize: FontSize.Large
        //Boldify the header text when the body is expanded
        textStyle.fontWeight: body.visible ? FontWeight.Bold : FontWeight.Normal
    }
    Container {
        id: body
        visible: false
        TextArea {
            id: bodyText
            text: "default body"
            textStyle.fontStyle: FontStyle.Italic
            editable: false
            input.submitKey: SubmitKey.Done
        }
        Button {
            id: answerButton
            onClicked: {
                answered();
            }
            horizontalAlignment: HorizontalAlignment.Right
        }
    }
    Divider {

    }

    //If the user selects the answerButton then we hide the body
	onAnswered: {
        bodyVisible = false;
    }

	//If this control is tapped anywhere it should toggle the body visibility
    gestureHandlers: TapHandler {
        onTapped: {
            if (body.visible) {
                body.visible = false;
            } else {
                body.visible = true;
            }
        }
    }
}