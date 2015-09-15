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

#ifndef CASCADESTESTER_HPP
#define CASCADESTESTER_HPP

#include <bb/cascades/Application>
#include <bb/cascades/Page>

#include <QtCore/QObject>
#include <QtTest/QtTest>

/**
 * @short A helper class to execute QTestLib based unit tests in Cascades
 *
 * This class executes a QTestLib based unit test inside the main event loop
 * of Cascades, so that Cascades controls can be tested.
 */
class CascadesTester : public QObject
{
    Q_OBJECT

    public:
        /**
         * Creates a new cascades tester object.
         *
         * @param object The unit test object.
         * @param argc The count of the command line arguments.
         * @param argv The command line arguments.
         */
        explicit CascadesTester(QObject *object, int argc = 0, char **argv = 0);

    public Q_SLOTS:
        /**
         * Starts the execution of the unit test.
         */
        void startTest();

    private:
        QObject *m_object;
        int m_argc;
        char **m_argv;
};

#define QTEST_CASCADES_MAIN(TestObject) \
int main(int argc, char **argv) \
{ \
    bb::cascades::Application app(argc, argv); \
    TestObject tc; \
    CascadesTester tester(&tc, argc, argv); \
    bb::cascades::Application::instance()->setScene(new bb::cascades::Page()); \
    QMetaObject::invokeMethod(&tester, "startTest", Qt::QueuedConnection); \
    return bb::cascades::Application::exec(); \
}

#endif
