/*
 * Copyright (c) 2011-2012 Research In Motion Limited.
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
import TwitterStreamGraph 1.0
import "Utils.js" as Utils
import "controls"

Page {
    content: Container {
            id: root
            layout: AbsoluteLayout {}
            background: Color.create("#000000");
            preferredWidth: 1280
		    onCreationCompleted: {
		        Utils.reset();
		    }
            
			TwitterStreamGraph {
                objectName: "twitterStreamGraph"
                id: twitterStreamGraph
                layoutProperties: AbsoluteLayoutProperties {
                    positionX: 0
                    positionY: 0
                }
				preferredWidth: 1280
				preferredHeight: 1280
			}      
            TeamRing {
                id: teams
                layoutProperties: AbsoluteLayoutProperties {
                    positionX: -1024
                    positionY: -640
                }
                rotationZ: -22.5
                
                onTouch: {
                    if (event.isDown())
                    {
                        Utils.startTouch(event.screenX, event.screenY);
                    }
                    else if (event.isUp()) {
                        if (Utils.isTap())
                        {
                            Utils.tapped = true;
                        }
                        
                        Utils.endTouch();
                        if (Utils.tapped == true)
                        {
                        	cs.goForward("stats.qml");
                        }
                    }
                    else if (event.isMove()) {
                        Utils.tapped = false;
                        Utils.moveTouch(event.screenX, event.screenY);
                        
                        if (Utils.isSwipeUp())
                        {
                            rotationZ -= 45;
                        	cs.selectTeamsFromRotation(rotationZ);
                            
                            if (Utils.matchUp())
                            {
                                conferences.rotationZ -= 180;
                            }
                            
                            Utils.endTouch();
                            Utils.startTouch(event.screenX, event.screenY);
                        }
                        else if (Utils.isSwipeDown())
                        {
                            rotationZ += 45;
                        	cs.selectTeamsFromRotation(rotationZ);

                            if (Utils.matchDown())
                            {
                                conferences.rotationZ += 180;
                            }

                            Utils.endTouch();
                            Utils.startTouch(event.screenX, event.screenY);
                        }
                    }
                }
            }
            ConferenceRing{
                id: conferences
                layoutProperties: AbsoluteLayoutProperties {
                    positionX: -469
                    positionY: -85
                }                
            }
            ImageView {
                id: title
                imageSource: "asset:///graphics/nba.png"
                preferredWidth: 284
                preferredHeight: 231
                layoutProperties: AbsoluteLayoutProperties {
                    positionX: 10
                    positionY: 268                      
                }
            }
     }
}
