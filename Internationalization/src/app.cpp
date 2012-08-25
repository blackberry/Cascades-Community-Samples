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

/*
 * Image sources & attribution:
 *
 * Tiger Image:
 * By J. Patrick Fischer (Own work) [CC-BY-SA-3.0 (http://creativecommons.org/licenses/by-sa/3.0)], via Wikimedia Commons
 * http://commons.wikimedia.org/wiki/File%3A2012_Suedchinesischer_Tiger.JPG
 *
 * Panda Image:
 * Jcwf from nl [GFDL (http://www.gnu.org/copyleft/fdl.html) or CC-BY-SA-3.0 (http://creativecommons.org/licenses/by-sa/3.0/)], via Wikimedia Commons
 * http://commons.wikimedia.org/wiki/File%3APanda_closeup.jpg
 *
 * Horse Image:
 * Pottok at the German language Wikipedia [GFDL (http://www.gnu.org/copyleft/fdl.html) or CC-BY-SA-3.0 (http://creativecommons.org/licenses/by-sa/3.0/)], from Wikimedia Commons
 * http://commons.wikimedia.org/wiki/File%3AArravani-Stuten.jpg
 *
 * Cat Image:
 * Luis Garc’a [GFDL (http://www.gnu.org/copyleft/fdl.html) or CC-BY-SA-3.0-2.5-2.0-1.0 (http://creativecommons.org/licenses/by-sa/3.0)], via Wikimedia Commons
 * http://commons.wikimedia.org/wiki/File%3AGato_callejero_en_Madrid_02.jpg
 *
 * Dog Image:
 * By Dr. Manfred Herrmann Allgemeiner Deutscher Rottweiler-Klub (ADRK) e.V. http://www.ADRK.de (Image:Rottweiler.jpg) [GFDL (http://www.gnu.org/copyleft/fdl.html) or CC-BY-SA-3.0 (http://creativecommons.org/licenses/by-sa/3.0/)], via Wikimedia Commons
 * http://commons.wikimedia.org/wiki/File%3ARottweiler_standing_facing_left.jpg
 *
 * Car Image:
 * By Redsimon at en.wikipedia [CC-BY-2.5 (http://creativecommons.org/licenses/by/2.5)], from Wikimedia Commons
 * http://commons.wikimedia.org/wiki/File%3AAustin_A110_Westminster_MkII_head.jpg
 *
 * Mountain Image:
 * By Ben Tubby (originally posted to Flickr as Makalu) [CC-BY-2.0 (http://creativecommons.org/licenses/by/2.0)], via Wikimedia Commons
 * http://commons.wikimedia.org/wiki/File%3AMakalu.jpg
 *
 * Telephone Image:
 * By Holger.Ellgaard (Own work) [CC-BY-SA-3.0 (http://creativecommons.org/licenses/by-sa/3.0)], via Wikimedia Commons
 * http://commons.wikimedia.org/wiki/File%3ADialog_gr_1972.jpg
 *
 */


#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/ListView>
#include <bb/cascades/GroupDataModel>
#include <bb/cascades/Container>
#include <bb/cascades/Option>
#include <bb/cascades/Label>
#include <bb/cascades/RadioGroup>
#include <bb/cascades/SegmentedControl>
#include <bb/cascades/XmlDataModel>
#include <bps/virtualkeyboard.h>
#include <QUrl>
#include <QFile>
#include "app.hpp"

using namespace bb::cascades;

/**
 * This application demonstrates how to install/use appropriate
 * translation engine.
 *
 */


/**
 * App::App()
 *
 * Constructor
 *
 * Initial setup to expose C++ objects to be used by the QML view
 */
App::App()
    : m_app(NULL)
    , m_currentLocale("en")
    , m_currentXML("creatures")
    , m_translator(NULL)
{
    QmlDocument* qml = QmlDocument::create("main.qml");
    qml->setContextProperty("cs", this);

    m_root = qml->createRootNode<AbstractPane>();
    Application::setScene(m_root);
}

/**
 * void App::setApplication(bb::cascades::Application* app, QTranslator* translator, QString currentLocale)
 *
 * This method allows to change translation engine basing on the given locale
 * at runtime.
 *
 */
void App::setApplication(bb::cascades::Application* app, QTranslator* translator, QString currentLocale)
{
    m_app = app;
    m_translator = translator;
    m_currentLocale = currentLocale;
    updateLocale("");

    //update certain controls to reflect the correct state
    //e.g language bar at the top should have the right language
    //if app is started in French for example
    SegmentedControl* sg1 = m_root->findChild<SegmentedControl*>("languageBar");
    if (sg1)
    {
        if (m_currentLocale == "fr_FR")
        {
            sg1->setSelectedIndex(1);
        }
        else if (m_currentLocale == "es_ES")
        {
            sg1->setSelectedIndex(2);
        }
    }
    RadioGroup* radio = m_root->findChild<RadioGroup*>("radioGroup1");
    if (radio)
    {
        radio->setSelectedIndex(1);
    }
}

/**
 * App::updateLocale(QString locale)
 *
 * Update view content basing on the given locale.
 *
 */
void App::updateLocale(QString locale)
{
    if (!m_app)
    {
        qDebug() << "app pointer not valid";
        return;
    }

    if (!m_translator)
    {
        qDebug() << "translator pointer not valid";
        return;
    }

    // if locale is empty - refresh current. otherwise change the local
    if (!locale.trimmed().isEmpty() && m_currentLocale != locale)
    {
        m_currentLocale = locale;

        qDebug() << "updating UI to language: " << m_currentLocale;
        QString filename = QString("Internationalization_%1").arg(m_currentLocale);
        if (m_translator->load(filename, "app/native/qm"))
        {
            // multiple translators can be installed but for this
            // app we only use one translator instance for brevity
            m_app->removeTranslator(m_translator);
            m_app->installTranslator(m_translator);
        }
    }

    // load the new xml datamodel for the language
    ListView* listView = m_root->findChild<ListView*>("listView");
    if (listView)
    {
        XmlDataModel* dm = new XmlDataModel();
        QString xmlfile = QString("%1_%2.xml").arg(m_currentXML).arg(m_currentLocale);
        if (QFile::exists(QString("app/native/assets/") + xmlfile) == false)
        {
            xmlfile = QString("%1_en.xml").arg(m_currentXML);
        }
        dm->setSource(QUrl(xmlfile));
        listView->setDataModel(dm);
    }

    // re-translate controls.
    SegmentedControl* sg1 = m_root->findChild<SegmentedControl*>("languageBar");
    if (sg1)
    {
        Option* opt = sg1->at(0);
        if (opt)
        {
            opt->setText(tr("English"));
        }

        opt = sg1->at(1);
        if (opt)
        {
            opt->setText(tr("French"));
        }

        opt = sg1->at(2);
        if (opt)
        {
            opt->setText(tr("Spanish"));
        }
    }
    Label* switchTo = m_root->findChild<Label*>("switchToLabel");
    if (switchTo)
    {
        switchTo->setText(tr("Switch to:"));
    }
    Label* currentLang = m_root->findChild<Label*>("currentLangLabel");
    if (currentLang)
    {
        currentLang->setText(tr("Your current language is:  ") + getCurrentLanguage());
    }
    RadioGroup* radio = m_root->findChild<RadioGroup*>("radioGroup1");
    if (radio)
    {
        Option* opt = radio->at(0);
        if (opt)
        {
            opt->setText(tr("Animals"));
        }
        opt = radio->at(1);
        if (opt)
        {
            opt->setText(tr("Objects"));
        }
    }
}

/**
 * App::getCurrentLanguage()
 *
 * Retrieve the language name corresponding to the current locale.
 */
QString App::getCurrentLanguage()
{
    if (m_currentLocale == "fr_FR")
    {
        return tr("French");
    }
    else if (m_currentLocale == "es_ES")
    {
        return tr("Spanish");
    }
    else
    {
        return tr("English");
    }
}

/**
 * App::suppressKeyboard()
 *
 * A helper function to force the keyboard to hide
 */
void App::suppressKeyboard()
{
    virtualkeyboard_request_events(0);
    virtualkeyboard_hide();
}

/**
 * App::loadXML(QString name)
 *
 * Update the data model of the list view to use a XML source determined
 * by the given name (e.g.: "creatures" or "objects") and the current locale.
 *
 */
void App::loadXML(QString name)
{
    m_currentXML = name;

    // load the new xml datamodel for the language
    ListView* listView = m_root->findChild<ListView*>("listView");
    if (listView)
    {
        XmlDataModel* dm = new XmlDataModel();
        QString xmlfile = QString("%1_%2.xml").arg(m_currentXML).arg(m_currentLocale);
        if (QFile::exists(QString("app/native/assets/") + xmlfile) == false)
        {
            xmlfile = QString("%1_en.xml").arg(m_currentXML);
        }
        dm->setSource(QUrl(xmlfile));
        listView->setDataModel(dm);
    }
}

