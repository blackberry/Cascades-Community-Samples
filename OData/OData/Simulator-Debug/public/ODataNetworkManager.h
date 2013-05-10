/*
 * ODataNetworkManager.h
 *
 *  Created on: 2013-03-26
 *      Author: Daniel Baxter
 */

#ifndef ODATANETWORKMANAGER_H_
#define ODATANETWORKMANAGER_H_

#include <qobject.h>
#include <qvariant.h>
#include <qnetworkaccessmanager.h>
#include <qnetworkreply.h>

static const QByteArray TYPE_ATOM = "application/atom+xml";
static const QByteArray TYPE_JSON = "application/json";
static const QByteArray TYPE_XML = "application/xml";

class ODataNetworkManager : public QObject {
    Q_OBJECT

public:
    ODataNetworkManager();
    virtual ~ODataNetworkManager();

    void read(QString url);
    void create(QString url, QByteArray dataModel);
    void update(QString url, QByteArray dataModel);
    void del(QString url); // wanted to call delete but it's a protected keyword

private:
    QNetworkAccessManager* mNetAccessManager;

    void handleError(QNetworkReply* reply);
    void handleErrorNoReply();

signals:
    void jsonReady(QVariant response);
    void atomReady(QVariant response);
    void xmlReady(QVariant response);

    void createJsonReady(QVariant response);
    void createAtomReady(QVariant response);
    void createXmlReady(QVariant response);

    void createSuccessful();
    void updateSuccessful();
    void deleteSuccessful();

    void networkError(int code, QString message);
public slots:
    void onReadReply();
    void onCreateReply();
    void onUpdateReply();
    void onDeleteReply();
};

#endif /* ODATANETWORKMANAGER_H_ */
