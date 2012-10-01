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

#ifndef APP_H
#define APP_H

#include <QtCore/QObject>
#include <bb/cascades/Application>
#include <bb/system/InvokeManager.hpp>
#include <bb/system/InvokeRequest.hpp>
#include <QtNfcSubset/qndefmessage.h>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include <bps/navigator.h>
#include <bps/navigator_invoke.h>

#include "NfcListener.hpp"

/*!
 * @brief Application GUI object
 */
class App : public QObject
{
    Q_OBJECT

public:
    App(bb::cascades::Application *app);
	virtual ~App();

    Q_INVOKABLE QString getValueFor(const QString &objectName, const QString &defaultValue);
    Q_INVOKABLE void saveValueFor(const QString &objectName, const QString &inputValue);
    Q_INVOKABLE void exitApp();

private:
    bb::system::InvokeManager *_invokeManager;
    bool _launchedByInvoke;
	bb::cascades::Application *_app;
    bb::cascades::QmlDocument *_qml;
    bb::cascades::AbstractPane *_root;
    NfcListener *_nfcListener;
	void initialiseListeners();

public slots:

signals:
	void startListening();
	void stopListening();
};

#endif // ifndef APP_H
