#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include "NfcSharing.hpp"

using namespace bb::cascades;

NfcSharing::NfcSharing(bb::cascades::Application *app) : QObject(app), _sharingActive(false)
{
    _qml = QmlDocument::create("asset:///main.qml");
    _root = _qml->createRootObject<AbstractPane>();
    _shareFilePage = _root->findChild<QObject*>("shareFilePage");
    _shareDataPage = _root->findChild<QObject*>("shareDataPage");
    _shareUrlPage  = _root->findChild<QObject*>("shareUrlPage");

    _qml->setContextProperty("_nfcSharing", this);

    _nfcShareManager = new NfcShareManager();

    // ============== Events from TabbedPane page

    QObject::connect(           _root, SIGNAL(disableSharing()),
    		                     this,   SLOT(disableSharing()));

    // ============== Data Sharing events

    QObject::connect(  _shareDataPage, SIGNAL(enableDataSharing()),
    		                     this,   SLOT(enableDataSharing()));
    QObject::connect(  _shareDataPage, SIGNAL(disableSharing()),
    		                     this,   SLOT(disableSharing()));
    QObject::connect(  _shareDataPage, SIGNAL(updateMessage(QString, QString)),
    		                     this,   SLOT(dataShareContentChanged(QString, QString)));

    // ============== File Sharing events

    QObject::connect(  _shareFilePage, SIGNAL(enableFileSharing()),
    		                     this,   SLOT(enableFileSharing()));
    QObject::connect(  _shareFilePage, SIGNAL(disableSharing()),
    		                     this,   SLOT(disableSharing()));
    QObject::connect(  _shareFilePage, SIGNAL(updatedFileList(QString)),
    		                     this,   SLOT(fileShareContentChanged(QString)));

    // ============== Url Sharing events

    QObject::connect(   _shareUrlPage, SIGNAL(enableUrlSharing()),
    		                     this,   SLOT(enableUrlSharing()));
    QObject::connect(   _shareUrlPage, SIGNAL(disableSharing()),
    		                     this,   SLOT(disableSharing()));
    QObject::connect(   _shareUrlPage, SIGNAL(updateUrl(QString)),
    		                     this,   SLOT(urlShareContentChanged(QString)));

    // ============== Signals from NfcShareManager

    QObject::connect(_nfcShareManager, SIGNAL(shareModeChanged(bb::system::NfcShareMode::Type)),
    		                     this,   SLOT(shareModeChanged(bb::system::NfcShareMode::Type)));
    QObject::connect(_nfcShareManager, SIGNAL(finished(bb::system::NfcShareSuccess::Type)),
    		                     this,   SLOT(finished(bb::system::NfcShareSuccess::Type)));
    QObject::connect(_nfcShareManager, SIGNAL(error(bb::system::NfcShareError::Type)),
    		                     this,   SLOT(error(bb::system::NfcShareError::Type)));

    // ============== Notify NfcShareManager to set Data Sharing Mode

    QObject::connect(            this, SIGNAL(setShareMode(bb::system::NfcShareMode::Type)),
    		         _nfcShareManager,   SLOT(setShareMode(bb::system::NfcShareMode::Type)));

    app->setScene(_root);
}


void NfcSharing::fileShareContentChanged(QString paths) {
	NfcShareFilesContent request;
    QList<QUrl> urls;
    QDir dir;

    QStringList list = paths.split(",");

    qDebug() << "XXXX Current Path" << dir.currentPath() << endl;
    qDebug() << "XXXX Home    Path" << dir.homePath() << endl;
    qDebug() << "XXXX Temp    Path" << dir.tempPath() << endl;

      QString publicPath(dir.currentPath().append("/app/public/"));

    for (int i = 0; i < list.size(); ++i) {

    	QUrl url(QString("file://").append(publicPath).append(list.at(i)));
        urls.append(url);

        QFileInfo fi(url.toLocalFile());

        qDebug() << "XXXX To Local File" << url.toLocalFile() << endl;
        qDebug() << "XXXX File String" << list.at(i) << endl;
        qDebug() << "XXXX File URL" << url << endl;
        qDebug() << "XXXX Absolute File Path" << fi.absoluteFilePath() << endl;
        qDebug() << "XXXX Absolute Path" << fi.absolutePath() << endl;
    }

    request.setFileUrls(urls);

    NfcShareSetContentError::Type rc = _nfcShareManager->setShareContent(request);

    qDebug() << "XXXX File setShareContent rc=" << rc << endl;
}

void NfcSharing::dataShareContentChanged(QString message, QString dataType) {
	qDebug() << "XXXX shareContentChanged message : " << message << endl;
	qDebug() << "XXXX shareContentChanged dataType: " << dataType << endl;
	NfcShareDataContent request;
    QByteArray data(message.toUtf8());
    QUrl url;
    request.setUri(url);
    request.setMimeType(dataType);
    request.setData(data);
    NfcShareSetContentError::Type rc = _nfcShareManager->setShareContent(request);
    qDebug() << "XXXX shareContentChanged rc =" << rc << endl;
}

void NfcSharing::urlShareContentChanged(QString urlString) {
	NfcShareDataContent request;
    QUrl url(urlString);
    request.setUri(url);
    NfcShareSetContentError::Type rc = _nfcShareManager->setShareContent(request);
    qDebug() << "XXXX shareContentChanged rc =" << rc << endl;
}

void NfcSharing::disableSharing() {
    qDebug() << "XXXX sharing disabled" << endl;
	setSharingActive(false);
	emit setShareMode(NfcShareMode::Disabled);
}

void NfcSharing::enableDataSharing() {
	setSharingActive(true);
	emit setShareMode(NfcShareMode::Data);
}

void NfcSharing::enableFileSharing() {
	setSharingActive(true);
	emit setShareMode(NfcShareMode::File);
}

void NfcSharing::enableUrlSharing() {
	setSharingActive(true);
	emit setShareMode(NfcShareMode::Data);
}

bool NfcSharing::sharingActive() const {
	return _sharingActive;
}

void NfcSharing::setSharingActive(bool sharingState) {
	if (_sharingActive == sharingState)
		return;
	_sharingActive = sharingState;
	emit detectSharingActive();
}

void NfcSharing::shareModeChanged(bb::system::NfcShareMode::Type mode) {
    qDebug() << "XXXX shareModeChanged mode =" << mode << endl;
}

void NfcSharing::finished(bb::system::NfcShareSuccess::Type result) {
    qDebug() << "XXXX finished result =" << result << endl;
}

void NfcSharing::error(bb::system::NfcShareError::Type error) {
    qDebug() << "XXXX error error =" << error << endl;
}

void NfcSharing::cleanUpOnExit() {
	qDebug() << "XXXX Application exiting";
}

