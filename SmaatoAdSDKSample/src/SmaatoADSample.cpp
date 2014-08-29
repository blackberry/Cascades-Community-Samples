/*
 * Copyright (c) 2014 BlackBerry Limited.
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
 
#include "SmaatoADSample.hpp"
#include "SSmaatoAd.h"
#include "SSmaatoAdView.h"
#include "SSmaatoAPI.h"
#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/PpsObject>
#include <bb/cascades/Container>
#include <bb/cascades/StackLayout>
#include <bb/cascades/StackLayoutProperties>
#include <bb/cascades/LayoutOrientation>
#include <bb/cascades/Button>

using namespace bb::cascades;
using namespace bb::system;

SmaatoADSample::SmaatoADSample(bb::cascades::Application *app) :
		QObject(app) {
	// create scene document from main.qml asset
	// set parent to created document to ensure it exists for the whole application lifetime
	qmlRegisterType<SSmaatoAdView>("smaatosdk", 1, 0, "SSmaatoAdView");
	qmlRegisterType<SSmaatoAPI>("smaatoapi", 1, 0, "SSmaatoAPI");
    initFullUI();
}


//! [1]

void SmaatoADSample::initFullUI() {
	QmlDocument *qml = QmlDocument::create("asset:///main.qml");
	qml->setContextProperty("_app", this);
	AbstractPane *root = qml->createRootObject<AbstractPane>();
	Application::instance()->setScene(root);

    Container *cppAdContainter = root->findChild<Container*>("_cppAdContainer");

	SSmaatoAdView* _adView = new SSmaatoAdView();
	_adView->setAdSpaceId("0"); //Add your AdSpace ID here
	_adView->setPublisherId("0"); //Add your Publisher ID here
	_adView->setAdViewSize(SSmaatoAdView::AdViewSizeNormal);
	_adView->setFormat(1);
	_adView->setCoppa(0);
	_adView->setGender(SSmaatoAPI::Male);
	_adView->setAge(30);
	_adView->setTags("News,Sports");
	_adView->setZip("90210");
	_adView->setAutoRefreshPeriod(30);
    _adView->setScale(2.4,2.4);
    _adView->setPreferredWidth(720);
    _adView->setTranslationY(40);
	_adView->updateAd();
	cppAdContainter->add(_adView);
}
