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
import bb.platform 1.2

/**
 * This page represents the storefront. All Digital Goods sold within the 
 * app can be seen and purchased here. As well this page will let users 
 * know what they have purchased and provide a means to manually restore
 * purchases from the BlackBerry World servers should it be needed: ex
 * if the user switches to a new device or has the app re-installed. 
 */

Page {

    id: storePage

    attachedObjects: [
        // The PaymentServiceControl property is defined here to allow explicit
        // setting of the variable over referencing the PaymentServiceControl of
        // the parent class.
        PaymentManager {
            id: storePaymentManager
            property bool busy: false
            onExistingPurchasesFinished: {
                storePaymentManager.busy = false;
                
                //If there's no error message in the response...
                if (reply.errorCode == 0) {
                    //...save the purchase to local cache
                    for (var i = 0; i < reply.purchases.length; ++ i) {
                        PurchaseStore.storePurchase(reply.purchases[i]["digitalGoodSku"]);
                    }
                } else {
                    console.log("Error: " + reply.errorInfo);
                }
            }
        }
    ]

    actionBarVisibility: ChromeVisibility.Hidden
    Container {
        layout: DockLayout {
        }
        ImageView {
            scalingMethod: ScalingMethod.AspectFill
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
            imageSource: "asset:///images/backgroundLandscape.png"
        }

        // An in-app advertisement Banner capable of being deleted.
        // See @FreemiumBanner.qml for more details
        FreemiumBanner {
            scaleX: 2.0
            scaleY: 2.0
            // removeAdsPurchased is the bool defined in main.qml
            hideAd: navigationPane.removeAdsPurchased
            horizontalAlignment: HorizontalAlignment.Center
        }
        Container {
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            Container {
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }

                /**
                 * The following 4 DigitalGood items represent the items available for purchase
                 * within the game. The user starts out with the UFO but needs to purchase the 
                 * remaining items which either augment the UFO or remove advertisements from the 
                 * game. 
                 * 
                 * See @DigitalGood.qml for more info
                 */

                DigitalGood {
                    id: ufo
                    owned: true
                    imageSource: "asset:///images/ufoThumb.png"
                    sku: "1"
                    name: "UFO"
                }
                DigitalGood {
                    id: tbeam
                    imageSource: "asset:///images/tbeamThumb.png"
                    sku: "2"
                    name: "T-Beam"
                    // tbeamPurchased is the bool defined in main.qml
                    onOwnedChanged: navigationPane.tbeamPurchased = owned
                }
                DigitalGood {
                    id: freakinLaserBeam
                    imageSource: "asset:///images/freakinLaserBeam.png"
                    sku: "3"
                    name: "Freakin Laser"
                    // freakinLaserBeamPurchased is the bool defined in main.qml
                    onOwnedChanged: navigationPane.freakinLaserBeamPurchased = owned
                }
                DigitalGood {
                    id: noAds
                    imageSource: "asset:///images/noAds.png"
                    sku: "4"
                    name: "Remove Ads"
                    // removeAdsPurchased is the bool defined in main.qml
                    onOwnedChanged: navigationPane.removeAdsPurchased = owned
                }
            }
        }

        // This button will force a refresh of purchased goods from the BlackBerry
        // World servers. It is manual and should not typically be needed.
        ImageButton {
            verticalAlignment: VerticalAlignment.Bottom
            horizontalAlignment: HorizontalAlignment.Center
            defaultImageSource: "asset:///images/restorePurchases.png"
            preferredWidth: 900
            enabled: ! storePaymentManager.busy
            onClicked: {
                storePaymentManager.busy = true;
                storePaymentManager.requestExistingPurchases(true);
            }
        }

        //This button pops the StorePage from the stack, returning to the previous page
        Container {
            horizontalAlignment: HorizontalAlignment.Left
            verticalAlignment: VerticalAlignment.Bottom
            leftPadding: 25
            bottomPadding: 25
            ImageButton {
                defaultImageSource: "asset:///images/ic_previous.png"
                onClicked: storePage.parent.pop()
            }
        }

        // This button removes all purchase records from local storage. Used for test
        // purposes only.
        Container {
            horizontalAlignment: HorizontalAlignment.Right
            verticalAlignment: VerticalAlignment.Bottom
            rightPadding: 25
            bottomPadding: 25
            ImageButton {
                defaultImageSource: "asset:///images/ic_delete.png"
                onClicked: PurchaseStore.deletePurchaseRecords()
            }
        }
    }
}
