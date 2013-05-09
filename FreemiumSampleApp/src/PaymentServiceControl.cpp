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

#include "PaymentServiceControl.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/Window>
#include <bb/platform/CancelSubscriptionReply>
#include <bb/platform/ExistingPurchasesReply>
#include <bb/platform/PriceReply>
#include <bb/platform/PurchaseReply>
#include <bb/platform/SubscriptionStatusReply>
#include <bb/platform/SubscriptionTermsReply>

#include <QDateTime>

using namespace bb::platform;

/**
 * Format the receipt into a user readable string.
 */
static QString receiptToString(bb::platform::PurchaseReceipt r) {
	const QString initialPeriod = QString::number(r.initialPeriod());
	const QDateTime startDate = r.startDate();
	const QString startDateStr =
			startDate.isNull() ? "N/A" : startDate.toString();
	const QDateTime endDate = r.endDate();
	const QString endDateStr = endDate.isNull() ? "N/A" : endDate.toString();
	const QString isSubscr = r.isSubscription() ? "true" : "false";
	const QString itemStateStr = QString::number(static_cast<int>(r.state()));

	const QString displayString = "Date: " + r.date().toString() + "\nID/SKU: "
			+ r.digitalGoodId() + "/" + r.digitalGoodSku()
			+ "\nPurchaseID/licenseKey: " + r.purchaseId() + "/"
			+ r.licenseKey() + "\nMetadata: " + r.purchaseMetadata()
			+ "\nItemState/isSubscription?: " + itemStateStr + "/" + isSubscr
			+ "\nStart/End: " + startDateStr + "/" + endDateStr
			+ "\nInitialPeriod: " + initialPeriod + "\n";

	return displayString;
}

PaymentServiceControl::PaymentServiceControl(QObject *parent) :
		QObject(parent), m_paymentManager(new PaymentManager(this)), m_isPaymentSystemBusy(
				false) {
	// Get the window group ID and pass it to the PaymentService instance.
	const QString windowGroupId =
			bb::cascades::Application::instance()->mainWindow()->groupId();
	m_paymentManager->setWindowGroupId(windowGroupId);
}

PaymentServiceControl::~PaymentServiceControl() {
}

/**
 * Request the purchase from the payment service based on the item's id, sku, name and metadata.
 */
void PaymentServiceControl::purchase(const QString & sku, const QString &name,
		const QString &metadata) {
	setIsPaymentSystemBusy(true);
	if (sku.isEmpty()) {
		emit infoResponseError(-1, "No SKU provided");
		setIsPaymentSystemBusy(false);
		return;
	}

	qDebug() << "\nRequesting purchase. SKU:" << sku << "Name:" << name
			<< "Metadata:" << metadata;

	const PurchaseReply *reply = m_paymentManager->requestPurchase(NULL, sku,
			name, metadata);
	connect(reply, SIGNAL(finished()), SLOT(purchaseResponse()));
}

/**
 * Invoked in response to the purchase signal. It differentiates between successful and
 * error responses and emits appropriate signal for each.
 */
void PaymentServiceControl::purchaseResponse() {
	bb::platform::PurchaseReply *reply = qobject_cast<
			bb::platform::PurchaseReply*>(sender());
	Q_ASSERT(reply);

	if (reply->isError()) {
		//emits error signal upon errors.
		qDebug() << "Purchase response error. Code(" << reply->errorCode()
				<< ") Text(" << reply->errorText() << ")";
		emit infoResponseError(reply->errorCode(), reply->errorText());

	} else {
		//emits success signal upon success
		qDebug() << "Purchase response success. "
				<< receiptToString(reply->receipt());
		QStringList listOfDigitalGoodSkus = m_digitalGoodsStore.value(
				"digitalGoods", QStringList()).toStringList();
		listOfDigitalGoodSkus.append(reply->digitalGoodSku());
		m_digitalGoodsStore.setValue("digitalGoods", listOfDigitalGoodSkus);
		emit skuOwned(reply->digitalGoodSku());
	}

	reply->deleteLater();
	setIsPaymentSystemBusy(false);
}

/**
 * Request existing purchases from the payment service.
 */
void PaymentServiceControl::getExisting(bool refresh) {
	setIsPaymentSystemBusy(true);
	qDebug() << "Get existing. refresh: " << refresh;

	if (!refresh) {
		if (m_digitalGoodsStore.contains("digitalGoods")) {
			QStringList listOfDigitalGoodSkus = m_digitalGoodsStore.value(
					"digitalGoods").toStringList();
			if (!listOfDigitalGoodSkus.isEmpty()) {
				foreach(QString digitalGoodSku, listOfDigitalGoodSkus){
				emit skuOwned(digitalGoodSku);
			}
			setIsPaymentSystemBusy(false);
			return;
		}
	}
}

	const ExistingPurchasesReply *reply =
			m_paymentManager->requestExistingPurchases(refresh);
	connect(reply, SIGNAL(finished()), SLOT(existingPurchasesResponse()));
}

/**
 * Invoked in response to retrieve existing purchases made and emit appropriate signal based
 * on the response data.
 */
void PaymentServiceControl::existingPurchasesResponse() {
	bb::platform::ExistingPurchasesReply *reply = qobject_cast<
			bb::platform::ExistingPurchasesReply*>(sender());
	Q_ASSERT(reply);

	//emits error signal upon errors.
	if (reply->isError()) {
		qDebug() << "Existing purchases response error. Code("
				<< reply->errorCode() << ") Text(" << reply->errorText() << ")";
		emit infoResponseError(reply->errorCode(), reply->errorText());

		//emits success signal upon success
	} else {
		qDebug() << "Existing purchases response success. (TODO)";
		const QList<PurchaseReceipt> receipts = reply->purchases();
		QStringList listOfDigitalGoodSkus = m_digitalGoodsStore.value(
				"digitalGoods", QStringList()).toStringList();

		foreach(PurchaseReceipt receipt, receipts){
		if(!listOfDigitalGoodSkus.contains(receipt.digitalGoodSku())) {
			listOfDigitalGoodSkus.append(receipt.digitalGoodSku());
			m_digitalGoodsStore.setValue("digitalGoods", listOfDigitalGoodSkus);
		}
		emit skuOwned(receipt.digitalGoodSku());
	}
}

	reply->deleteLater();
	setIsPaymentSystemBusy(false);
}

/**
 * Query the payment service for an item's price based on its ID and SKU.
 */
void PaymentServiceControl::getPrice(const QString & sku) {
	setIsPaymentSystemBusy(true);
	if (sku.isEmpty()) {
		emit infoResponseError(-1, "No SKU provided");
		setIsPaymentSystemBusy(false);
		return;
	}

	qDebug() << "Requesting price. SKU: " << sku;

	//Make the price request and indicate what method to invoke on signal response.
	const PriceReply *reply = m_paymentManager->requestPrice(NULL, sku);
	connect(reply, SIGNAL(finished()), SLOT(priceResponse()));
}

/**
 * Invoked in response to price request for an item. Emit appropriate signal based on response data.
 */
void PaymentServiceControl::priceResponse() {
	bb::platform::PriceReply *reply = qobject_cast<bb::platform::PriceReply*>(
			sender());
	Q_ASSERT(reply);

//emits error signal upon errors.
	if (reply->isError()) {
		qDebug() << "Price response error. Code(" << reply->errorCode()
				<< ") Text(" << reply->errorText() << ")";
		emit infoResponseError(reply->errorCode(), reply->errorText());

		//emits success signal upon success
	} else {
		qDebug() << "Price response success. Price: " << reply->price();
		//Emit success response if no errors occurred.
		emit priceResponseSuccess(reply->digitalGoodSku(), reply->price());
	}

	reply->deleteLater();
	setIsPaymentSystemBusy(false);
}

/** Clear out all records of purchases. This is meant to only be used for testing
 *  purposes and should be removed when moving to the release builds.
 */
void PaymentServiceControl::deletePurchaseRecords() {
	m_digitalGoodsStore.setValue("digitalGoods", QStringList());
	emit purchaseRecordsDeleted();
}

