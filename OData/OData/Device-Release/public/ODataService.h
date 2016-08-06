/*
 * ODataService.h
 *
 *  Created on: 2013-03-29
 *      Author: Daniel Baxter
 */

#ifndef ODATASERVICE_H_
#define ODATASERVICE_H_

#include <qobject.h>
#include <qvariant.h>

class ODataService : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString source READ getSource WRITE setSource NOTIFY sourceChanged)
public:
    ODataService();
    ODataService(QString source);
    virtual ~ODataService();

    QString getSource();
    void setSource(QString newSource);

    Q_INVOKABLE QVariant getServiceDefinition();
    Q_INVOKABLE QVariant getMetadata();

signals:
    void sourceChanged();
    void definitionReady();
    void metadataReady();

public slots:
    void jsonDefinitionReadComplete(QVariant response);
    void xmlDefinitionReadComplete(QVariant response);
    void metadataReadComplete(QVariant response);

private:
    QString mSource;
    QVariant mServiceDefinition;
    QVariant mMetadata;

    void loadData();
};

#endif /* ODATASERVICE_H_ */
