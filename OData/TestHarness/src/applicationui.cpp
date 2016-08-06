// Default empty project template
#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <ODataListModel.h>
#include <ODataObjectModel.h>
#include <ODataConstants.h>

using namespace bb::cascades;

ApplicationUI::ApplicationUI(bb::cascades::Application *app)
: QObject(app)
{
    qmlRegisterType<ODataListModel>("odata", 1, 0, "ODataListModel");
    qmlRegisterType<ODataObjectModel>("odata", 1, 0, "ODataObjectModel");
    // create scene document from main.qml asset
    // set parent to created document to ensure it exists for the whole application lifetime
    QmlDocument* qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("_controller", this);

    /*******************************************************************
     * Place your read/write url here in place of the read only one
     *
     * Access the following url to get a read/write sandbox and copy it in below
     *
     * http://services.odata.org/(S(readwrite))/OData/OData.svc/
     *
     *                                    VVVVVVVVVVVVVVVVVVVVVVVVVVVVV
     *******************************************************************/
    _dataService = new ODataService("http://services.odata.org/(S(bjfyisldyghu2py0bznjakvk))/OData/OData.svc/");
    qml->setContextProperty("dataService", _dataService);

    // create root object for the UI
    AbstractPane* root = qml->createRootObject<AbstractPane>();
    // set created root object as a scene
    app->setScene(root);
}

void ApplicationUI::createProduct(QVariant model) {
    QByteArray links;

    for (int i = 0; i < model.toList().count(); i++) {
        // now we parse out the types of links and preprocess them
        if (model.toList()[i].toMap()[TYPE].toString().compare(NAVIGATION_PROPERTY) == 0) { // link
            QString link = QString(LINK_TEMPLATE);

            if (model.toList()[i].toMap()[NAME].toString().compare("Category") == 0) {
                link.replace("{{schema}}", "http://schemas.microsoft.com/ado/2007/08/dataservices/related/Category");
                link.replace("{{title}}", "Category");
            }
            else if (model.toList()[i].toMap()[NAME].toString().compare("Supplier") == 0) {
                link.replace("{{schema}}", "http://schemas.microsoft.com/ado/2007/08/dataservices/related/Supplier");
                link.replace("{{title}}", "Supplier");
            }

            link.replace("{{link}}", model.toList()[i].toMap()[DATA].toString());

            links.append(link);
        }
    }

    ODataObjectModel* createModel = new ODataObjectModel();
    createModel->createModel(_dataService->getSource() + "Products", "ODataDemo.Product", model, links);

    connect(createModel, SIGNAL(modelCreated()), this, SLOT(createComplete()));
}

void ApplicationUI::updateProduct(QString itemUrl, QVariant model) {
    QByteArray links;

    for (int i = 0; i < model.toList().count(); i++) {
        // now we parse out the types of links and preprocess them
        if (model.toList()[i].toMap()[TYPE].toString().compare(NAVIGATION_PROPERTY) == 0) { // link
            QString link = QString(LINK_TEMPLATE);

            if (model.toList()[i].toMap()[NAME].toString().compare("Category") == 0) {
                link.replace("{{schema}}", "http://schemas.microsoft.com/ado/2007/08/dataservices/related/Category");
                link.replace("{{title}}", "Category");
            }
            else if (model.toList()[i].toMap()[NAME].toString().compare("Supplier") == 0) {
                link.replace("{{schema}}", "http://schemas.microsoft.com/ado/2007/08/dataservices/related/Supplier");
                link.replace("{{title}}", "Supplier");
            }

            link.replace("{{link}}", model.toList()[i].toMap()[DATA].toString());

            links.append(link);
        }
    }

    ODataObjectModel* updateModel = new ODataObjectModel();
    updateModel->updateModel(itemUrl, "ODataDemo.Product", model, links);

    connect(updateModel, SIGNAL(modelUpdated()), this, SLOT(updateComplete()));
}

void ApplicationUI::createComplete() {
    emit createProductSuccess();
}

void ApplicationUI::updateComplete() {
    emit updateProductSuccess();
}
