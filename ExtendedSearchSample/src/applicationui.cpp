// Default empty project template
#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>



ApplicationUI::ApplicationUI(bb::cascades::Application *app)
: QObject(app)
{



	_invokeManager = new InvokeManager(this);
	connect(_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
			this, SLOT(onInvoked(const bb::system::InvokeRequest&)));

	switch (_invokeManager->startupMode()) {
	case ApplicationStartupMode::LaunchApplication:
		// Normal launch
		break;
	case ApplicationStartupMode::InvokeApplication:
		// App was invoked.
		// This is what the search app does, but you could
		// also end up here if you have registered for other
		// invocations.

		// You may want to set a special property or load a specific UI
		break;
	case ApplicationStartupMode::InvokeViewer:
	case ApplicationStartupMode::InvokeCard:
		// Invoked other ways, only applicable if you are using the invocation
		// framework for something other than search.
		break;
	}

    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    _root = qml->createRootObject<AbstractPane>();
    app->setScene(_root);
}

void ApplicationUI::onInvoked(const bb::system::InvokeRequest& invokeRequest) {

	if(invokeRequest.action() == "bb.action.SEARCH.EXTENDED") {
		// Begin searching in app
		_root->setProperty("searchString", QString(invokeRequest.data()));
	} else {
		// some other invocation..
	}
}


