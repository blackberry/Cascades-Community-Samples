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

 Container {
       layout: DockLayout {}
       
       //Refer to StatusBarLabel.qml for more info on this
       StatusBarLabel { id: label0 }
       StatusBarLabel { id: label1 }
       
       //This section performs an animation on the StatusBarLabel when the label switches
       function setText(text) {
           var incomingLabel = null
           var outgoingLabel = null
           if (label1.opacity == 0) {
               incomingLabel = label1
               outgoingLabel = label0
           } else {
               incomingLabel = label0
               outgoingLabel = label1
           }
           if (outgoingLabel.text !== text) {
	           incomingLabel.text = text;
	           incomingLabel.opacity = 1;
	           incomingLabel.translationY = 0
	           incomingLabel.scaleX = 1
	           incomingLabel.scaleY = 1
	           outgoingLabel.opacity = 0
	           outgoingLabel.translationY = 200
	           outgoingLabel.scaleX = .1
	           outgoingLabel.scaleY = .1
           }
       }
   }