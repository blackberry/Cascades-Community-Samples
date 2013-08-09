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

private:
	bb::community::camera::CardManager *_cardManager;

};

#endif /* ApplicationUI_HPP_ */
