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

#ifndef APP_HPP
#define APP_HPP

#include <QtCore/QObject>

#include <bb/cascades/GroupDataModel>

class HtmlInfo;

/**
 * This sample application shows how to parse XML (XHTML) documents with
 * the QXmlStreamReader class.
 * The user can select a XHTML document from the predefined list of files
 * and the application will extract various information (title, paragraph count etc.)
 * from it and show them in the UI.
 */
class App : public QObject
{
    Q_OBJECT

    // This property makes the title of the currently selected file accessible to the UI
    Q_PROPERTY(QString fileTitle READ fileTitle NOTIFY fileTitleChanged)

public:
    App();

    // This method is called when the user has selected an XHTML document in the ListView
    Q_INVOKABLE void setFileName(const QVariantList &indexPath);

Q_SIGNALS:
    // The change notification signal for the file title property
    void fileTitleChanged();

private:
    // The accessor method for the file title property
    QString fileTitle() const;

    // The object that extracts the information from the XHTML document
    HtmlInfo *m_htmlInfo;

    // The model of available XML documents to process
    bb::cascades::GroupDataModel m_model;

    // Contains the title of the currently selected file
    QString m_fileTitle;
};

#endif
