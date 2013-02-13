// Default empty project template
#include "BeanCanCommunicator.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <QtNfcSubset/qndefmessage.h>
#include <sys/types.h>

using namespace bb::cascades;

BeanCanCommunicator::BeanCanCommunicator(bb::cascades::Application *app) :
		QObject(app) {

	_invokeManager = new bb::system::InvokeManager();
	QObject::connect(_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(receivedInvokeRequest(const bb::system::InvokeRequest&)));

	_nfcShareManager = new bb::system::NfcShareManager();
	_nfcShareManager->reset();
	QObject::connect(_nfcShareManager, SIGNAL(finished(bb::system::NfcShareSuccess::Type)), this, SLOT(finished(bb::system::NfcShareSuccess::Type)));

	// create scene document from main.qml asset
	// set parent to created document to ensure it exists for the whole application lifetime
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	if (!qml->hasErrors()) {

		// create root object for the UI
		AbstractPane *root = qml->createRootObject<AbstractPane>();
		// set created root object as a scene
		qml->setContextProperty("app", this);

		_sound_manager = new SoundManager("sounds/");

		_talkPage = root->findChild<QObject*>("talky");

		QObject::connect(_talkPage, SIGNAL(updateMessage(QString)), this, SLOT(messageChanged(QString)));
		QObject::connect(this, SIGNAL(messageReceived(QVariant)), _talkPage, SLOT(messageReceived(QVariant)));

		app->setScene(root);
	}
}

BeanCanCommunicator::~BeanCanCommunicator() {
	qDebug() << "XXXX BeanCanCommunicator destructor";
}

void BeanCanCommunicator::playSound(const QString &msg) {
	_sound_manager->play(msg);
}

void BeanCanCommunicator::messageChanged(QString message) {
	qDebug() << "XXXX messageChanged:" << message;
	_nfcShareManager->setShareMode(bb::system::NfcShareMode::Data);
	bb::system::NfcShareDataContent request;
	QByteArray data(message.toUtf8());
	QUrl url;
	request.setUri(url);
	request.setMimeType("text/plain");
	request.setData(data);
	bb::system::NfcShareSetContentError::Type rc = _nfcShareManager->setShareContent(request);
	qDebug() << "XXXX messageChanged rc =" << rc << endl;
}

void BeanCanCommunicator::receivedInvokeRequest(const bb::system::InvokeRequest& request) {

	qDebug() << "XXXX received invoke request";

	if (request.mimeType().compare("application/vnd.rim.nfc.ndef") != 0) {
		qDebug() << "XXXX unexpected MIME type in invoke request";
		return;
	}
	QByteArray data = request.data();
	QtMobilitySubset::QNdefMessage ndefMessage = QtMobilitySubset::QNdefMessage::fromByteArray(data);
	QList<QtMobilitySubset::QNdefRecord>::const_iterator ndefRecord;
	ndefRecord = ndefMessage.begin();

	qDebug() << "XXXX TNF=" << ndefRecord->typeNameFormat();
	qDebug() << "XXXX Type=" << QString(ndefRecord->type());
	//	XXXX TNF= 2
	//	XXXX Type= "text/plain"

	if (ndefRecord->typeNameFormat() == QtMobilitySubset::QNdefRecord::NfcRtd) {
		qDebug() << "XXXX TNF_WELL_KNOWN (RTD)";
		if (QString(ndefRecord->type()).compare("T") == 0) {
			qDebug() << "XXXX Payload=" << QString(ndefRecord->payload());
			emit messageReceived(getTextFromTextRecordPayload(ndefRecord->payload()));
		}
	}

	if (_sound_manager == 0) {
		_sound_manager = new SoundManager("sounds/");
	}
	qDebug() << "XXXX playing sound effect";
	playSound("old_phone_2.wav");
}

QString BeanCanCommunicator::getTextFromTextRecordPayload(const QByteArray &payload) {

	QString text;

	qDebug() << "XXXX Text - raw: " << payload.toHex();

	uchar_t statusByte = payload[0];
	bool isUtf16 = false;

	qDebug() << "XXXX StatusByte: " << statusByte;

	if ( statusByte & 0x80 ) { // bit 7 is UTF type ( 8 or 16 )
		isUtf16 = true;
	}

	int langLen = statusByte & 0x3F; // lower 6 bits is language length
	int textLength = payload.length() - (langLen + 1);

	qDebug() << "XXXX Lang Length: " << langLen;
	qDebug() << "XXXX Text Length: " << textLength;

	if (isUtf16) {
		text = QString(payload.mid(1+langLen).constData());
	} else {
		text = QString::fromUtf8(payload.mid(1+langLen).constData());
	}

    qDebug() << "XXXX Text       : " << text;

	return text;
}

void BeanCanCommunicator::finished (bb::system::NfcShareSuccess::Type result) {
	if (result == 1) {
		qDebug() << "XXXX finished sharing over SNEP";
	} else {
		if (result == 2) {
			qDebug() << "XXXX finished sharing by tag write";
		} else {
			qDebug() << "XXXX finished sharing with unexpected result " << result;
		}
	}
	_nfcShareManager->reset();
}
