// Default empty project template
#ifndef NfcChicken_HPP_
#define NfcChicken_HPP_

#include <QObject>
#include "SoundManager.h"
#include <bb/system/InvokeManager.hpp>
#include <bb/system/InvokeRequest.hpp>


namespace bb {
namespace cascades {
class Application;
}
}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class NfcChicken: public QObject {
Q_OBJECT

public:
	NfcChicken(bb::cascades::Application *app);
	virtual ~NfcChicken();
	Q_INVOKABLE	void playSound(const QString &msg);

private:
	SoundManager *_sound_manager;
    bb::system::InvokeManager *_invokeManager;

public slots:
	void receivedInvokeRequest(const bb::system::InvokeRequest& request);


};

#endif /* NfcChicken_HPP_ */
