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

Container {
    /*
     * Don't get confused here...
     * All we are doing with the goodPayMentManager property is taking the 'storePaymentManager' from main.qml
     * and exposing it as a property here in the Digitalgood.qml.
     * If you reference the main.qml, you will see we assign storePaymentManager to this poroperty
     * This is neccessary in order to communicate with the PaymentManager (in main.qml) from this qml document 
     * 
     */
    property PaymentManager goodPayMentManager

    // The sku string property is used to store the sku of the digital good
    property string sku

    // The owned property indicates whether or not the user currently owns the project
    property bool owned: false

    onOwnedChanged: {
        //Simple color change to correspond with the status of 'owned'
        if (owned == true) {
            background = Color.create("#8000ffff")
        } else {
            background = Color.DarkRed
        }
    }

    background: Color.DarkRed
    topPadding: 12
    bottomPadding: 12
    leftPadding: 12
    rightPadding: 12

    Label {
        text: "Hi! I'm a digital good!"
    }
    Button {
        text: "Buy me"
        enabled: ! owned
        onClicked: {
            // When this button is clicked, we initiate the transaction, and pass the sku (assigned in main.qml)
            goodPayMentManager.requestPurchase("", sku, "", "", "")
        }
    }

    Label {
        // visual indication of the good's owned status
        text: "Owned: " + owned
    }

    /*
     * VERY IMPORTANT -- this funtion is connected to a signal emitted by the PurchaseStore.cpp
     * The signal (purchaseRetrieved) is emitted anytime the sku property for the digital good matches a sku
     * in the locall purchase store. So, if the sku matches a digital good, that digital good's 'owned'
     * property changes to true indicating the digital good is owned by the user
     */
    function purchaseMade(digitalGoodSku) {
        owned |= (digitalGoodSku == sku);
    }
    //This is used generally only when testing and is tied to a signal indicating that the local purchase store has been cleared
    function purchaseRecordsDeleted() {
        owned = false;
    }

    onCreationCompleted: {
        //This is where the purchaseRetrieved signal is connected to the purchaseMade() JS funtion above
        PurchaseStore.purchaseRetrieved.connect(purchaseMade);
        PurchaseStore.purchaseRecordsDeleted.connect(purchaseRecordsDeleted);
    }
}
