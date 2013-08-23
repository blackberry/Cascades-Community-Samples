/* Copyright (c) 2012 Research In Motion Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>

namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
    }
}

class QTranslator;

/*!
 * @brief Application object
 *
 *
 */

class ApplicationUI: public QObject
{
Q_OBJECT

    // a Q_PROPERTY declaration is a binding between a property we can use in QML and underlying C++ methods and signals

Q_PROPERTY(
        // the name of our property as used from QML
        QString my_property
        // a method that will be called to populate our QML property with a value from C++
        READ getMyProperty
        // a method that will be called when we want to change the value of our corresponding C++ variable
        WRITE setMyProperty
        // a signal that will be emitted if the underlying data value changes. This signal will be automatically connected to our QML property
        // so that anywhere our property has been used, it's value will be updated
        NOTIFY signalMyPropertyChanged)

public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI()
    {
    }

    // for my Q_PROPERTY
    QString getMyProperty();
    void setMyProperty(QString text);

    Q_INVOKABLE void resetMyProperty();

signals:
    // signal that is emitted if my property value changes
    void signalMyPropertyChanged();

private slots:
    void onSystemLanguageChanged();
private:
    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;
    // the data we bound to our QML property
    QString _some_text;

    void logMsg(const QVariant &msg);
};

#endif /* ApplicationUI_HPP_ */
