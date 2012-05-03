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
import ReboundsGraph 1.0
import MinutesPerGameGraph 1.0
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
		        initialize();
		    }

		    function initialize()
		    {
                minsOn.opacity = 1;
                reboundsOn.opacity = 0;

                axesLeft.visible = false;
                axesRight.visible = false;
                scaleLeft.visible = false;
                scaleRight.visible = false;

                dualMinutesPerGameGraph.visible = true;

                dualMinutesPerGameGraph.initGLRenderer();
		    }
		    function switchToMinutes()
		    {
                minsOn.opacity = 1;
                reboundsOn.opacity = 0;

                axesLeft.visible = false;
                axesRight.visible = false;
                scaleLeft.visible = false;
                scaleRight.visible = false;

                dualMinutesPerGameGraph.visible = true;
                
                // Team A
                
                teamAPlayer1Container.showMinutes();
                teamAPlayer1Container.layoutProperties.positionX = teamAPlayer1Container.getReboundsX();
                teamAPlayer1Container.layoutProperties.positionY = teamAPlayer1Container.getReboundsY();

                teamAPlayer2Container.showMinutes();
                teamAPlayer2Container.layoutProperties.positionX = teamAPlayer2Container.getReboundsX();
                teamAPlayer2Container.layoutProperties.positionY = teamAPlayer2Container.getReboundsY();

                teamAPlayer3Container.showMinutes();
                teamAPlayer3Container.layoutProperties.positionX = teamAPlayer3Container.getReboundsX();
                teamAPlayer3Container.layoutProperties.positionY = teamAPlayer3Container.getReboundsY();

                teamAPlayer4Container.showMinutes();
                teamAPlayer4Container.layoutProperties.positionX = teamAPlayer4Container.getReboundsX();
                teamAPlayer4Container.layoutProperties.positionY = teamAPlayer4Container.getReboundsY();

                teamAPlayer5Container.showMinutes();
                teamAPlayer5Container.layoutProperties.positionX = teamAPlayer5Container.getReboundsX();
                teamAPlayer5Container.layoutProperties.positionY = teamAPlayer5Container.getReboundsY();
                
                // TeamB

                teamBPlayer1Container.showMinutes();
                teamBPlayer1Container.layoutProperties.positionX = teamBPlayer1Container.getReboundsX();
                teamBPlayer1Container.layoutProperties.positionY = teamBPlayer1Container.getReboundsY();

                teamBPlayer2Container.showMinutes();
                teamBPlayer2Container.layoutProperties.positionX = teamBPlayer2Container.getReboundsX();
                teamBPlayer2Container.layoutProperties.positionY = teamBPlayer2Container.getReboundsY();

                teamBPlayer3Container.showMinutes();
                teamBPlayer3Container.layoutProperties.positionX = teamBPlayer3Container.getReboundsX();
                teamBPlayer3Container.layoutProperties.positionY = teamBPlayer3Container.getReboundsY();

                teamBPlayer4Container.showMinutes();
                teamBPlayer4Container.layoutProperties.positionX = teamBPlayer4Container.getReboundsX();
                teamBPlayer4Container.layoutProperties.positionY = teamBPlayer4Container.getReboundsY();

                teamBPlayer5Container.showMinutes();
                teamBPlayer5Container.layoutProperties.positionX = teamBPlayer5Container.getReboundsX();
                teamBPlayer5Container.layoutProperties.positionY = teamBPlayer5Container.getReboundsY();
                
                reboundsGraph.stopGLRenderer();
                dualMinutesPerGameGraph.initGLRenderer();
		    }
		    function switchToRebounds()
		    {
                minsOn.opacity = 0;
                reboundsOn.opacity = 1;
                
                axesLeft.visible = true;
                axesRight.visible = true;
                scaleLeft.visible = true;
                scaleRight.visible = true;
                
                dualMinutesPerGameGraph.visible = false;
                
                teamAPlayer1Container.saveLocation(teamAPlayer1Container.layoutProperties.positionX, teamAPlayer1Container.layoutProperties.positionY);
                teamAPlayer2Container.saveLocation(teamAPlayer2Container.layoutProperties.positionX, teamAPlayer2Container.layoutProperties.positionY);
                teamAPlayer3Container.saveLocation(teamAPlayer3Container.layoutProperties.positionX, teamAPlayer3Container.layoutProperties.positionY);
                teamAPlayer4Container.saveLocation(teamAPlayer4Container.layoutProperties.positionX, teamAPlayer4Container.layoutProperties.positionY);
                teamAPlayer5Container.saveLocation(teamAPlayer5Container.layoutProperties.positionX, teamAPlayer5Container.layoutProperties.positionY);

                teamBPlayer1Container.saveLocation(teamBPlayer1Container.layoutProperties.positionX, teamBPlayer1Container.layoutProperties.positionY);
                teamBPlayer2Container.saveLocation(teamBPlayer2Container.layoutProperties.positionX, teamBPlayer2Container.layoutProperties.positionY);
                teamBPlayer3Container.saveLocation(teamBPlayer3Container.layoutProperties.positionX, teamBPlayer3Container.layoutProperties.positionY);
                teamBPlayer4Container.saveLocation(teamBPlayer4Container.layoutProperties.positionX, teamBPlayer4Container.layoutProperties.positionY);
                teamBPlayer5Container.saveLocation(teamBPlayer5Container.layoutProperties.positionX, teamBPlayer5Container.layoutProperties.positionY);
                
                
                // Team A
                
                teamAPlayer1Container.hideMinutes();
                teamAPlayer1Container.layoutProperties.positionX = 1150;
                teamAPlayer1Container.layoutProperties.positionY = 172;
                
                teamAPlayer2Container.hideMinutes();
                teamAPlayer2Container.layoutProperties.positionX = 1150;
                teamAPlayer2Container.layoutProperties.positionY = 242;
                
                teamAPlayer3Container.hideMinutes();
                teamAPlayer3Container.layoutProperties.positionX = 1150;
                teamAPlayer3Container.layoutProperties.positionY = 312;
                
                teamAPlayer4Container.hideMinutes();
                teamAPlayer4Container.layoutProperties.positionX = 1150;
                teamAPlayer4Container.layoutProperties.positionY = 382;
                
                teamAPlayer5Container.hideMinutes();
                teamAPlayer5Container.layoutProperties.positionX = 1150;
                teamAPlayer5Container.layoutProperties.positionY = 452;
                
                // Team B
                
                teamBPlayer1Container.hideMinutes();
                teamBPlayer1Container.layoutProperties.positionX = -190;
                teamBPlayer1Container.layoutProperties.positionY = 172;
                
                teamBPlayer2Container.hideMinutes();
                teamBPlayer2Container.layoutProperties.positionX = -190;
                teamBPlayer2Container.layoutProperties.positionY = 242;
                
                teamBPlayer3Container.hideMinutes();
                teamBPlayer3Container.layoutProperties.positionX = -190;
                teamBPlayer3Container.layoutProperties.positionY = 312;
                
                teamBPlayer4Container.hideMinutes();
                teamBPlayer4Container.layoutProperties.positionX = -190;
                teamBPlayer4Container.layoutProperties.positionY = 382;
                
                teamBPlayer5Container.hideMinutes();
                teamBPlayer5Container.layoutProperties.positionX = -190;
                teamBPlayer5Container.layoutProperties.positionY = 452;
                
                dualMinutesPerGameGraph.stopGLRenderer();
                reboundsGraph.initGLRenderer();
		    }


            ReboundsGraph {
                objectName: "reboundsGraph"
                id: reboundsGraph
                preferredWidth: 1280
                preferredHeight: 1280
                layoutProperties: AbsoluteLayoutProperties {
                    positionX: 0
                    positionY: 0                      
                }	                
            }
            MinutesPerGameGraph 
            {
                id: dualMinutesPerGameGraph
                preferredWidth: 700
                preferredHeight: 700
            }
            
		    ImageView {
                id: axesLeft
                imageSource: "asset:///graphics/l_axis.png"
                preferredWidth: 446
                preferredHeight: 477
                layoutProperties: AbsoluteLayoutProperties {
                    positionX: 86
                    positionY: 156                      
                }		        
		    }
		    ScaleLeft {
		        id: scaleLeft
                layoutProperties: AbsoluteLayoutProperties {
                    positionX: 124
                    positionY: 540                      
                }		        
		    }
		    ImageView {
                id: axesRight
                imageSource: "asset:///graphics/r_axis.png"
                preferredWidth: 441
                preferredHeight: 384
                layoutProperties: AbsoluteLayoutProperties {
                    positionX: 731
                    positionY: 156                      
                }		        
		    }
		    ScaleRight {
		        id: scaleRight
                layoutProperties: AbsoluteLayoutProperties {
                    positionX: 720
                    positionY: 540                      
                }		        
		    }
		    ImageView {
                id: reboundsOff
                imageSource: "asset:///graphics/rebounds_off.png"
                preferredWidth: 557
                preferredHeight: 96
                layoutProperties: AbsoluteLayoutProperties {
                    positionX: 0
                    positionY: 672                      
                }		        
		    }		    
		    ImageView {
                id: reboundsOn
                imageSource: "asset:///graphics/rebounds_on.png"
                preferredWidth: 557
                preferredHeight: 96
                layoutProperties: AbsoluteLayoutProperties {
                    positionX: 0
                    positionY: 672                      
                }
                onTouch: {
                    if (event.isUp()) {
                        if (reboundsOn.opacity == 0)
                        {
                            root.switchToRebounds();
                        }
                    }
                }		        
                		        
		    }		    
		    ImageView {
                id: minsOff
                imageSource: "asset:///graphics/mins_off.png"
                preferredWidth: 752
                preferredHeight: 96
                layoutProperties: AbsoluteLayoutProperties {
                    positionX: 528
                    positionY: 672                      
                }		        
		    }		    
		    ImageView {
                id: minsOn
                imageSource: "asset:///graphics/mins_on.png"
                preferredWidth: 752
                preferredHeight: 96
                opacity: 0
                layoutProperties: AbsoluteLayoutProperties {
                    positionX: 528
                    positionY: 672                      
                }
                onTouch: {
                    if (event.isUp()) {
                        if (minsOn.opacity == 0)
                        {
                            root.switchToMinutes();
                        }
                    }
                }		        
		    }
            onTouch: {
                if (event.isDown())
                {
                    Utils.startTouch(event.screenX, event.screenY);
                }
                else if (event.isUp()) {
                    Utils.endTouch();
                }
                else if (event.isMove()) {
                    Utils.tapped = false;
                    Utils.moveTouch(event.screenX, event.screenY);
                        

                    if (Utils.isSwipeRight())
                    {
                        if(Utils.rightSwiped == true)
                        {
                        }
                        else
                        {
                            Utils.rightSwiped = true;
                            cs.goBack();
                        }
                    }
                }
            }
	        TeamAPlayerStats
	        {
	            objectName: "teamAPlayer1Container"
	            id: teamAPlayer1Container
	            layoutProperties: AbsoluteLayoutProperties 
	            {
	                positionX : 1
	                positionY : 500
	            }
	        }
	
	        TeamAPlayerStats 
	        {
	            objectName: "teamAPlayer2Container"
	            id: teamAPlayer2Container
	            layoutProperties: AbsoluteLayoutProperties 
	            {
	                positionX : 1
	                positionY : 500
	            }
	        }
	
	        TeamAPlayerStats
	        {
	            objectName: "teamAPlayer3Container"
	            id: teamAPlayer3Container
	            layoutProperties: AbsoluteLayoutProperties 
	            {
	                positionX : 1
	                positionY : 500
	            }
	        }
	
	        TeamAPlayerStats 
	        {
	            objectName: "teamAPlayer4Container"
	            id: teamAPlayer4Container
	            layoutProperties: AbsoluteLayoutProperties 
	            {
	                positionX : 1
	                positionY : 500
	            }
	        }
	
	        TeamAPlayerStats 
	        {
	            objectName: "teamAPlayer5Container"
	            id: teamAPlayer5Container
	            layoutProperties: AbsoluteLayoutProperties 
	            {
	                positionX : 1
	                positionY : 500
	            }
	        }
	
	        //team 2
	        TeamBPlayerStats
	        {
	            objectName: "teamBPlayer1Container"
	            id: teamBPlayer1Container
	            layoutProperties: AbsoluteLayoutProperties 
	            {
	                positionX : 700
	                positionY : 500
	            }
	        }
	
	        TeamBPlayerStats 
	        {
	            objectName: "teamBPlayer2Container"
	            id: teamBPlayer2Container
	            layoutProperties: AbsoluteLayoutProperties 
	            {
	                positionX : 700
	                positionY : 500
	            }
	        }
	
	        TeamBPlayerStats
	        {
	            objectName: "teamBPlayer3Container"
	            id: teamBPlayer3Container
	            layoutProperties: AbsoluteLayoutProperties 
	            {
	                positionX : 700
	                positionY : 500
	            }
	        }
	
	        TeamBPlayerStats 
	        {
	            objectName: "teamBPlayer4Container"
	            id: teamBPlayer4Container
	            layoutProperties: AbsoluteLayoutProperties 
	            {
	                positionX : 700
	                positionY : 500
	            }
	        }
	
	        TeamBPlayerStats 
	        {
	            objectName: "teamBPlayer5Container"
	            id: teamBPlayer5Container
	            layoutProperties: AbsoluteLayoutProperties 
	            {
	                positionX : 700
	                positionY : 500
	            }
	        }	

	        Label
	        {
	            objectName: "teamAName"
	            id: teamAName
	            preferredWidth: 500
	            layoutProperties: AbsoluteLayoutProperties 
	            {
	                positionX : 780
	                positionY : 64
	            }
	            text: "Team A"
	            textStyle.base: SystemDefaults.TextStyles.BigText
	            textStyle.color:Color.create("#FFFFFF");
	            textStyle.alignment: TextAlignment.ForceLeft
	        }

	        Label
	        {
	            objectName: "teamBName"
	            id: teamBName
	            preferredWidth: 500
	            layoutProperties: AbsoluteLayoutProperties 
	            {
	                positionX : 0
	                positionY : 64
	            }
	            text: "Team B"
	            textStyle.base: SystemDefaults.TextStyles.BigText
	            textStyle.color:Color.create("#FFFFFF");
	            textStyle.alignment: TextAlignment.ForceRight
	        }

		}    
}
