/* Copyright (c) 2013 BlackBerry Limited.
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
#include <QEasingCurve>
#include <QVariant>
#include <QFuture>
#include <QMutex>
#include <QFutureWatcher>

namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
    }
}

class QTranslator;

class ApplicationUI: public QObject
{

	Q_OBJECT

public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI() {}

    Q_INVOKABLE void findBleDevices();
    Q_INVOKABLE void deviceSelected(int index);

signals:
	void deviceSelected(QVariant device_inx, QVariant device_addr);

private slots:
    void onSystemLanguageChanged();
    void finishedSearching();

private:
    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;
    QFuture<void> *_future;
    QFutureWatcher<void> *_watcher;
    QMutex _mutex;
};

#endif /* ApplicationUI_HPP_ */
