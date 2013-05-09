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

#ifndef PAYMENTSERVICECONTROL_HPP
#define PAYMENTSERVICECONTROL_HPP

#include <bb/platform/PaymentManager>
#include <bb/platform/PurchaseReceipt>
#include <bb/cascades/GroupDataModel>

#include <QObject>
#include <QSharedPointer>

/**
 * PaymentServiceControl handles the various methods related to payment. Such as pricing, subscriptions, terms
 * and purchases queries. As well as the buying and canceling of subscriptions.
 * This class has been designed such that it can be exposed to QML and have methods and properties accessible
 * without needing to switch to the C++ context.
 */
//! [0]
class PaymentServiceControl : public QObject
{
    Q_OBJECT

    // The Payment Service currently only supports one call at a time, this property lets us monitor the state
    // to ensure only one call at a time occurs.
    Q_PROPERTY(bool isPaymentSystemBusy READ isPaymentSystemBusy NOTIFY isPaymentSystemBusyChanged);
public:
    PaymentServiceControl(QObject *parent = 0);
    virtual ~PaymentServiceControl();

    //invokable purchase method from the qml control
    Q_INVOKABLE void purchase(const QString & sku, const QString &name, const QString &metadata=QString());

    //invokable purchases query from the qml control
    Q_INVOKABLE void getExisting(bool refresh);

    //invokable price query from the qml control
    Q_INVOKABLE void getPrice(const QString & sku);

public Q_SLOTS:
    //This method is called whenever a purchase is invoked
    void purchaseResponse();

    //This method is called whenever a purchases query is invoked
    void existingPurchasesResponse();

    //This method is called whenever a price request is made
    void priceResponse();

    void deletePurchaseRecords();

Q_SIGNALS:
    //This signal is emited upon successful purchase
    void skuOwned(const QString & sku);

    //This signal is emitted upon successful price query
    void priceResponseSuccess(const QString & sku, const QString &price);

    //This signal is emitted whenever any of the payment service requests generated an error
    void infoResponseError(int errorCode, const QString &errorText);

    void isPaymentSystemBusyChanged(bool isBusy);
    void purchaseRecordsDeleted();


private:
    bb::platform::PaymentManager *m_paymentManager;

    // This sample stores all purchases in local persistent store to make goods readily
    //	accessible even without network access or relying on the cache.
    QSettings m_digitalGoodsStore;

    bool m_isPaymentSystemBusy;
    bool isPaymentSystemBusy(){
    	return m_isPaymentSystemBusy;
    }
    void setIsPaymentSystemBusy(bool isBusy){
    	m_isPaymentSystemBusy = isBusy;
    	emit isPaymentSystemBusyChanged(isBusy);
    }
};
//! [0]

#endif
