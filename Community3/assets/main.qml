/* Copyright (c) 2014 BlackBerry Ltd.
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
 * 
 * The Lithium REST APIs are owned by Lithium Technologies, Inc. 
 * (http://www.lithium.com) and are not part of the “Works” licensed 
 * hereunder pursuant to the Apache 2.0 license.
 */

import bb.cascades 1.2
import com.msohm.EULA 1.0
import com.msohm.ApplicationSettings 1.0

TabbedPane {
    id: mainTabbedPane
    Menu.definition: MenuDefinition {
        // Add a Help action
        helpAction: HelpActionItem {
            onTriggered: {
                var hlpSheet = helpSheet.createObject();
                hlpSheet.open();
            }
        }

        // Add a Settings action
        settingsAction: SettingsActionItem {
            onTriggered: {
                var setSheet = settingsSheet.createObject();
                setSheet.open();
            }
        }

        // Add any remaining actions
        actions: [
            ActionItem {
                title: qsTr("Invite To Download")
                enabled: bbmHandler.allowed
                imageSource: "asset:///images/ic_bbm.png"
                onTriggered: {
                    bbmHandler.sendInvite();
                }
            }
        
        ]
    }
    showTabsOnActionBar: false
	Tab { //Inside BB Blog tab
	    // Localized text with the dynamic translation and locale updates support
        id: blogTab  //ToDo: Ensure this is a unique name if you add multiple tabs.
	    title: qsTr("Your Blog") + Retranslate.onLocaleOrLanguageChanged  //ToDo: Enter your blog name.
	    delegate: Delegate {
	        id: blogTabDelegate
	        source: "blackBerryBlog.qml"
	        onObjectChanged: {
	            if (blogTabDelegate.object != null)
	            {
                    blogTabDelegate.object.blogURL = "http://blogs.blackberry.com/"  //ToDo:  Enter your blog URL.
             	}
         }
	    }
	    delegateActivationPolicy: TabDelegateActivationPolicy.Default
        imageSource: "asset:///images/BlogIcon.png"
    } //End of Inside BB tab

    Tab { //Developer Forums tab
        title: qsTr("Your Forums") + Retranslate.onLocaleOrLanguageChanged  //ToDo:  Enter your forum name
        id: forumTab  //ToDo: Ensure this is a unique name if you add multiple tabs.
        delegate: Delegate {
            id: developerForumsDelegate
            source: "forumCategoryList.qml"
            onObjectChanged: {
                if (developerForumsDelegate.object != null)
                {
                    developerForumsDelegate.object.top.categoryId = "blackberrydev";  //ToDo: Enter your forum uber category.
                    developerForumsDelegate.object.top.pageTitle = "Your Forums";  //ToDo:  Enter your forum name
                    developerForumsDelegate.object.top.requestContent();
                }
            }
        }
        delegateActivationPolicy: TabDelegateActivationPolicy.Default
        imageSource: "asset:///images/ic_feedback.png"
    } //End of Developer Forums tab
    Tab { //BBM Channels tab
        title: qsTr("BBM Channels") + Retranslate.onLocaleOrLanguageChanged
        delegate: Delegate {
            id: bbmChannelsDelegate
            source: "bbmChannels.qml"
        }
        delegateActivationPolicy: TabDelegateActivationPolicy.Default
        imageSource: "asset:///images/ChannelsIcon.png"
    } //End of BBM Channels tab    
    
    attachedObjects: [
	    ComponentDefinition
	    {
	        id: settingsSheet
	        source: "settingsSheet.qml"
	    },
	    ComponentDefinition {
        	id: helpSheet
        	source: "helpSheet.qml"
    	},
        ComponentDefinition {
            id: eulaSheet
            source: "eulaSheet.qml"
        },
	    EULA {
	        id: eula
	    },
        ApplicationSettings {
            id: appSettings
        }
    ]
    
    onCreationCompleted: {
        if (!eula.agreedTo)
        {
            var eulash = eulaSheet.createObject();
            eulash.open();
        }
        
        mainTabbedPane.activeTab = mainTabbedPane.at(appSettings.getDefaultTab());
    }
}
