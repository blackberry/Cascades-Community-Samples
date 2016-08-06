/*
 * ODataListModel.h
 *
 *  Created on: 2013-03-26
 *      Author: Daniel Baxter
 */

#ifndef ODATALISTMODEL_H_
#define ODATALISTMODEL_H_

#include <qobject.h>
#include <qvariant.h>
#include <bb/cascades/DataModel>

class ODataListModel : public bb::cascades::DataModel {
    Q_OBJECT

    Q_PROPERTY(QString source READ getSource WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(int pageSize READ getPageSize WRITE setPageSize NOTIFY pageSizeChanged)
public:
    ODataListModel();
    virtual ~ODataListModel();

    QString getSource();
    void setSource(QString newSource);

    int getPageSize();
    void setPageSize(int newSize);

    Q_INVOKABLE QVariant data(const QVariantList& indexPath);
    Q_INVOKABLE int childCount(const QVariantList& indexPath);
    Q_INVOKABLE bool hasChildren(const QVariantList& indexPath);
    Q_INVOKABLE void refresh();

signals:
    void sourceChanged();
    void pageSizeChanged();
    void modelError(int code, QString message);

public slots:
    void jsonCompleted(QVariant response);
    void atomCompleted(QVariant response);

    void error(int code, QString message);

private:
    bool initialized;
    QVariantList mDataList;
    QString mSource;
    int mPageSize;

    void loadData();
};

#endif /* ODATALISTMODEL_H_ */
