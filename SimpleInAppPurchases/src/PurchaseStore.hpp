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
