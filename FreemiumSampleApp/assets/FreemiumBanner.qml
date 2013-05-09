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
import bb.cascades.advertisement 1.0

/**
 * Rather than relying solely on users purchasing items within the app we
 * also monetize by displaying advertisements throughout the app in areas
 * that are eye catching but do not impeded the usability of the game. If
 * users would prefer not to have ads then they have the option to remove
 * them through purchase of a Digital Good in the @StoreFront page
 */

Container {
    property bool hideAd: false
    id: bannerContainer
    
    // If the user pays to remove ads we need to make sure the ads get deleted from
    // screen immediately.
    onHideAdChanged: {
        if (hideAd) {
            bannerContainer.maxHeight = 0;
            bannerContainer.maxWidth = 0;
        }
    }
    onCreationCompleted: {
        // Only create the ad if the user has not paid to have them removed
        if (! bannerContainer.hideAd) {
            var bannerControl = adBanner.createObject();
            bannerContainer.insert(-1,bannerControl);
        }
    }
    attachedObjects: [
        ComponentDefinition {
            id: adBanner
            Banner {
                //Test Zone ID, replace with your own before publishing or you won't get paid
                zoneId: 117145 
                refreshRate: 60
                preferredWidth: 320
                preferredHeight: 50
            }
        }
    ]
}
