// Default empty project template
#ifndef Pjsipdemo_HPP_
#define Pjsipdemo_HPP_

#include <QObject>

namespace bb { namespace cascades { class Application; }}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class PjsipEngine;
class Pjsipdemo : public QObject
{
    Q_OBJECT
public:
    Pjsipdemo(bb::cascades::Application *app);
    virtual ~Pjsipdemo();
private:
    PjsipEngine* m_iPjsipEngine;
};


#endif /* Pjsipdemo_HPP_ */
