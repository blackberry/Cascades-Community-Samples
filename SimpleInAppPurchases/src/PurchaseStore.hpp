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

#ifndef PURCHASESTORE_HPP_
#define PURCHASESTORE_HPP_

#include <QObject>
#include <QSettings>
#include <Qt/qdeclarativedebug.h>

/**
 * This class handles the caching and retrieval of purchases to and from QSettings.
 * There is no interaction with the Payment Service from this class, it just stores
 * and retrieves QString values representing the SKUs of digital goods purchased.
 */

class PurchaseStore: public QObject {
	Q_OBJECT
public:
	PurchaseStore(QObject* parent = 0);
	virtual ~PurchaseStore();

public Q_SLOTS:
	void deletePurchaseRecords();
	void storePurchase(const QString& digitalGoodSku);
	void retrieveLocalPurchases();

	Q_SIGNALS:
		void purchaseRecordsDeleted();
		void purchaseRetrieved(const QString& digitalGoodSku);

private:
	// This sample stores all purchases in local persistent store to make goods readily
	//	accessible even without network access or relying on the cache.
	QSettings m_digitalGoodsStore;
};
#endif /* PURCHASESTORE_HPP_ */
