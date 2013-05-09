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
#include <bb/cascades/Window>
#include <bb/cascades/Button>
#include <bb/cascades/Container>
#include <bb/cascades/Color>
#include <bb/cascades/ForeignWindowControl>
#include <bb/cascades/ImplicitAnimationController>
#include <bb/cascades/Page>
#include <bb/cascades/LayoutUpdateHandler>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/SceneCover>

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

	Application::instance()->setAutoExit(false);

	QObject::connect(
			Application::instance(),
			SIGNAL(manualExit()),
			this,
			SLOT(shutdown()));


    QmlDocument *qml = QmlDocument::create("asset:///main.qml");

    if (!qml->hasErrors()) {
       	OpenGLView::setRenderingAPI(GL_ES_1);

		m_pGoodCitizen = new GoodCitizen(DISPLAY_DEVICE);

        qml->setContextProperty("_goodCitizen", m_pGoodCitizen);

        // The application NavigationPane is created from QML.
    	m_navPane = qml->createRootObject<NavigationPane>();
        if (m_navPane) {
        	m_fwBound = false;
         	m_pForeignWindowControl = m_navPane->findChild<ForeignWindowControl*>("goodCitizenCascadesFW");

         	LayoutUpdateHandler *handler = LayoutUpdateHandler::create(m_pForeignWindowControl)
        	    .onLayoutFrameChanged(this, SLOT(onLayoutFrameChanged(const QRectF &)));


    		// connect ForeignWindowControl signals to slots
    		QObject::connect(m_pForeignWindowControl, SIGNAL(touch(bb::cascades::TouchEvent *)),
    						 m_pGoodCitizen,   SLOT(onTouch(bb::cascades::TouchEvent *)) );

            // Finally the main scene for the application is set the Page.
            Application::instance()->setScene(m_navPane);
        }
    }
}

void GoodCitizenApp::shutdown() {
	qDebug()  << "GoodCitizenApp::shutdown";

	OpenGLView::shutdown();

	Application::instance()->quit();
}

void GoodCitizenApp::onLayoutFrameChanged(const QRectF &layoutFrame) {
	qDebug()  << "GoodCitizenApp::onLayoutFrameChanged: " << m_pForeignWindowControl;

	fprintf(stderr, "fw size: %f,%f %fx%f\n", layoutFrame.x(), layoutFrame.y(), layoutFrame.width(), layoutFrame.height());

	if (m_fwBound == false) {
		m_fwBound = true;

		QString mainWindowGroupId = Application::instance()->mainWindow()->groupId();

		m_pGoodCitizen->setWindowGroup(mainWindowGroupId);
		m_pGoodCitizen->setWindowID("goodCitizenCascadesAppID1");
		m_pGoodCitizen->setPosition(layoutFrame.x(), layoutFrame.y());
		m_pGoodCitizen->setSize(layoutFrame.width(), layoutFrame.height());
		m_pGoodCitizen->add();
		m_pGoodCitizen->setEnabled(true);

		m_pForeignWindowControl->setVisible(true);
	} else {
		m_pGoodCitizen->setAngle(OrientationSupport::instance()->displayDirection());
		m_pGoodCitizen->setPosition(layoutFrame.x(), layoutFrame.y());
		m_pGoodCitizen->setSize(layoutFrame.width(), layoutFrame.height());
		m_pGoodCitizen->setAltered(true);
	}
}
