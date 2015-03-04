/*
 * Copyright (c) 2014 BlackBerry Limited.
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
//import the bb.platform library so we can use the Payment Manager
import bb.platform 1.2

Page {
    Container {
        //Todo: fill me with QML
        layout: DockLayout {

        }
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        Container {
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center

            /*
             * Below we call digital goods created from the DigitalGood.qml document
             * Obviously, digital goods don't require a UI, but for demonstrative purposes, these have UI
             * Assigned to each digital good must be a sku in the form of a string
             * The sku is a unique identifier used by BlackBerry World to associate a purchase with a product
             * In order to request a purchase from BlackBerry World, a sku is absolutely required!
             */

            DigitalGood {
                id: digitalgood1
                sku: "prodsku0001"
                goodPayMentManager: storePaymentManager
                horizontalAlignment: HorizontalAlignment.Center

            }
            Divider {
            }
            DigitalGood {
                id: digitalgood2
                sku: "prodsku0002"
                goodPayMentManager: storePaymentManager
                horizontalAlignment: HorizontalAlignment.Center

            }
            //The button below clears the local purchase store and is meant to beused during testing only
            Button {
                text: "Delete all Purchases"
                horizontalAlignment: HorizontalAlignment.Center
                onClicked: {
                    onClicked: PurchaseStore.deletePurchaseRecords()
                }
            }
        }
    }
    attachedObjects: [

        /*
         * Below is the Payment Manager which allows BlackBerry device users to initiate the purchase of digital goods from within an application
         * In other words, the PaymentManager is what communicates with BlackBerry World
         * 
         */

        PaymentManager {
            id: storePaymentManager
            property bool busy: false
            onExistingPurchasesFinished: {
                storePaymentManager.busy = false;

                //If there's no error message in the response...
                if (reply.errorCode == 0) {
                    //...save the purchase(s) to local cache
                    for (var i = 0; i < reply.purchases.length; ++ i) {

                        //If an existing purchase is found, the purchase(s) is/are stored locally...
                        PurchaseStore.storePurchase(reply.purchases[i]["digitalGoodSku"]);
                        console.log(reply.purchases[i].receipt["digitalGoodSku"]);
                    }
                } else {
                    //if there is an error, we throw the error to the console
                    console.log("Error: " + reply.errorText);
                }
            }
            onPurchaseFinished: {
                //this signal fires when the porcess of making a purchase is complete
                //If there's no error message in the response...
                if (reply.errorCode == 0) {
                    //...save the purchase to local cache
                    PurchaseStore.storePurchase(reply.digitalGoodSku);
                } else {
                    console.log("Error: " + reply.errorText);
                }
            }
        }
    ]

    onCreationCompleted: {
        //The connection mode is set, in this sample, to 0 which is local/sandboxmode.
        //This must be changed to Production Mode (1) before publishing to BlackBerry World
        storePaymentManager.setConnectionMode(0); // 0=sandbox mode | 1=production mode
        // As soon as the page loads, lets retrieve any past purchases from local store
        PurchaseStore.retrieveLocalPurchases();
        //Below, we also retrieve the BlackBerry World cache to check for existing purchases
        storePaymentManager.requestExistingPurchases(false);
        //See Digitalgood.qml...
    }
}
