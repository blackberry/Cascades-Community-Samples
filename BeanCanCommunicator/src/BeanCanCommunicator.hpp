// Default empty project template
#ifndef BeanCanCommunicator_HPP_
#define BeanCanCommunicator_HPP_

#include <QObject>
#include "SoundManager.h"
#include <bb/system/InvokeManager.hpp>
#include <bb/system/InvokeRequest.hpp>
#include <bb/system/NfcShareManager>


namespace bb { namespace cascades { class Application; }}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class BeanCanCommunicator : public QObject
{
    Q_OBJECT
public:
    BeanCanCommunicator(bb::cascades::Application *app);
    virtual ~BeanCanCommunicator();

	Q_INVOKABLE	void playSound(const QString &msg);

private:
	SoundManager *_sound_manager;
    bb::system::InvokeManager *_invokeManager;
    bb::system::NfcShareManager *_nfcShareManager;
    QObject *_talkPage;
    QString getTextFromTextRecordPayload(const QByteArray &payload);
signals:
	void messageReceived(const QVariant &message);

public slots:
	void receivedInvokeRequest(const bb::system::InvokeRequest& request);
	void messageChanged(QString message);
	void finished (bb::system::NfcShareSuccess::Type result);

};


#endif /* BeanCanCommunicator_HPP_ */
