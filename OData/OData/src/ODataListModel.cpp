/*
 * ODataListModel.cpp
 *
 *  Created on: 2013-03-26
 *      Author: Daniel Baxter
 */

#include "ODataListModel.h"
#include "ODataNetworkManager.h"
#include <bb/cascades/DataModelChangeType>

/*
 * CONSTRUCTORS / DESCTRUCTORS
 */

ODataListModel::ODataListModel() {
    initialized = false;
}

ODataListModel::~ODataListModel() {
}

/*
 * PROPERTIES
 */

QString ODataListModel::getSource(){
    return mSource;
}
void ODataListModel::setSource(QString newSource){
    mSource = newSource;

    if (initialized) {
        // when we get a new url source we should load that data and toss the current data
        mDataList.clear();
        emit itemsChanged(bb::cascades::DataModelChangeType::Init);

        loadData();
    }

    emit sourceChanged();
}

int ODataListModel::getPageSize(){
    return mPageSize;
}
void ODataListModel::setPageSize(int newSize){
    mPageSize = newSize;
    emit pageSizeChanged();
}

/*
 * FUNCTIONS
 */

QVariant ODataListModel::data(const QVariantList& indexPath){
    return mDataList[indexPath[0].toInt()].toMap();
}

int ODataListModel::childCount(const QVariantList& indexPath){
    // the empty list is the root.
    // when it requests the root we should go get the data
    if (indexPath.count() == 0 && !initialized) {
        loadData();
    }

    // we are a single level data source so we can just return the amount of items we own
    return mDataList.count();
}

bool ODataListModel::hasChildren(const QVariantList& indexPath){
    // we aren't multilevel so we are only concerned with the root having children
    return indexPath.count() == 0 && !mDataList.isEmpty();
}

void ODataListModel::refresh() {
    // when we get a new url source we should load that data and toss the current data
    mDataList.clear();
    emit itemsChanged(bb::cascades::DataModelChangeType::Init);

    loadData();
}

void ODataListModel::loadData(){
    // once we have called loadData at least once we are initialized
    initialized = true;

    if (mSource.isEmpty()) {
        return;
    }

    ODataNetworkManager* manager = new ODataNetworkManager();
    manager->read(mSource);

    connect(manager, SIGNAL(jsonReady(QVariant)), this, SLOT(jsonCompleted(QVariant)));
    connect(manager, SIGNAL(atomReady(QVariant)), this, SLOT(atomCompleted(QVariant)));
    connect(manager, SIGNAL(networkError(int, QString)), this, SLOT(error(int, QString)));
}

/*
 * SLOTS
 */

void ODataListModel::jsonCompleted(QVariant response) {
    mDataList = response.toMap()["d"].toList();

    // handle the case where there was only 1 item in the list
    if (mDataList.count() == 0) {
        mDataList.append(response.toMap()["d"]);
    }

    emit itemsChanged(bb::cascades::DataModelChangeType::Init);
}


void ODataListModel::atomCompleted(QVariant response) {
    mDataList = response.toMap()["entry"].toList();

    // handle the case where there was only 1 item in the list
    if (mDataList.count() == 0) {
        mDataList.append(response.toMap()["entry"]);
    }

    emit itemsChanged(bb::cascades::DataModelChangeType::Init);
}

void ODataListModel::error(int code, QString message) {
    emit modelError(code, message);
}
