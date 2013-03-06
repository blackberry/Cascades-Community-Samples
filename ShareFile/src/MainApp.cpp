/*
*  This sample used the original Filepicker sample and added a share button
*
*  Developed by Steven Kader
*  The Jared Compoany
*
*  You can use this sample for anything that you want in part or in whole
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
