// Default empty project template
#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>

#include <ODataService.h>

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

    Q_INVOKABLE void createProduct(QVariant model);
    Q_INVOKABLE void updateProduct(QString itemUrl, QVariant model);

signals:
    void createProductSuccess();
    void updateProductSuccess();

public slots:
    void createComplete();
    void updateComplete();

private:
    ODataService* _dataService;
};


#endif /* ApplicationUI_HPP_ */
