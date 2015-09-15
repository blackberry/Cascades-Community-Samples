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

#include "htmlinfo.hpp"

#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QXmlStreamReader>

HtmlInfo::HtmlInfo(QObject *parent)
    : QObject(parent)
{
}

void HtmlInfo::parseHtmlFile(const QString &fileName)
{
    QFile file(fileName);

    // Reset the error flag
    m_error.clear();
    emit errorChanged();

    // Try to open the XHTML document and report an error if it fails
    if (!file.open(QIODevice::ReadOnly)) {
        m_error = tr("Couldn't open the file.");
        emit errorChanged();
        return;
    }

    // Create a XML stream reader on the file
    QXmlStreamReader reader(&file);

    int paragraphCount = 0;
    QStringList links;
    QString title;
    /**
     * Read the XHTML document token by token until the end is reached.
     * For every token check whether it's one we are interested in and extract
     * the needed information.
     */
    while (!reader.atEnd()) {
        reader.readNext();
        if (reader.isStartElement()) {
            if (reader.name() == "title")
                title = reader.readElementText();
            else if (reader.name() == "a")
                links.append(reader.attributes().value("href").toString());
            else if (reader.name() == "p")
                ++paragraphCount;
        }
    }

    // If the XML stream reader signaled an error, report it to the UI
    if (reader.hasError()) {
        m_error = tr("The HTML file isn't well-formed: %1").arg(reader.errorString());
        emit errorChanged();
        return;
    }

    // Update the properties with the extracted information
    m_title = title.simplified();
    m_paragraphs = paragraphCount;
    m_links = links.size();
    m_linksContent = links.join("\n");

    // Notify the UI that the properties have changed
    emit infoChanged();
}

QString HtmlInfo::title() const
{
    return m_title;
}

int HtmlInfo::paragraphs() const
{
    return m_paragraphs;
}

int HtmlInfo::links() const
{
    return m_links;
}

QString HtmlInfo::linksContent() const
{
    return m_linksContent;
}

QString HtmlInfo::error() const
{
    return m_error;
}
