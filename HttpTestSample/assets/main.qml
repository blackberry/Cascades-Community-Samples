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

/**
*  This is a TabbedPane layout that allows you to switch 3 NavigationPanes (GET, POST, Settings)
*
*  You will learn how to:
*  -- Create a TabbedPane
*  -- Create Tabs
*  -- Add NavigationPane to a Tab 
*  -- Use both DockLayout and StackLayout
*  -- Style text using textStyle property of a Label
*  -- Push a new layout on a NavigationPane by pressing a button
*  -- Use a RadioGroup to set a flag in a C++ object
**/
TabbedPane {
    Tab {
        title : "GET"
	    NavigationPane {
	        id : httpGetNavPane
	        Page {
	            id : getpage
	            content: Container {
	                background : Color.DarkRed
	                layout : DockLayout {   
	                }                 
	                Container {
	                    background : Color.Black
	                    layout : StackLayout {
	                    }
	                    layoutProperties : DockLayoutProperties {
	                        verticalAlignment : VerticalAlignment.Top
	                        horizontalAlignment : HorizontalAlignment.Fill
	                    }
	                    Label {
	                        layoutProperties : StackLayoutProperties {
	                            horizontalAlignment : HorizontalAlignment.Center
	                        }
	                    textStyle { 
	                        base: SystemDefaults.TextStyles.BigText; 
	                        color: Color.White
	                        alignment : TextAlignment.Center
	                        }           
	                        text : "GET REQUESTS"
	                        textStyle.fontStyle: FontStyle.Italic
	                        textStyle.fontWeight: FontWeight.Bold
	                    }
	                }
	                Container {
	                    layout : StackLayout {
	                        layoutDirection : LayoutDirection.TopToButtom
	                    }
	                    layoutProperties : DockLayoutProperties {
	                        verticalAlignment : VerticalAlignment.Center
	                        horizontalAlignment : HorizontalAlignment.Fill
	                    }
	                    Button {
	                        layoutProperties : StackLayoutProperties {
	                            horizontalAlignment : HorizontalAlignment.Center
	                        }
	                        id : getIpButton
	                        text : "External IP"
	                        onClicked: {
	                            httpGetNavPane.deprecatedPushQmlByString("ipinfo.qml");
	                        }
	                    }
	                    Button {
	                        layoutProperties : StackLayoutProperties {
	                            horizontalAlignment : HorizontalAlignment.Center
	                        }
	                        id : getHeadersButton
	                        text : "Get Request Headers"
	                        onClicked: {
	                            httpGetNavPane.deprecatedPushQmlByString("requestinfo.qml");
	                        }
	                    }
	                }
	            }
	        }
	    }
    }
    Tab {
        title : "POST"
	    NavigationPane {
	        id : httpPostNavPane
	        Page {
	            content: Container {
	                 background : Color.DarkBlue
	                 layout : DockLayout {   
	                 }                 
	                Container {
	                    background : Color.Black
	                    layout : StackLayout {
	                    }
	                    layoutProperties : DockLayoutProperties {
	                        verticalAlignment : VerticalAlignment.Top
	                        horizontalAlignment : HorizontalAlignment.Fill
	                    }
	                    Label {
	                        layoutProperties : StackLayoutProperties {
	                            horizontalAlignment : HorizontalAlignment.Center
	                        }
	                    textStyle { 
	                        base: SystemDefaults.TextStyles.BigText; 
	                        color: Color.White
	                        alignment : TextAlignment.Center
	                        }           
	                        text : "POST REQUESTS"
	                        textStyle.fontStyle: FontStyle.Italic
	                        textStyle.fontWeight: FontWeight.Bold
	                    }
	                }
	                Container {
	                    layout : StackLayout {
	                        layoutDirection : LayoutDirection.TopToBottom
	                    }
	                    layoutProperties : DockLayoutProperties {
	                        verticalAlignment : VerticalAlignment.Center
	                    }
	                    Label {
	                        text : "Enter post body:"
	                    }
	                    TextArea {
	                        id: postbody
	                        preferredHeight : 350
	                        text : ""
	                    }
	                    Button {
	                        layoutProperties : StackLayoutProperties {
	                            horizontalAlignment : HorizontalAlignment.Center
	                        }
	                        id : postButton
	                        text : "Post!"
	                        onClicked: {
	                            httpsample.setPostBody(postbody.text);
	                            httpPostNavPane.deprecatedPushQmlByString("post.qml");
	                        }
	                    }
	                }
	            }
	        }
	    }
    }
    Tab {
        title : "Settings"
	    NavigationPane {
	        id : settingsGetNavPane
	        Page {
	            id : settingspage
	            content: Container {
	                background : Color.DarkGray
	                layout : DockLayout {   
	                }
	                Container {
	                    background : Color.Black
	                    layout : StackLayout {
	                    }
	                    layoutProperties : DockLayoutProperties {
	                        verticalAlignment : VerticalAlignment.Top
	                        horizontalAlignment : HorizontalAlignment.Fill
	                    }
	                    Label {
	                        layoutProperties : StackLayoutProperties {
	                            horizontalAlignment : HorizontalAlignment.Center
	                        }
	                    textStyle { 
	                        base: SystemDefaults.TextStyles.BigText; 
	                        color: Color.White
	                        alignment : TextAlignment.Center
	                        }           
	                        text : "CHANGE SETTINGS"
	                        textStyle.fontStyle: FontStyle.Italic
	                        textStyle.fontWeight: FontWeight.Bold
	                    }
	                }
	                Container {
	                    preferredWidth : 500
	                    layoutProperties : DockLayoutProperties {
	                        verticalAlignment : VerticalAlignment.Center
	                        horizontalAlignment : HorizontalAlignment.Center
	                    }
	                    layout : StackLayout {
	                    }
	                    RadioGroup {
	                        objectName : "securitySettingGroup"
	                        id : securitySettingGroup
	                        layoutProperties : StackLayoutProperties {
	                            
	                        }
	                        Option {
	                            objectName : "httpOption"
	                            id : httpOption
	                            text : "HTTP (normal)"
	                            onCreationCompleted: {
	                                selected = httpsample.isUsingHttp();
	                            }
	                            onSelectedChanged: {
	                                if(selected == true) {
	                                    httpsample.useHttps(false);
	                                }
	                            }
	                        }
	                        Option {
	                            objectName : "httpsOption"
	                            id : httpsOption
	                            text : "HTTPS (secure)"
	                            onCreationCompleted: {
	                                selected = httpsample.isUsingHttps();
	                            }
	                            onSelectedChanged: {
	                                if(selected == true) {
	                                    httpsample.useHttps(true);
	                                }
	                            }
	                        }
	                    }
	                }
	            }
	        }
	    }                 
    }
}    