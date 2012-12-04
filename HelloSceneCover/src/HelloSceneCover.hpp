// Default empty project template
#ifndef HelloSceneCover_HPP_
#define HelloSceneCover_HPP_

#include <QObject>

namespace bb { namespace cascades { class Application; }}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class HelloSceneCover : public QObject
{
    Q_OBJECT
public:
    HelloSceneCover(bb::cascades::Application *app);
    virtual ~HelloSceneCover() {}
};


#endif /* HelloSceneCover_HPP_ */
