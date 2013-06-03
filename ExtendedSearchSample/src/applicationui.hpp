// Default empty project template
#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <bb/cascades/AbstractPane>
#include <bb/system/InvokeManager.hpp>
#include <bb/system/InvokeRequest.hpp>

using namespace bb::cascades;
using namespace bb::system;

namespace bb { namespace cascades { class Application; }}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ApplicationUI : public QObject
{
    Q_OBJECT
public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI() {}

public slots:
	void onInvoked(const bb::system::InvokeRequest& invokeRequest);

private:
	AbstractPane *_root;
	InvokeManager *_invokeManager;
};


#endif /* ApplicationUI_HPP_ */
