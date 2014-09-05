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

#include "PurchaseStore.hpp"

#include <QStringList>

PurchaseStore::PurchaseStore(QObject* parent): QObject(parent) {
}

PurchaseStore::~PurchaseStore() {
}

/**
 * Save the digital good to the store if it has not already been purchased.
 */
void PurchaseStore::storePurchase(const QString& digitalGoodSku) {
	QStringList listOfDigitalGoodSkus = m_digitalGoodsStore.value(
			"digitalGoods", QStringList()).toStringList();
	if (!listOfDigitalGoodSkus.contains(digitalGoodSku)) {
		listOfDigitalGoodSkus.append(digitalGoodSku);
		m_digitalGoodsStore.setValue("digitalGoods", listOfDigitalGoodSkus);
	}
	emit purchaseRetrieved(digitalGoodSku);
}

/**
 * Grab all purchases from cache and emit a signal for each. The signal
 * makes it very easy for our app to listen for each good found.
 */
void PurchaseStore::retrieveLocalPurchases() {
	QStringList listOfDigitalGoodSkus = m_digitalGoodsStore.value(
			"digitalGoods").toStringList();
	if (!listOfDigitalGoodSkus.isEmpty()) {
	foreach(QString digitalGoodSku, listOfDigitalGoodSkus) {
		emit purchaseRetrieved(digitalGoodSku);
	}
}
}

/** Clear out all records of purchases. This is meant to only be used for testing
 *  purposes and should be removed when moving to the release builds.
 */
void PurchaseStore::deletePurchaseRecords() {
	m_digitalGoodsStore.setValue("digitalGoods", QStringList());
	emit purchaseRecordsDeleted();
}
