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
import bb.platform 1.2

/**
 * This Container represents a DigitalGood that can be sold. The QML file is fairly 
 * portable along with the PaymentServiceControl.cpp/hpp files.
 */

Container {
    attachedObjects: [
        // The PaymentServiceControl property is defined here to allow explicit
        // setting of the variable over referencing the PaymentServiceControl of
        // the parent class.
        PaymentManager {
            id: digitalGoodPaymentManager
            onPurchaseFinished: {
                if (reply.errorCode == 0) {
                    PurchaseStore.storePurchase(reply.digitalGoodSku);
                } else {
                    console.log("Error: " + reply.errorInfo);
                }
            }
        }
    ]

    // Giving this control an ID allows it to be referenced explicitly from
    // methods where context is lost.
    id: digitalGood

    // Basic list of properties exposed for assignment
    property bool owned: false // Is the DigitalGood currently owned?
    property string sku // The SKU of the DigitalGood
    property alias imageSource: digitalGoodImage.imageSource // The accompanying image
    property alias name: digitalGoodName.text // The text displayed under the image

    layout: DockLayout {
    }

    //Payment calls can only be made one at a time, so this line gives us a mutex
    //enabled: ! digitalGoodPaymentManager.isPaymentSystemBusy

    preferredHeight: 200
    preferredWidth: 250
    leftMargin: 25
    rightMargin: 25
    ImageView {
        imageSource: "asset:///images/backgroundFrame.png.amd"
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        opacity: 0.9
    }
    // A small icon representing the digital good
    ImageView {
        id: digitalGoodImage
        scalingMethod: ScalingMethod.AspectFit
        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center
    }
    // Space for the name of the Digital Good to be displayed
    Label {
        id: digitalGoodName
        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Bottom
    }

    // The below container is an overlay displayed atop the digital good
    // if it has not yet been purchased.
    Container {
        layout: DockLayout {
        }
        visible: ! owned
        opacity: 0.3
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill
        ImageView {
            imageSource: "asset:///images/backgroundFrame.png.amd"
            horizontalAlignment: HorizontalAlignment.Fill
            verticalAlignment: VerticalAlignment.Fill
        }
        ImageView {
            //By RRZEicons (Own work) [CC-BY-SA-3.0 (http://creativecommons.org/licenses/by-sa/3.0)], via Wikimedia Commons
            imageSource: "asset:///images/lock.png"
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
        }
    }

    // Handle clicks by the user, normally symbolizing a purchase attempt
    gestureHandlers: TapHandler {
        onTapped: {
            if (digitalGood.owned) {
                // Do something, not needed for this sample
                // May be good to display a message to the user, or bring up details on the
                // digital good
            } else {
                digitalGoodPaymentManager.requestPurchase("", digitalGood.sku, digitalGood.name);
            }
        }
    }

    // Connect the important signals from the PaymentServiceControl to localized
    // functions within the DigitalGood class
    onCreationCompleted: {
        PurchaseStore.purchaseRetrieved.connect(purchaseMade);
        PurchaseStore.purchaseRecordsDeleted.connect(purchaseRecordsDeleted);
    }

    // Checks to see if the recent purchase was for this good
    function purchaseMade(digitalGoodSku) {
        digitalGood.owned |= (digitalGoodSku == digitalGood.sku);
    }

    // Wipes owenership field of this good if requested by the app
    function purchaseRecordsDeleted() {
        digitalGood.owned = false;
    }
}
