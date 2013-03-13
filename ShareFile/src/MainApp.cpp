/*
*  This sample used the original Filepicker sample and added a share button
*
*  Developed by Steven Kader
*  The Jared Compoany
*
Copyright (c) 2013, JaredCo
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
following conditions are met:

Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "MainApp.hpp"
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Application>
#include <bb/cascades/Page>


#include <bb/cascades/InvokeQuery>
#include <bb/cascades/Invocation>

#include <stdio.h>

using namespace bb::cascades;
using namespace bb::system;

MainApp::MainApp(QObject *parent) :
		QObject(parent) {
	// TODO Auto-generated constructor stub
	// Load the UI description from main.qml
	QmlDocument *qml = QmlDocument::create("asset:///main.qml");
	qml->setContextProperty("cppObject", this);

	// Create the application scene
	AbstractPane *appPage = qml->createRootObject<Page>();
	Application::instance()->setScene(appPage);

}

MainApp::~MainApp() {
	// TODO Auto-generated destructor stub
}

Invocation* invocation;

void MainApp::shareFile(QString fileName) {

	fileName = fileName.prepend("file://");
	//fprintf(stderr, "In shareFile: %s \n", qPrintable(fileName));

	invocation = Invocation::create(
			InvokeQuery::create().parent(this).uri(fileName));

	connect(invocation, SIGNAL(armed()), this, SLOT(onArmed()));
	connect(invocation, SIGNAL(finished()), invocation, SLOT(deleteLater()));

}

void MainApp::onArmed() {
	invocation->trigger("bb.action.SHARE");
}
