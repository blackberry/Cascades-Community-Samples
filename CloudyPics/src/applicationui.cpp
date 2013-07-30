// Default empty project template
#include "applicationui.hpp"
#include "CameraSettingsManager.h"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/multimedia/Camera>


using namespace bb::cascades;
using namespace bb::system;
using namespace bb::cascades::multimedia;
using namespace bb::community::camera;

ApplicationUI::ApplicationUI(bb::cascades::Application *app)
: QObject(app)
{

	qDebug() << "+++++++++ ApplicationUI Started" << endl;

	_cardManager = new CardManager(this);

	qmlRegisterType<CameraSettingsManager>("bb.community.camera", 1, 0,
				"CameraSettingsManager");

    // create scene document from main.qml asset
    // set parent to created document to ensure it exists for the whole application lifetime
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

    //qml->setContextProperty("_invoked", _invoked);



    qDebug() << "+++++++++ Creating root object and starting app" << endl;
    // create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();
    // set created root object as a scene

    qDebug() << "+++++++++ Finding camera" << endl;
    Camera* camera = root->findChild<Camera*>("camera");
    qDebug() << "+++++++++ Camera: " << camera << endl;
    _cardManager->setCamera(camera);
    app->setScene(root);
}

