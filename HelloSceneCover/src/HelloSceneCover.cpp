// Default empty project template
#include "HelloSceneCover.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

// Required for Scene Cover to work
#include <bb/cascades/SceneCover>
#include <bb/cascades/AbstractCover>

using namespace bb::cascades;

HelloSceneCover::HelloSceneCover(bb::cascades::Application *app)
: QObject(app)
{
	
	// Required for SceneCover to work
	qmlRegisterType<SceneCover>("bb.cascades", 1, 0, "SceneCover");
	qmlRegisterUncreatableType<AbstractCover>("bb.cascades", 1, 0, "AbstractCover", "");
	
    // create scene document from main.qml asset
    // set parent to created document to ensure it exists for the whole application lifetime
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
	
    // create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();
    // set created root object as a scene
    app->setScene(root);
}

