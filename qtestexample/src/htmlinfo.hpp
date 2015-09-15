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

#ifndef HTMLINFO_HPP
#define HTMLINFO_HPP

#include <QtCore/QObject>

/**
 * The HtmlInfo object contains the business logic of this application.
 *
 * It parses a given XML (XHTML) file with a QXmlStreamReader and extracts
 * information about the document. These information are available to the UI
 * via properties.
 */
class HtmlInfo : public QObject
{
    Q_OBJECT

    // Makes the title of the document available to the UI
    Q_PROPERTY(QString title READ title NOTIFY infoChanged)

    // Makes the number of paragraphs in the document available to the UI
    Q_PROPERTY(int paragraphs READ paragraphs NOTIFY infoChanged)

    // Makes the number of links in the document available to the UI
    Q_PROPERTY(int links READ links NOTIFY infoChanged)

    // Makes the link content (everything between <a> and </a>) available to the UI
    Q_PROPERTY(QString linksContent READ linksContent NOTIFY infoChanged)

    // Makes the error message available to the UI
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)

public:
    HtmlInfo(QObject *parent = 0);

public Q_SLOTS:
    // This method starts the actual information extraction
    void parseHtmlFile(const QString &fileName);

Q_SIGNALS:
    // The change notification signals of the properties
    void infoChanged();
    void errorChanged();

private:
    // The accessor methods of the properties
    QString title() const;
    int paragraphs() const;
    int links() const;
    QString linksContent() const;
    QString error() const;

    // The property values
    QString m_title;
    int m_paragraphs;
    int m_links;
    QString m_linksContent;
    QString m_error;
};

#endif
