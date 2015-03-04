/****************************************************************************
 **
 ** Portions Copyright (C) 2014 BlackBerry Limited.
 ** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 ** All rights reserved.
 ** Contact: Research In Motion Ltd. (http://www.rim.com/company/contact/)
 ** Contact: Nokia Corporation (qt-info@nokia.com)
 **
 ** This file is part of the examples of the BB10 Platform and is derived
 ** from a similar file that is part of the Qt Toolkit.
 **
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Research In Motion, nor the name of Nokia
 **     Corporation and its Subsidiary(-ies), nor the names of its
 **     contributors may be used to endorse or promote products
 **     derived from this software without specific prior written
 **     permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 **
 ****************************************************************************/

#include <bb/cascades/Application>
#include <bb/cascades/Control>
#include <bb/cascades/ListView>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Page>

#include "app.hpp"
#include "htmlinfo.hpp"

using namespace bb::cascades;

App::App()
    : m_htmlInfo(new HtmlInfo(this))
{
    // Fill the model with XHTML files from the assets directory
    {
        QVariantMap entry;
        entry["title"] = "Apache";
        entry["file"] = "apache_org.html";
        m_model.insert(entry);
    }
    {
        QVariantMap entry;
        entry["title"] = "RIM";
        entry["file"] = "rim_com.html";
        m_model.insert(entry);
    }
    {
        QVariantMap entry;
        entry["title"] = "Simple";
        entry["file"] = "simpleexample.html";
        m_model.insert(entry);
    }
    {
        QVariantMap entry;
        entry["title"] = "Trolltech";
        entry["file"] = "trolltech_com.html";
        m_model.insert(entry);
    }
    {
        QVariantMap entry;
        entry["title"] = "W3C";
        entry["file"] = "w3c_org.html";
        m_model.insert(entry);
    }
    {
        QVariantMap entry;
        entry["title"] = "YouTube";
        entry["file"] = "youtube_com.html";
        m_model.insert(entry);
    }

    m_model.setGrouping(ItemGrouping::None);
    m_model.setSortingKeys(QStringList() << "title");
    m_model.setSortedAscending(true);

    QmlDocument *qml = QmlDocument::create("asset:///main.qml");
    if (!qml->hasErrors()) {
        // Make the business logic objects available to the UI as context properties
        qml->setContextProperty("_app", this);
        qml->setContextProperty("_htmlInfo", m_htmlInfo);
        qml->setContextProperty("_model", &m_model);

        Page *appPage = qml->createRootObject<Page>();
        if (appPage) {
            Application::instance()->setScene(appPage);

            // Pre-select the first entry in the ListView
            setFileName(QVariantList() << QVariant(0));
        }
    }
}

void App::setFileName(const QVariantList &indexPath)
{
    // Extract the name of the selected XHTML file from the data model ...
    const QString fileName = m_model.data(indexPath).toMap().value("file").toString();

    // ... and let the information extractor process it
    m_htmlInfo->parseHtmlFile("app/native/assets/" + fileName);

    // Extract the title of the selected XHTML file from the data model ...
    const QString fileTitle = m_model.data(indexPath).toMap().value("title").toString();

    if (m_fileTitle == fileTitle)
        return;

    m_fileTitle = fileTitle;
    emit fileTitleChanged();
}

QString App::fileTitle() const
{
    return m_fileTitle;
}
