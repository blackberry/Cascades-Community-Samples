// Default empty project template
#ifndef TemplateTest_HPP_
#define TemplateTest_HPP_

#include <QObject>

namespace bb { namespace cascades { class Application; }}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class TemplateTest : public QObject
{
    Q_OBJECT
public:
    TemplateTest(bb::cascades::Application *app);
    virtual ~TemplateTest() {}
};


#endif /* TemplateTest_HPP_ */
