// Default empty project template
#include "NfcChicken.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

NfcChicken::NfcChicken(bb::cascades::Application *app) :
		QObject(app) {

	_invokeManager = new bb::system::InvokeManager();
	QObject::connect(_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(receivedInvokeRequest(const bb::system::InvokeRequest&)));

	// create scene document from main.qml asset
	// set parent to created document to ensure it exists for the whole application lifetime
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	if (!qml->hasErrors()) {

		// create root object for the UI
		AbstractPane *root = qml->createRootObject<AbstractPane>();

		qml->setContextProperty("app", this);

		_sound_manager = new SoundManager("sounds/");
		// set created root object as a scene
		app->setScene(root);
	}
}

NfcChicken::~NfcChicken() {
	delete _sound_manager;
}

void NfcChicken::playSound(const QString &msg) {
	_sound_manager->play(msg);
}

void NfcChicken::receivedInvokeRequest(const bb::system::InvokeRequest& request) {

	QByteArray data = request.data();
	if (request.mimeType().compare("application/vnd.rim.nfc.ndef") == 0) {
		if (_sound_manager == 0) {
			_sound_manager = new SoundManager("sounds/");
		}
		_sound_manager = new SoundManager("sounds/");
		playSound("chicken1.wav");
	}
}
