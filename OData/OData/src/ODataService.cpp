/*
 * ODataService.cpp
 *
 *  Created on: 2013-03-29
 *      Author: Daniel Baxter
 */

#include "ODataService.h"
#include "ODataConstants.h"

#include "ODataNetworkManager.h"

/*
 * CONSTRUCTORS / DESCTRUCTORS
 */

ODataService::ODataService() {
}

ODataService::ODataService(QString source) {
    setSource(source);
}

ODataService::~ODataService() {
}

/*
 * PROPERTIES
 */

QString ODataService::getSource(){
    return mSource;
}
void ODataService::setSource(QString newSource){
    mSource = newSource;

    mServiceDefinition.clear();
    mMetadata.clear();
    loadData();

    emit sourceChanged();
}


/*
 * FUNCTIONS
 */

void ODataService::loadData(){
    ODataNetworkManager* manager = new ODataNetworkManager();
    manager->read(mSource);

    connect(manager, SIGNAL(jsonReady(QVariant)), this, SLOT(jsonDefinitionReadComplete(QVariant)));
    connect(manager, SIGNAL(atomReady(QVariant)), this, SLOT(xmlDefinitionReadComplete(QVariant)));
    connect(manager, SIGNAL(xmlReady(QVariant)), this, SLOT(xmlDefinitionReadComplete(QVariant)));

    // we want to connect this to a different SLOT so we need a new one
    ODataNetworkManager* metaManager = new ODataNetworkManager();
    metaManager->read(mSource + "/" + METADATA);

    connect(metaManager, SIGNAL(atomReady(QVariant)), this, SLOT(metadataReadComplete(QVariant)));
    connect(metaManager, SIGNAL(xmlReady(QVariant)), this, SLOT(metadataReadComplete(QVariant)));
}

QVariant ODataService::getServiceDefinition() {
    return mServiceDefinition;
}

QVariant ODataService::getMetadata() {
    return mMetadata;
}

/*
 * SLOTS
 */

// for now lets just store the values we get back

void ODataService::jsonDefinitionReadComplete(QVariant response){
    mServiceDefinition = response.toMap()["d"];
    emit definitionReady();
}

void ODataService::xmlDefinitionReadComplete(QVariant response){
    mServiceDefinition = response.toMap()[WORKSPACE];
    emit definitionReady();
}

void ODataService::metadataReadComplete(QVariant response){
    mMetadata = response.toMap()[EDMX_DATASERVICES].toMap()[SCHEMA];

    emit metadataReady();
}
