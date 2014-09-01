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
