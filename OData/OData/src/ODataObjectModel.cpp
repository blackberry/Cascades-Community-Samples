/*
 * ODataObjectModel.cpp
 *
 *  Created on: 2013-03-28
 *      Author: Daniel Baxter
 */

#include "ODataObjectModel.h"
#include "ODataConstants.h"

#include "ODataNetworkManager.h"

#include "stdio.h"

/*
 * CONSTRUCTORS / DESCTRUCTORS
 */

ODataObjectModel::ODataObjectModel() {
}

ODataObjectModel::ODataObjectModel(QString source) {
    setSource(source);
}

ODataObjectModel::~ODataObjectModel() {
}

/*
 * PROPERTIES
 */

QString ODataObjectModel::getSource(){
    return mSource;
}
void ODataObjectModel::setSource(QString newSource){
    mSource = newSource;

    mModel.clear();
    loadModel();

    emit sourceChanged();
}

QVariant ODataObjectModel::getModel(){
    return mModel;
}

void ODataObjectModel::setModel(QVariant newModel){
    mModel = newModel;

    emit modelChanged();
}

/*
 * FUNCTIONS
 */

void ODataObjectModel::loadModel(){
    ODataNetworkManager* manager = new ODataNetworkManager();
    manager->read(mSource);

    connect(manager, SIGNAL(jsonReady(QVariant)), this, SLOT(jsonReadComplete(QVariant)));
    connect(manager, SIGNAL(atomReady(QVariant)), this, SLOT(atomReadComplete(QVariant)));
    connect(manager, SIGNAL(networkError(int, QString)), this, SLOT(error(int, QString)));
}

void ODataObjectModel::refreshModel() {
    loadModel();
}

void ODataObjectModel::deleteModel() {
    ODataNetworkManager* manager = new ODataNetworkManager();
    manager->del(mSource);

    connect(manager, SIGNAL(deleteSuccessful()), this, SLOT(deleteComplete()));
    connect(manager, SIGNAL(networkError(int, QString)), this, SLOT(error(int, QString)));
}

QVariantList ODataObjectModel::getModelStructure(QString modelKey, QVariant metadata) {
    mStructure = QVariantList();

    QVariantMap metaEntity;

    // Grab the entities from the Meta Data
    QVariantList entities = metadata.toMap()[ENTITY_TYPE].toList();

    // toList won't list things that only occur once so we handle this case here
    if (entities.count() == 0) {
        entities.append(metadata.toMap()[ENTITY_TYPE]);
    }

    // Get the complex data types (if any)
    QVariantList complexTypes = metadata.toMap()[COMPLEX_TYPE].toList();

    if (complexTypes.count() == 0) {
        complexTypes.append(metadata.toMap()[COMPLEX_TYPE]);
    }

    // find the correct entity
    for (int i = 0; i < entities.count(); i++) {
        if (entities[i].toMap()[NAME].toString().compare(modelKey) == 0) {
            metaEntity = entities[i].toMap();
            break;
        }
    }

    // parse out the correct properties and add them to the mStructure Variant
    if (metaEntity.count() >= 1) {
        /*
         * Handle the fields.
         */
        entities = metaEntity[PROPERTY].toList();

        if (entities.count() == 0) {
            entities.append(metaEntity[PROPERTY]);
        }


        for (int i = 0; i < entities.count(); i++) {
            QVariantMap entityData;

            entityData[NAME] = entities[i].toMap()[NAME];
            entityData[TYPE] = entities[i].toMap()[TYPE];
            entityData[NULLABLE] = entities[i].toMap()[NULLABLE];
            entityData[DATA] = "";

            mStructure.append(entityData);

            // handle complex data types, ie not Edm.<type>
            if (entityData[TYPE].toString().indexOf(EDM) != 0) {
                QVariantMap complexType;

                for (int j = 0; j < complexTypes.count(); j++) {
                    if (entityData[TYPE].toString().contains(complexTypes[j].toMap()[NAME].toString())) {
                        complexType = complexTypes[j].toMap();
                        break;
                    }
                }

                if (!complexType.isEmpty()) {
                    QVariantList typeFields = complexType[PROPERTY].toList();

                    if (typeFields.count() == 0) {
                        typeFields.append(complexType[PROPERTY]);
                    }

                    for (int j = 0; j < typeFields.count(); j++) {
                        entityData[NAME] = typeFields[j].toMap()[NAME];
                        entityData[TYPE] = typeFields[j].toMap()[TYPE];
                        entityData[NULLABLE] = typeFields[j].toMap()[NULLABLE];
                        entityData[DATA] = "";

                        mStructure.append(entityData);
                    }

                    // add an end tag so we know we got to the end
                    entityData[NAME] = entities[i].toMap()[NAME];
                    entityData[TYPE] = END_COMPLEX_TYPE;
                    entityData[NULLABLE] = false;
                    entityData[DATA] = "";

                    mStructure.append(entityData);
                }
            }
        }

        /*
         * Handle the links to other items
         */
        entities = metaEntity[NAVIGATION_PROPERTY].toList();

        if (entities.count() == 0) {
            entities.append(metaEntity[NAVIGATION_PROPERTY]);
        }

        for (int i = 0; i < entities.count(); i++) {
            QVariantMap entityData;

            entityData[NAME] = entities[i].toMap()[NAME];
            entityData[TYPE] = NAVIGATION_PROPERTY;
            entityData[DATA] = "";

            mStructure.append(entityData);
        }
    }

    return mStructure;
}

void ODataObjectModel::createModel(QString postUrl, QString category, QVariant content, QByteArray links){
    QByteArray model;

    model.append(XML_TAG);
    model.append(ENTRY_OPEN);

    model.append(CATEGORY_OPEN);
    model.append(category);
    model.append(CATEGORY_CLOSE);

    model.append(links);

    model.append(CONTENT_OPEN);
    model.append(parseContent(content));
    model.append(CONTENT_CLOSE);

    model.append(ENTRY_CLOSE);


    ODataNetworkManager* manager = new ODataNetworkManager();
    manager->create(postUrl, model);

    connect(manager, SIGNAL(createSuccessful()), this, SLOT(createComplete()));
    connect(manager, SIGNAL(networkError(int, QString)), this, SLOT(error(int, QString)));
}

void ODataObjectModel::updateModel(QString putUrl, QString category, QVariant content, QByteArray links){
    QByteArray model;

    model.append(XML_TAG);
    model.append(ENTRY_OPEN);

    model.append(CATEGORY_OPEN);
    model.append(category);
    model.append(CATEGORY_CLOSE);

    model.append(links);

    model.append(CONTENT_OPEN);
    model.append(parseContent(content));
    model.append(CONTENT_CLOSE);

    model.append(ENTRY_CLOSE);

    ODataNetworkManager* manager = new ODataNetworkManager();
    manager->update(putUrl, model);

    connect(manager, SIGNAL(updateSuccessful()), this, SLOT(updateComplete()));
    connect(manager, SIGNAL(networkError(int, QString)), this, SLOT(error(int, QString)));
}


QByteArray ODataObjectModel::parseContent(QVariant content) {
    QByteArray parsedContent;

    QVariantList contentList = content.toList();

    for (int i = 0; i < content.toList().count(); i++) {
        QVariantMap dataMap = contentList[i].toMap();

        // now we ignore the types of links and preprocess everything else
        if (!dataMap[TYPE].toString().contains(NAVIGATION_PROPERTY)) {
            if (dataMap[TYPE].toString().indexOf(EDM) != 0) {
                if (dataMap[TYPE].toString().compare(END_COMPLEX_TYPE) == 0) { // close tag
                    parsedContent.append(createCloseTag(dataMap[NAME].toString()));
                }
                else { // open tag
                    parsedContent.append(createOpenTag(dataMap[NAME].toString()
                            .append(QString("m:type='"))
                            .append(dataMap[TYPE].toString())
                            .append(QString("'"))
                        ));
                }
            }
            else {
                if (dataMap[DATA].toString().isEmpty()) {
                    parsedContent.append(createNullTag(dataMap[NAME].toString()));
                }
                else {
                    parsedContent.append(createOpenTag(dataMap[NAME].toString()));
                    parsedContent.append(dataMap[DATA].toString());
                    parsedContent.append(createCloseTag(dataMap[NAME].toString()));
                }
            }
        }
    }

    return parsedContent;
}

QString ODataObjectModel::createOpenTag(QString tag) {
    return tag.prepend("<d:").append(">");
}

QString ODataObjectModel::createCloseTag(QString tag) {
    return tag.prepend("</d:").append(">");
}

QString ODataObjectModel::createNullTag(QString tag) {
    return tag.prepend("<d:").append(" m:null='true' />");
}

/*
 * SLOTS
 */

void ODataObjectModel::jsonReadComplete(QVariant response){
    mModel = response.toMap()["d"];

    emit modelReady();
}

void ODataObjectModel::atomReadComplete(QVariant response){
    mModel = response;

    emit modelReady();
}

void ODataObjectModel::deleteComplete() {
    emit modelDeleted();
}

void ODataObjectModel::createComplete() {
    emit modelCreated();
}

void ODataObjectModel::updateComplete() {
    emit modelUpdated();
}

void ODataObjectModel::error(int code, QString message) {
    emit modelError(code, message);
}
