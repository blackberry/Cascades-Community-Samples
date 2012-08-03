/* Copyright (c) 2012 Research In Motion Limited.
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

#include "app.hpp"

#include <iostream>
using std::cout;
using std::endl;

App::App() {
	QmlDocument *qml = QmlDocument::create("main.qml");
	//-- setContextProperty expose C++ object in QML as an variable
	//-- uncomment next line to introduce 'this' object to QML name space as an 'app' variable
	//qml->setContextProperty("app", this);

	root = qml->createRootNode<AbstractPane>();

	qml->setContextProperty("_app", this);
	Application::setScene(root);
}

void App::invoke(int targetType, const QString &action, const QString &mime, const QString &uri, const QString &data, const QString &target) {
	iManager = new InvokeManager(this);

	InvokeRequest iRequest;

	if (action != 0 && action.length() > 0) {
		printf("################");
		printf(action.toAscii().constData());
		if (action != QString("All") && action != QString("Menu Actions")) {
			iRequest.setAction(action);
		} else if(target.length() <= 0){
			root->findChild<QObject*>("qlDialogMessage")->setProperty("text", "[" + action + "] is not a valid action type for an unbound invocation.");
			root->findChild<QObject*>("qcdDialog")->setProperty("visible", true);
			return;
		}
	}
	if (mime != 0 && mime.length() > 0) {
		printf("################");
		printf(mime.toAscii().constData());
		iRequest.setMimeType(mime);
	} else {
		root->findChild<QObject*>("qlDialogMessage")->setProperty("text", "MIME type must be specified!");
		root->findChild<QObject*>("qcdDialog")->setProperty("visible", true);
		return;
	}
	if (uri != 0 && uri.length() > 0) {
		printf("################");
		printf(uri.toAscii().constData());
		iRequest.setUri(uri);
	}
	if (data != 0 && data.length() > 0) {
		printf("################");
		printf(data.toAscii().constData());
		iRequest.setData(data.toUtf8());
	}
	if (target != 0 && target.length() > 0) {
		printf("################");
		printf(target.toAscii().constData());
		iRequest.setTarget(target);
	}

	iReply = iManager->invoke(iRequest);
	QObject::connect(iReply, SIGNAL(finished()), this, SLOT(processInvokeReply()));
	if (iReply == NULL) {
		root->findChild<QObject*>("qlDialogMessage")->setProperty("text", "Invoke Failed! InvokeReply is NULL.");
		root->findChild<QObject*>("qcdDialog")->setProperty("visible", true);
		return;
	}
}

void App::processInvokeReply() {
	switch (iReply->error()) {
	case InvokeReply::ErrorBadRequest:
		root->findChild<QObject*>("qlDialogMessage")->setProperty("text", "[ErrorBadRequest] Invoke Failed!");
		root->findChild<QObject*>("qcdDialog")->setProperty("visible", true);
		break;
	case InvokeReply::ErrorInternal:
		root->findChild<QObject*>("qlDialogMessage")->setProperty("text", "[ErrorInternal] Invoke Failed!");
		root->findChild<QObject*>("qcdDialog")->setProperty("visible", true);
		break;
	case InvokeReply::ErrorNoTarget:
		root->findChild<QObject*>("qlDialogMessage")->setProperty("text", "[ErrorNoTarget] Invoke Failed!");
		root->findChild<QObject*>("qcdDialog")->setProperty("visible", true);
		break;
	case InvokeReply::ErrorTarget:
		root->findChild<QObject*>("qlDialogMessage")->setProperty("text", "[ErrorTarget] Invoke Failed.");
		root->findChild<QObject*>("qcdDialog")->setProperty("visible", true);
		break;
	}
}

void App::query(int targetType, const QString& action, const QString& mime, const QString& uri) {
	iManager = new InvokeManager(this);

	InvokeQueryTargetsRequest iQuery;

	if (targetType == 0) {
		iQuery.setTargetType(InvokeTarget::All);
	} else if (targetType == 1) {
		iQuery.setTargetType(InvokeTarget::Application);
	} else if (targetType == 2) {
		iQuery.setTargetType(InvokeTarget::Viewer);
	} else if (targetType == 3) {
		iQuery.setTargetType(InvokeTarget::Service);
	}

	if (action != 0 && action.length() > 0) {
		printf("################");
		printf(action.toAscii().constData());
		if (action.compare("All") == 0) {
			iQuery.setActionType(InvokeAction::All);
		} else if (action.compare("Menu Actions") == 0) {
			iQuery.setActionType(InvokeAction::Menu);
		} else {
			iQuery.setAction(action);
		}
	}
	if (mime != 0 && mime.length() > 0) {
		printf("################");
		printf(mime.toAscii().constData());
		iQuery.setMimeType(mime);
	} else if (uri.length() <= 0) {
		root->findChild<QObject*>("qlDialogMessage")->setProperty("text", "MIME type OR URI must be specified!");
		root->findChild<QObject*>("qcdDialog")->setProperty("visible", true);
		return;
	}

	if (uri != 0 && uri.length() > 0) {
		printf("################");
		printf(uri.toAscii().constData());
		iQuery.setUri(uri);
	} else if (mime.length() <= 0) {
		root->findChild<QObject*>("qlDialogMessage")->setProperty("text", "MIME type OR URI must be specified!");
		root->findChild<QObject*>("qcdDialog")->setProperty("visible", true);
		return;
	}

	iQueryReply = iManager->queryTargets(iQuery);
	QObject::connect(iQueryReply, SIGNAL(finished()), this, SLOT(processQueryReply()));
}

void App::processQueryReply() {
	if (iQueryReply->error() == InvokeReply::ErrorNone) {
		QList<InvokeAction> actions = iQueryReply->actions();
		Container* resultsContainer = root->findChild<Container*>("qcQueryResults");
		resultsContainer->removeAll();
		QString result = "";

		for (int i = 0; i < actions.count(); i++) {
			InvokeAction action = actions.at(i);
			QList<InvokeTarget> targets = action.targets();
			for (int j = 0; j < targets.count(); j++) {
				InvokeTarget target = targets.at(j);
				Button *button = new Button(resultsContainer);
				button->setImageSource(target.icon());
				button->setText(target.name());
				StackLayoutProperties* layoutProperties = new StackLayoutProperties();
				layoutProperties->setHorizontalAlignment(HorizontalAlignment::Center);
				button->setLayoutProperties(layoutProperties);
				resultsContainer->add(button);

				connect(button, SIGNAL(clicked()), this, SLOT(handleInvokeButtonClick()));
			}
		}
	}
	switch (iQueryReply->error()) {
	case InvokeReply::ErrorBadRequest:
		root->findChild<QObject*>("qlDialogMessage")->setProperty("text", "[ErrorBadRequest] Query Failed!");
		root->findChild<QObject*>("qcdDialog")->setProperty("visible", true);
		break;
	case InvokeReply::ErrorInternal:
		root->findChild<QObject*>("qlDialogMessage")->setProperty("text", "[ErrorInternal] Query Failed!");
		root->findChild<QObject*>("qcdDialog")->setProperty("visible", true);
		break;
	case InvokeReply::ErrorNoTarget:
		root->findChild<QObject*>("qlDialogMessage")->setProperty("text", "[ErrorNoTarget] Query Failed!");
		root->findChild<QObject*>("qcdDialog")->setProperty("visible", true);
		break;
	case InvokeReply::ErrorTarget:
		root->findChild<QObject*>("qlDialogMessage")->setProperty("text", "[ErrorTarget] Query Failed.");
		root->findChild<QObject*>("qcdDialog")->setProperty("visible", true);
		break;
	}
}

void App::handleInvokeButtonClick() {
	Button *button = dynamic_cast<Button*>(QObject::sender());

	DropDown* qddAction = root->findChild<DropDown*>("qddAction");
	QString action;
	if(qddAction->selectedIndex() != 5){
		action = qddAction->at(qddAction->selectedIndex())->text();
	} else{
		action = root->findChild<QObject*>("qtaAction")->property("text").toString();
	}

	QString mime = root->findChild<QObject*>("qtaMimeType")->property("text").toString();

	QString uri = root->findChild<QObject*>("qtaURI")->property("text").toString();

	QString data = root->findChild<QObject*>("qtaData")->property("text").toString();

	QString target = button->text();

	invoke(0, action, mime, uri, data, target);
}

