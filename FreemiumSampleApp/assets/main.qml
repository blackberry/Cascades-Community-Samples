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

import bb.cascades 1.2
import bb.platform 1.2

NavigationPane {
    id: navigationPane

    // The below 3 bool properties represent the purchase state of the goods
    property bool removeAdsPurchased: false
    property bool tbeamPurchased: false
    property bool freakinLaserBeamPurchased: false

    attachedObjects: [
        PaymentManager {
            id: rootPaymentManager
            onExistingPurchasesFinished: {
                if (reply.errorCode == 0) {
                    for (var i = 0; i < reply.purchases.length; ++ i) {
                        console.log(reply.purchases[i]["digitalGoodSku"]);
                        PurchaseStore.storePurchase(reply.purchases[i]["digitalGoodSku"]);
                    }
                } else {
                    console.log("Error: " + reply.errorText);
                }
            }
        },
        // StorePage is where users will see all Digital Goods for sale and be able
        // to make purchases
        StorePage {
            id: store
        },
        // The GamePlay page is a placeholder for when actual gameplay gets added to the app
        GamePlay {
            id: game
        }
    ]
    Page {
        id: main
        property double scale: 2.0
        Container {
            layout: DockLayout {
            }

            // Display the background image
            ImageView {
                scalingMethod: ScalingMethod.AspectFill
                horizontalAlignment: HorizontalAlignment.Fill
                verticalAlignment: VerticalAlignment.Fill
                imageSource: "asset:///images/backgroundLandscape.png"
            }
            // See @FreemiumBanner.qml. This control disappears if the user purchases
            // a specific digital good.
            FreemiumBanner {
                scaleX: main.scale
                scaleY: main.scale
                hideAd: removeAdsPurchased
                horizontalAlignment: HorizontalAlignment.Center
            }

            /**
             * The next 3 ImageView elements provide a visual representation
             * of the state of the UFO based on what items the user may have
             * purchased. When a new item is purchased the UI updates to 
             * display the new addition to the arsenal.
             */
            ImageView {
                imageSource: "asset:///images/tbeam.png"
                visible: tbeamPurchased
                scaleX: main.scale
                scaleY: main.scale
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                translationY: 130
            }
            ImageView {
                imageSource: "asset:///images/freakinLaserBeam.png"
                visible: freakinLaserBeamPurchased
                scaleX: main.scale
                scaleY: main.scale
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                translationY: 110
                translationX: -2
            }
            ImageView {
                imageSource: "asset:///images/ufo.png"
                scaleX: main.scale
                scaleY: main.scale
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Center
                translationY: -140
            }

            // The two main functions of our app, Play the game or Shop
            // for new features
            Container {
                horizontalAlignment: HorizontalAlignment.Center
                verticalAlignment: VerticalAlignment.Bottom
                bottomPadding: 100
                layout: StackLayout {
                    orientation: LayoutOrientation.LeftToRight
                }
                ImageButton {
                    defaultImageSource: "asset:///images/play.png"
                    onClicked: navigationPane.push(game)
                }
                ImageButton {
                    defaultImageSource: "asset:///images/shop.png"
                    onClicked: navigationPane.push(store)
                }
            }
            // This container provides a button to invite a friend to download the app over BBM
            Container {
                horizontalAlignment: HorizontalAlignment.Right
                verticalAlignment: VerticalAlignment.Bottom
                rightPadding: 25
                bottomPadding: 25
                ImageButton {
                    defaultImageSource: "asset:///images/ic_bbm.png"
                    onClicked: {
                        mainApp.inviteUserToDownloadViaBBM();
                    }
                }
            }
        }
    }
    onCreationCompleted: {
        rootPaymentManager.setConnectionMode(0);
        // As soon as the page loads, lets retrieve any past purchases from local store then cache
        PurchaseStore.retrieveLocalPurchases();
        rootPaymentManager.requestExistingPurchases(false);
    }
}
