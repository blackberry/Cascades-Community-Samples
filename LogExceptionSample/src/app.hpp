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

/*
 * Core includes
 */
#include <QtCore/QObject>
#include <QtCore/QMetaType>
#include <bb/cascades/Control>
#include <bb/cascades/Application>
#include <QObject>
#include <QThread>

using namespace bb::cascades;

/*
 * Forward declaration of cascade types
 */
namespace bb
{
namespace cascades
{
class NavigationPane;
class Page;
class QmlDocument;
class TextArea;
class Button;
}
}

class App : public QObject
{
    Q_OBJECT

public:
    /*
     * Default Constructor
     */
    App();

    /*
     * Destructor
     */
    ~App();

    /*
     * Like in QT, signals and slots must be declared under 'signals'
     * or 'slots' sections as shown below.
     */
public slots:

    /*
     * This method is be used as a slot by the exception button
     * in our application.
     */
    void onGenerateExceptionButtonClicked();

    /*
     * This method is be used as a slot by the clear log button
     * in our application.
     */
    void onClearLogButtonClicked();

private:

    /*
     * This method is used to initialize signals and slots for our application.
     */
    bool connectButtons();

    /*
     * Method used to generate an exception of type CustomException
     * for the purpose of this sample.
     */
    void generateException();

    /*
     * Method used to load the log file into a QString.
     * @return The contents of the log file in a QString object.
     */
    QString loadLogFile();

    /*
     * Method used to display the log into the applicaiton's main texfield.
     * @param textContents The text value to be displayed.
     */
    void displayLog(QString& textContents);

    /*
     * This handle represents the main application Page container.
     */
    Page* m_mainPage;

    /*
     * A handle on the text area to display the logs.
     * Keeping this handle in the object simplifies the code.
     */
    TextArea* m_logTextArea;


    void testThrow();

};

#endif /* ifndef APP_H */




