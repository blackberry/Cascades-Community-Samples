/*
 * Copyright (c) 2011, 2012 Research In Motion Limited.
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
#include <bb/cascades/AbsoluteLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/Application>
#include <bb/cascades/Button>
#include <bb/cascades/Container>
#include <bb/cascades/Color>
#include <bb/cascades/ForeignWindow>
#include <bb/cascades/ImplicitAnimationController>
#include <bb/cascades/Page>
#include <bb/cascades/QmlDocument>
#include <QtCore/QTimer>
#include <QDebug>


#include "GoodCitizenApp.hpp"

#include <screen/screen.h>

#include <GLES/gl.h>
#include <GLES/glext.h>
#include <EGL/egl.h>
#include <sys/strm.h>

using namespace bb::cascades;

GoodCitizenApp::GoodCitizenApp()
{
    QmlDocument *qml = QmlDocument::create("main.qml");

    if (!qml->hasErrors()) {

        // The application NavigationPane is created from QML.
    	m_navPane = qml->createRootNode<NavigationPane>();
        if (m_navPane) {

        	m_pOpenGLThread = new OpenGLThread();

        	m_pGoodCitizen = new GoodCitizen();

        	//m_pGoodCitizen->moveToThread(m_pOpenGLThread);
        	m_pOpenGLThread->addView(m_pGoodCitizen);

            qml->setContextProperty("_navPane", m_navPane);
            qml->setContextProperty("_goodCitizen", m_pGoodCitizen);
            qml->setContextProperty("_openGLThread", m_pOpenGLThread);

            // Finally the main scene for the application is set the Page.
            Application::setScene(m_navPane);

            // use a timer to startup OpenGL code
            QTimer* timer = new QTimer(this);
            timer->setSingleShot(true);

        	QObject::connect(timer,
        			SIGNAL(timeout()),
        			this,
        			SLOT(onTimeout()));

        	timer->start(1000);
        }
    }
}

void GoodCitizenApp::onAttachedChanged(bool changed) {
	qDebug()  << "GoodCitizenApp::onAttachedChanged: " << changed;
}

void GoodCitizenApp::onWindowAttached(unsigned long handle, const QString &group, const QString &id) {

	qDebug()  << "GoodCitizenApp::onWindowAttached: " << handle << ":" << group << ":" << id;
}

void GoodCitizenApp::onTimeout() {

	qDebug()  << "GoodCitizenApp::onCreationCompleted: ";

    // Get the foreign window Control specified in QML and attach to the window attached signal.
	//Page *page = m_navPane->findChild<Page*>("nav");
    //Container *container = page->findChild<Container*>("back");
	m_pForeignWindow = m_navPane->findChild<ForeignWindow*>("goodCitizenCascadesFW");

	qDebug()  << "GoodCitizenApp::onCreationCompleted: " << m_pForeignWindow;

	QObject::connect(m_pForeignWindow,
			SIGNAL(windowAttached(unsigned long,const QString&,const QString&)),
			this,
			SLOT(onWindowAttached(unsigned long,const QString&,const QString&)));

	// connect ForeignWindow signals to slots
	QObject::connect(m_pForeignWindow, SIGNAL(touch(bb::cascades::TouchEvent *)),
					 m_pGoodCitizen,   SLOT(onTouch(bb::cascades::TouchEvent *)) );

	StackLayoutProperties *layoutProperties =
            dynamic_cast<StackLayoutProperties*>(m_pForeignWindow->layoutProperties());

	m_pGoodCitizen->bind(ForeignWindow::mainWindowGroupId(), "goodCitizenCascadesAppID",
			(int) m_pForeignWindow->translationX(), (int) m_pForeignWindow->translationY(), m_pForeignWindow->preferredWidth(), m_pForeignWindow->preferredHeight());

	m_pOpenGLThread->start();

	m_pForeignWindow->setVisible(true);
}
