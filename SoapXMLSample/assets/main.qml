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
import "controls"
import wsf.cdyne.com.WeatherWS 1.0

/**
*  This is a simple Page layout that allows you to select a city from a DropDown
*  menu and click a button to get the current temperature for that city
*
*  You will learn how to:
*  -- Use both DockLayout and StackLayout
*  -- Use a DropDown menu
*  -- Execute different code for a Button onClicked based on the selected item in a DropDown menu
*  -- Use a custom control to notify when there is network activity
*  -- Use a C++ object's members
*  -- Use Javascript to control visibility of different parts of the UI
*  -- Style text using textStyle property of a Label
**/

Page {
    content: Container {
        id: mainContainer
        background : Color.Black
        layout: DockLayout {
        }
        Container {
            layout: StackLayout {
                layoutDirection : LayoutDirection.TopToBottom
            }
            layoutProperties : DockLayoutProperties {
                verticalAlignment : VerticalAlignment.Top
                horizontalAlignment : HorizontalAlignment.Center
            }
            Label {
                text: "Select a city:"
                textStyle {
                    color: Color.White
                }
            }
            Container {
                background : Color.White
	            DropDown {
	                id: cityDropDown
	                title : "City"
	                enabled : true                
	                Option {
	                    id : beverlyHillsOption
	                    text : "Beverly Hills"
	                    value : "90210"
	                    description : "Beverly Hills, California"
	                    selected : true
	                }
	                Option {
	                    id : detroitOption
	                    text : "Detroit"
	                    value : "48201"
	                    description : "Detroit, Michigan"
	                }
	                Option {
	                    id : pittsburghOption
	                    text : "Pittsburgh"
	                    value : "15203"
	                    description : "City of Pittsburgh (South Side), Pennsylvania"
	                }
	                Option {
	                    id : miamiOption
	                    text : "Miami"
	                    value : "33126"
	                    description : "Miami, Florida"
	                }
	                Option {
	                    id : mordorOption
	                    text : "Mordor"
	                    value : "331261"
	                    description : "One does not simply walk into Mordor"
	                }
	                onSelectedIndexChanged: {
	                    mainContainer.resetContainers();
	                }
	            }
            }
            Button {
                id: weatherButton
                layoutProperties : StackLayoutProperties {
                    horizontalAlignment : HorizontalAlignment.Center
                }
                text: "Get Weather"
                onClicked: {
                    weatherButton.enabled = false;
                    mainContainer.showProgressContainer();
                    
                    //dropdown control does not expose a "getSelectedValue" (yet)
                    switch(cityDropDown.selectedIndex) {
                        case 0:
                            weatherservice.getWeather(beverlyHillsOption.value);
                            break;
                        case 1:
                            weatherservice.getWeather(detroitOption.value);
                            break;
                        case 2:
                            weatherservice.getWeather(pittsburghOption.value);
                            break;
                        case 3:
                            weatherservice.getWeather(miamiOption.value);
                            break;
                        case 4:
                            weatherservice.getWeather(mordorOption.value);
                            break;    
                        default:
                            //nothing happened so..
                            mainContainer.resetContainers();
                            weatherButton.enabled = true;
                    }
                }
            }
            NetworkActivity {
                id : progressIndicator
                visible : false
                layoutProperties : StackLayoutProperties {
                    horizontalAlignment : HorizontalAlignment.Center
                }
            }
            Container {
                id: weatherContainer
                layoutProperties : StackLayoutProperties {
                    horizontalAlignment : HorizontalAlignment.Center
                }
                Label {
                    id : weatherdescriptionLabel
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                        alignment: TextAlignment.Center
                    }
                }
                Label {
                    id : temperatureLabel
                    textStyle {
                        size : 130
                        color: Color.create ("#ffFDfDfD")
                        alignment: TextAlignment.Center	                
                    }
                }
            }
            Container {
                id: errorContainer
                visible : false
                layoutProperties : StackLayoutProperties {
                    horizontalAlignment : HorizontalAlignment.Center
                }
                TextArea {
                    id : errorLabel
                    editable : false
                    backgroundVisible : false
                    textStyle {
                        base: SystemDefaults.TextStyles.SmallText
                        color: Color.create ("#ffFDfDfD")
                        alignment: TextAlignment.Center	                
                    }
                }
            }
        }
        attachedObjects: [
            WeatherService {
                id : weatherservice
                onComplete :{
                    weatherButton.enabled = true;
                    // display the weather
                    if(weatherservice.succeeded)
                    {
                        mainContainer.showWeatherContainer();
                        weatherdescriptionLabel.text = weatherservice.description
                        temperatureLabel.text = weatherservice.temperature
                    }
                    else
                    {
                        mainContainer.showErrorContainer();
                        //can create a separate container for errors but we'll use
                        //the temperature label in this simple app
                        errorLabel.text = weatherservice.error
                    }
                }
            }
        ]
        function resetContainers()
        {
            weatherdescriptionLabel.text = "";
            temperatureLabel.text = "";
            errorContainer.visible = false;
            weatherContainer.visible = false;
            progressIndicator.stopIndicator();
            progressIndicator.visible = false;
        }
        function showErrorContainer()
        {
            errorContainer.visible = true;
            weatherContainer.visible = false;
            progressIndicator.stopIndicator();
            progressIndicator.visible = false;
        }
        function showWeatherContainer()
        {
            errorContainer.visible = false;
            weatherContainer.visible = true;
            progressIndicator.stopIndicator();
            progressIndicator.visible = false;
        }
        function showProgressContainer()
        {
            errorContainer.visible = false;
            weatherContainer.visible = false;
            progressIndicator.startIndicator();
            progressIndicator.visible = true;
        }
    }
}
