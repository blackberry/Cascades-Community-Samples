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


#include "htmlinfotest.hpp"
#include "htmlinfo.hpp"

#include <QtTest/QtTest>
#include <iostream>

void HtmlInfoTest::testInvalidParseHtmlFile()
{
    HtmlInfo* hinfo = new HtmlInfo(this);

    QSignalSpy invalidFileSpy(hinfo, SIGNAL(errorChanged()));

    hinfo->parseHtmlFile("app/native/assets/not_existing.html");

    QCOMPARE(invalidFileSpy.count(), 2);
    QCOMPARE(hinfo->property("error").toString(), QString("Couldn't open the file."));
}

void HtmlInfoTest::testValidParseHtmlFile() {
    HtmlInfo* hinfo = new HtmlInfo(this);

    QSignalSpy validFileSpy(hinfo, SIGNAL(infoChanged()));

    hinfo->parseHtmlFile("app/native/assets/apache_org.html");

    QCOMPARE(validFileSpy.count(), 1);
}

void HtmlInfoTest::testSimpleParseHtmlFile() {
    HtmlInfo* hinfo = new HtmlInfo(this);

    QSignalSpy validFileSpy(hinfo, SIGNAL(infoChanged()));

    hinfo->parseHtmlFile("app/native/assets/simpleexample.html");

    QCOMPARE(hinfo->property("links").toInt(), 2);
    QCOMPARE(hinfo->property("paragraphs").toInt(), 3);
    QCOMPARE(hinfo->property("title").toString(), QString("Qt is cute! Frans is too!"));
}

void HtmlInfoTest::testInvalidlyFormedParseHtmlFile() {
    HtmlInfo* hinfo = new HtmlInfo(this);

    QSignalSpy invalidFormatFileSpy(hinfo, SIGNAL(errorChanged()));

    hinfo->parseHtmlFile("app/native/assets/invalidly_formed.html");

    QCOMPARE(invalidFormatFileSpy.count(), 2);

    bool result = hinfo->property("error").toString().contains("The HTML file isn't well-formed:");

    QVERIFY(result);
}
