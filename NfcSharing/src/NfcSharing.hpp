#ifndef NfcSharing_HPP_
#define NfcSharing_HPP_

#include <QObject>

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include <bb/system/NfcShareManager>

using namespace bb::cascades;
using namespace bb::system;

namespace bb { namespace cascades { class Application; }}

class NfcSharing : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool   sharingActive
    		   READ   sharingActive
    		   WRITE  setSharingActive
    		   NOTIFY detectSharingActive)

public:
    NfcSharing(bb::cascades::Application *app);
    virtual ~NfcSharing() {}

    bool sharingActive() const;
	void setSharingActive(bool sharingState);

public slots:
    void fileShareContentChanged(QString paths);
    void dataShareContentChanged(QString message, QString dataType);
    void urlShareContentChanged(QString urlString);
    void disableSharing();
    void enableDataSharing();
    void enableFileSharing();
    void enableUrlSharing();
    void shareModeChanged(bb::system::NfcShareMode::Type mode);
    void finished(bb::system::NfcShareSuccess::Type result);
    void error(bb::system::NfcShareError::Type error);
	void cleanUpOnExit();

signals:
    void setShareMode(bb::system::NfcShareMode::Type mode);
	void detectSharingActive();

private:
    NfcShareManager *_nfcShareManager;
    QmlDocument *_qml;
    AbstractPane *_root;
    QObject *_shareFilePage;
    QObject *_shareDataPage;
    QObject *_shareUrlPage;
    bool _sharingActive;
};


#endif /* NfcSharing_HPP_ */
