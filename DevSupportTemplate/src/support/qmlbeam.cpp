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
#include "qmlbeam.h"

#include <QDeclarativeContext>

#include <bb/cascades/AbstractPane>
#include <bb/cascades/core/developmentsupport.h>
#include <bb/cascades/QmlDocument>

using namespace bb::cascades;

const QString QmlBeam::mDefaultMainFile("asset:///main.qml");

QmlBeam::QmlBeam(QObject *parent) :
        QObject(parent)
{
    bool connectResult;
    Q_UNUSED(connectResult);

    // Initiate the name of the application main QML file to the default value.
    mMainFile = mDefaultMainFile;

    // Get the application scene, its needed since the scene is recreated when
    // QML files are altered.
    mRoot = Application::instance()->scene();
    Q_ASSERT(mRoot);

    mRoot->setParent(this);

    // Create the Development support object and connect to its signal,
    // so that the application scene can be recreated as changes are made
    // to the QML files.
    DevelopmentSupport* devSupport = new DevelopmentSupport(this);
    connectResult = connect(devSupport, SIGNAL(assetsChanged(QUrl)), SLOT(reloadQML(QUrl)));
    Q_ASSERT(connectResult);

    connectResult = connect(Application::instance(), SIGNAL(aboutToQuit()), SLOT(cleanup()));

    Q_ASSERT(connectResult);
}

void QmlBeam::reloadQML(QUrl mainFile)
{
    // The mainFile parameter is ignored, instead a property is used to cover non-default asset names.
    Q_UNUSED(mainFile);

    // Get the context of the first scene root to keep the contextProperties.
    QDeclarativeContext* context = QDeclarativeEngine::contextForObject(mRoot);

    // Clear the QML cache.
    QDeclarativeEngine* appEngine = context->engine();
    appEngine->clearComponentCache();

    // Reload all QML.
    QmlDocument* qml = QmlDocument::create(mMainFile);
    AbstractPane *root = qml->createRootObject<AbstractPane>(context);
    qml->setParent(root);
    Application::instance()->setScene(root);
}

void QmlBeam::cleanup()
{
    // Clean up.
    Application::instance()->setScene(0);
    mRoot->setParent(0);
    delete mRoot;
}

void QmlBeam::setMainFile(const QString &mainFile)
{
    if (mainFile != mMainFile.toString()) {
        mMainFile = QUrl(mainFile);
        emit mainFileChanged(mainFile);
    }
}

QString QmlBeam::mainFile() const
{
    return mMainFile.toString();
}
