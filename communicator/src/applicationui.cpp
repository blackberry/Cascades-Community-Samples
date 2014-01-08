/* Copyright (c) 2013 BlackBerry Limited.
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

#include "applicationui.hpp"

#include "soundmanager.h"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/data/DataSource>
#include <bb/system/phone/Phone>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeTargetReply>
#include <bb/system/SystemToast>
#include <bb/pim/message/ConversationBuilder>
#include <bb/pim/message/MessageBuilder>
#include <bb/pim/message/MessageService>
#include <bb/pim/message/MessageContact>
#include <bb/pim/account/AccountService>
#include <bb/PpsObject>
#include <bb/system/phone/Call>
#include <bb/cascades/TextArea>
#include <bb/cascades/TextField>

using namespace bb::cascades;
using namespace bb::data;
using namespace bb::system;
using namespace bb::pim::account;
using namespace bb::pim::message;
using namespace bb::system::phone;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
		QObject(app) {
	// prepare the localization
	m_pTranslator = new QTranslator(this);
	m_pLocaleHandler = new LocaleHandler(this);
	if (!QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()),
			this, SLOT(onSystemLanguageChanged()))) {
		// This is an abnormal situation! Something went wrong!
		// Add own code to recover here
		qWarning() << "Recovering from a failed connect()";
	}
	// initial load
	onSystemLanguageChanged();

	qmlRegisterType<phone::Phone>("bb.system.phone", 1, 0, "Phone");
	DataSource::registerQmlTypes();

	// Create scene document from main.qml asset, the parent is set
	// to ensure the document gets destroyed properly at shut down.
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	//Sharing Object With QML
	qml->setContextProperty("communicatorApp", this);

	//SoundManager Initialization
	mSoundManager = new SoundManager("audio/wav/");

	// Create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();

	//get Reference to the Call Info Text Area in Main.qml
	m_CallInfoTxtArea = root->findChild<TextArea*>(QString("CallInfoTxt"));
	//get Reference to the Phone Number Text Field in Main.qml
	m_PhoneNumberTxtField = root->findChild<TextField*>(
			QString("phoneNumbertxt"));

	// Set created root object as the application scene
	app->setScene(root);
}

void ApplicationUI::onSystemLanguageChanged() {
	QCoreApplication::instance()->removeTranslator(m_pTranslator);
	// Initiate, load and install the application translation files.
	QString locale_string = QLocale().name();
	QString file_name = QString("communicator_%1").arg(locale_string);
	if (m_pTranslator->load(file_name, "app/native/qm")) {
		QCoreApplication::instance()->installTranslator(m_pTranslator);
	}
}

void ApplicationUI::InvokeSmsComposer(QString PhoneNumber) {
	InvokeRequest cardrequest;
	//cardrequest.setTarget("sys.pim.text_messaging.composer"); - not working on WorkSpace perimeter
	//cross-perimeter invoke
	cardrequest.setTarget("sys.pim.text_messaging.smsuri");
	cardrequest.setAction("bb.action.SENDTEXT");
	cardrequest.setUri("tel:" + PhoneNumber);
	InvokeManager invokemanager;
	InvokeTargetReply *reply = invokemanager.invoke(cardrequest);
	if (reply) {
		connect(reply, SIGNAL(finished()), this, SLOT(OnInvokeFinished()));
	} else {
		qDebug() << "Invocation Connecting Failed!!";
	}
}

void ApplicationUI::OnInvokeFinished() {
	InvokeTargetReply *reply = qobject_cast<InvokeTargetReply*>(sender());
	if (reply->error()) {
		qDebug() << "Invoke Failed - Do something";
	}
	reply->deleteLater();
}

void ApplicationUI::PlaySound(const QString &msg) {
	// Play sound determined by QString parameter
	mSoundManager->play(msg);
}

void ApplicationUI::SendSMSMessage(QString ToStr, QString BodyStr) {

	MessageService messageService;
	AccountService accountService;

	//Get the SMS/MMS account
	QList<Account> accountList = accountService.accounts(Service::Messages,
			"sms-mms");
	AccountKey accountId = accountList.first().id();

	//Create a conversation  because sms/mms chats most of the time is a conversation
	ConversationBuilder* conversationBuilder = ConversationBuilder::create();
	conversationBuilder->accountId(accountId);

	// Create a contact to whom you want to send sms/mms.
	int contactKey = -1;
	MessageContact recipient = MessageContact(contactKey, MessageContact::To,
			ToStr, ToStr);
	QList<MessageContact> participants;
	participants.append(recipient);

	//Add the contact to the conversation
	conversationBuilder->participants(participants);
	Conversation conversation = *conversationBuilder;
	ConversationKey conversationId = messageService.save(accountId,
			conversation);

	//Create a message Builder for sms/mms
	MessageBuilder* messageBuilder = MessageBuilder::create(accountId);
	messageBuilder->conversationId(conversationId);
	messageBuilder->addRecipient(recipient);
	messageBuilder->addAttachment(
			Attachment("text/plain", "body.txt", BodyStr));
	Message message;
	message = *messageBuilder;

	//Sending SMS/MMS
	messageService.send(accountId, message);
	ShowToast("SMS sent successfully");
}

void ApplicationUI::ShowToast(QString Msg) {
	using namespace bb::system;

	SystemToast *toast = new SystemToast(this);

	toast->setBody(Msg);
	toast->show();
}

void ApplicationUI::onCallUpdated(const Call &call) {
	QMetaObject MetaCallTypeObject = CallType::staticMetaObject;
	QMetaEnum CallTypeEnum = MetaCallTypeObject.enumerator(
			MetaCallTypeObject.indexOfEnumerator("Type"));

	QMetaObject MetaCallStateObject = CallState::staticMetaObject;
	QMetaEnum CallStateEnum = MetaCallStateObject.enumerator(
			MetaCallStateObject.indexOfEnumerator("Type"));

	CallType::Type CurrentCallType = call.callType();
	CallState::Type CurrentCallState = call.callState();

	//qDebug() << "Call ID : " << call.callId();
	//qDebug() << "Call Type : " << CallTypeEnum.valueToKey(CurrentCallType);
	//qDebug() << "Call State : " << CallStateEnum.valueToKey(CurrentCallState);

	if (m_CallInfoTxtArea) { //Check the reference is not NULL
		//Update the GUI
		QString CallStatusStr = m_CallInfoTxtArea->text();
		CallStatusStr = CallStatusStr + "Call [ID:" + QString::number(call.callId());
		if(m_PhoneNumberTxtField){
			CallStatusStr = CallStatusStr + ",Number:" 	+ m_PhoneNumberTxtField->text();
		}
		CallStatusStr = CallStatusStr + ",Type:" 	+ CallTypeEnum.valueToKey(CurrentCallType);
		CallStatusStr = CallStatusStr + ",State:"	+ CallStateEnum.valueToKey(CurrentCallState) + "]\n";

		qDebug() << "Call Details : " << CallStatusStr;

		m_CallInfoTxtArea->setText(CallStatusStr);
	}
}
