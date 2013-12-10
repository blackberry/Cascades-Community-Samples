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
#ifndef QMLBEAM_H_
#define QMLBEAM_H_

#include <bb/cascades/Application>
#include <QObject>
#include <QUrl>

namespace bb
{
    namespace cascades
    {
        class AbstractPane;
    }
}

using namespace bb::cascades;

/**
 * QmlBeam Description
 *
 * Beam me up! An utility object that can be used to run the application
 * using the DevelopmentSupport signals to update the QML part of your app
 * on the fly. Every time you save a QML file the file is uploaded
 * to the device and the application scene is reloaded. Note that the
 * Preferences -> BlackBerry ->"Send QML files to device on save" option
 * in Momentics has to be enabled in order for this to work.
 */
class QmlBeam: public QObject
{
    Q_OBJECT

    /**
     * The application main QML File, the default value is set to "asset:///main.qml",
     * but if the application use a different name this property has to be set in order
     * for the correct file to be loaded when the reloadQML signal is triggered.
     */
    Q_PROPERTY( QString mainFile READ mainFile WRITE setMainFile NOTIFY mainFileChanged FINAL );

public:
    /**
     * Constructor, creates a QmlBeam object, which connects to the
     * DevelopmentSupport signals that trigger reloads of the UI.
     *
     * @param parent The parent object.
     */
    QmlBeam(QObject *parent = 0);

    /**
     * Returns the application main QML file.
     *
     * @return The asset name of the main QML file.
     */
    Q_SLOT QString mainFile() const;

    /**
     * Sets which QML file should be used to reload the UI when the reloadQML signal is triggered.
     * This has to be an assets path in the form of "asset:///myui.qml".
     *
     * @param mainFile The QML asset name of the main QML application file.
     */
    Q_SLOT void setMainFile(const QString &mainFile);

signals:
    /**
     * Emitted when the application main QML file changes.
     *
     * @param mainFile The asset name of the main QML application file.
     */
    void mainFileChanged(const QString &mainFile);

private slots:
    /**
     * The reloadQML method is where the QML gets reloaded in the application.
     *
     * @param mainFile The default name of the application root QML file, not used in this implementation.
     */
    void reloadQML(QUrl mainFile);

    /**
     * The cleanup is where the saved application context is cleaned up.
     */
    void cleanup();

private:
    // The default name of the application main QML file.
    static const QString mDefaultMainFile;

    // Root pane of the application scene.
    bb::cascades::AbstractPane* mRoot;

    // The application main QML file.
    QUrl mMainFile;
};

#endif /* QMLBEAM_H_ */
