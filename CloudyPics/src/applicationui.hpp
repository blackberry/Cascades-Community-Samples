// Default empty project template
#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>

#include <bb/system/InvokeManager.hpp>
#include <bb/system/InvokeRequest.hpp>
#include "CardManager.h"

namespace bb {
namespace cascades {
class Application;
}
}

using namespace bb::community::cloudypics;

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ApplicationUI: public QObject {
	Q_OBJECT
public:
	ApplicationUI(bb::cascades::Application *app);
	virtual ~ApplicationUI() {
	}

public slots:
/*	void onInvoked(const bb::system::InvokeRequest&);
	void onCardPooled(const bb::system::CardDoneMessage&);*/


private:
	CardManager *_cardManager;
	bool _invoked;

};

#endif /* ApplicationUI_HPP_ */
