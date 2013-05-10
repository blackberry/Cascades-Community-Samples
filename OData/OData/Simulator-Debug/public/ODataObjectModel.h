/*
 * ODataObjectModel.h
 *
 *  Created on: 2013-03-28
 *      Author: Daniel Baxter
 */

#ifndef ODATAOBJECTMODEL_H_
#define ODATAOBJECTMODEL_H_

#include <qobject.h>
#include <qvariant.h>

class ODataObjectModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString source READ getSource WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QVariant model READ getModel WRITE setModel NOTIFY modelChanged)

public:
    ODataObjectModel();
    ODataObjectModel(QString source);
    virtual ~ODataObjectModel();

    QString getSource();
    void setSource(QString newSource);

    QVariant getModel();
    void setModel(QVariant newModel);

    Q_INVOKABLE void refreshModel();

    Q_INVOKABLE QVariantList getModelStructure(QString modelKey, QVariant metadata);

    Q_INVOKABLE void createModel(QString postUrl, QString category, QVariant content, QByteArray links);
    Q_INVOKABLE void updateModel(QString putUrl, QString category, QVariant content, QByteArray links);

    Q_INVOKABLE void deleteModel();

signals:
    void modelReady();
    void sourceChanged();
    void modelChanged();

    void modelDeleted();
    void modelCreated();
    void modelUpdated();

    void modelError(int code, QString message);
public slots:
    void jsonReadComplete(QVariant response);
    void atomReadComplete(QVariant response);

    void deleteComplete();
    void createComplete();
    void updateComplete();

    void error(int code, QString message);
private:
    QString mSource;
    QVariant mModel;

    QVariantList mStructure; // the data structure of the object (used for creates and updates)

    void loadModel();
    QByteArray parseContent(QVariant content);
    QString createOpenTag(QString tag);
    QString createCloseTag(QString tag);
    QString createNullTag(QString tag);
};

#endif /* ODATAOBJECTMODEL_H_ */
