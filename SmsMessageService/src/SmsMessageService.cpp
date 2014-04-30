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
#include "SmsMessageService.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/TextArea>
#include <bb/cascades/TextField>
#include <bb/pim/account/Provider>
#include <bb/pim/account/Result>
#include <bb/pim/message/MessageService>
#include <bb/pim/message/Message>
#include <bb/pim/message/MessageBuilder>
#include <bb/pim/message/ConversationBuilder>
#include <bb/pim/message/Keys>
#include <bb/pim/message/StatusReport>
#include <bb/pim/message/MessageUpdateType>
#include <bb/pim/message/MimeTypes>
#include <bb/system/SystemToast>
#include <QSettings>

using namespace bb::cascades;
using namespace bb::pim::account;
using namespace bb::pim::message;

static AbstractPane *_root = 0;
const char *TEXT = "text/*";
const char *TEXT_HTML = "text/html";
const char *TEXT_PLAIN = "text/plain";

static QString KEY_PHONE_NUMBER="phone_number";


QString statusReportTypeName(bb::pim::message::StatusReportType::Type type) {

	switch (type) {
	case bb::pim::message::StatusReportType::ReadReport:
		return "ReadReport";
	case bb::pim::message::StatusReportType::DeliveryReport:
		return "DeliveryReport";
	case bb::pim::message::StatusReportType::ErrorReport:
		return "ErrorReport";
	default:
		"Invalid Status Report Type";
	}
}

QString statusDescription(int status) {

	switch (status) {
	case 0:
		return "Short message received by the SME";
	case 1:
		return "Short message forwarded by the SC to the SME but the SC is unable to confirm delivery";
	case 2:
		return "Short message replaced by the SC";
	case 32:
		return "Congestion";
	case 33:
		return "SME Busy";
	case 34:
		return "No response from SME";
	case 35:
		return "Service rejected";
	case 36:
		return "Quality of service not available";
	case 37:
		return "Error in SME";
	case 64:
		return "Remote procedure error";
	case 65:
		return "Incompatible destination";
	case 66:
		return "Connection rejected by SME";
	case 67:
		return "Not obtainable";
	case 68:
		return "Quality of service not available";
	case 69:
		return "No interworking available";
	case 70:
		return "SM Validity Period Expired";
	case 71:
		return "SM Deleted by originating SME";
	case 72:
		return "SM Deleted by SC Administration";
	case 73:
		return "SM does not exist";
	case 96:
		return "Congestion";
	case 97:
		return "SME Busy";
	case 98:
		return "No response from SME";
	case 99:
		return "Service rejected";
	case 100:
		return "Quality of service not available";
	case 101:
		return "Error in SME";
	case 129:
		// the value of 129 for "delivered" is really for MMS but also used for SMS in the current BlackBerry 10 APIs
		return "Short message received by the SME";
	}

	if ((status >=3 && status <= 15)||(status >=38 && status <= 47)||(status >=74 && status <= 79)||(status >=102 && status <= 111)) {
		return "Service rejected";
	}

	return "SC specific status";
}


SmsMessageService::SmsMessageService(bb::cascades::Application *app) :
		QObject(app), _initialised_ok(false) {

	_message_service = new MessageService();
	_account_service = new AccountService();

    QSettings settings;
	_phone_number = settings.value(KEY_PHONE_NUMBER,"").toString();

	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	_root = qml->createRootObject<AbstractPane>();

	QObject *mainPage = _root->findChild<QObject*>((const QString) "mainPage");

	logArea = _root->findChild<TextArea*>("logArea");
	destAddr = _root->findChild<TextField*>("destAddr");
	destAddr->setText(_phone_number);

	connect(this, SIGNAL(logMessage(const QString&)), this, SLOT(onLogMessage(const QString&)));

	connect(mainPage, SIGNAL(sendMessage(const QString &, const QString &)), this, SLOT(sendMessage(const QString &, const QString &)));

	connect(_message_service, SIGNAL(
			messageAdded(bb::pim::account::AccountKey, bb::pim::message::ConversationKey, bb::pim::message::MessageKey)), SLOT(
			messageReceived(bb::pim::account::AccountKey, bb::pim::message::ConversationKey, bb::pim::message::MessageKey)));

	connect(_message_service, SIGNAL(
			messageUpdated(bb::pim::account::AccountKey, bb::pim::message::ConversationKey, bb::pim::message::MessageKey, bb::pim::message::MessageUpdate)), SLOT(
			messageUpdated(bb::pim::account::AccountKey, bb::pim::message::ConversationKey, bb::pim::message::MessageKey, bb::pim::message::MessageUpdate)));

	qDebug() << "XXXX getting SMS-MMS account";
	QList<Account> account_list = _account_service->accounts(Service::Messages, "sms-mms");
	_sms_account_id = account_list.first().id();
	qDebug() << "XXXX got SMS-MMS account. id=" << _sms_account_id;

	app->setScene(_root);

}

SmsMessageService::~SmsMessageService() {

}

void SmsMessageService::log(const QString & message) {
	qDebug() << message;
	emit logMessage(message);
}

void SmsMessageService::hexlog(const QByteArray & data) {
	int data_len = data.length();
	for (int i = 0; i < data_len; i += 16) {
		QString hex;
		int segment_len;
		if (i + 16 < data_len) {
			segment_len = 16;
		} else {
			segment_len = data_len - i;
		}
		QByteArray hex16 = data.mid(i, segment_len);
		QString line(hex16.toHex());
		line = line.toUpper();
		log(line);
	}
}

void SmsMessageService::onLogMessage(const QString & message) {
	qDebug() << "XXXX onLogMessage:[" << message << "]";
	QString current_log_content = logArea->text();
	QString new_log_content = current_log_content;
	new_log_content = new_log_content + message + "\n";
	qDebug() << "XXXX onLogMessage: [" << new_log_content << "]";
	logArea->setText(new_log_content);
}

void SmsMessageService::sendMessage(const QString & text, const QString & destination_addr) {
	if (destination_addr.length() < 3) {
		return;
	}
	qDebug() << "XXXX sendMessage:" << text << " to " << destination_addr;
	log("Sending message...");

    QSettings settings;
	settings.setValue(KEY_PHONE_NUMBER,destination_addr);

//	First create a conversation, as SMS messages are dealt with as a conversation.
	qDebug() << "XXXX sendMessage: creating Conversation";
	ConversationBuilder* conversation_builder = ConversationBuilder::create();
	conversation_builder->accountId(_sms_account_id);
	QList<MessageContact> participants;

	qDebug() << "XXXX sendMessage: creating MessageContact";
	MessageContact recipient = MessageContact(-1, MessageContact::To, "Wilma Flintstone", destination_addr);
	qDebug() << "XXXX sendMessage: adding recipient";
	participants.append(recipient);
// indicate who the conversation is with
	conversation_builder->participants(participants);

//obtain the conversation and save it
	Conversation conversation = *conversation_builder;
	ConversationKey conversation_id = _message_service->save(_sms_account_id, conversation);

// make a Message object
	qDebug() << "XXXX sendMessage: creating MessageBuilder";
	MessageBuilder* msg_builder = MessageBuilder::create(_sms_account_id);

	qDebug() << "XXXX sendMessage: setting body";
	msg_builder->addAttachment(Attachment("text/plain", "", text));
	qDebug() << "XXXX sendMessage: recipient=" << recipient.address();
	msg_builder->addRecipient(recipient);

	qDebug() << "XXXX sendMessage: setting conversation ID";
	msg_builder->conversationId(conversation_id);

	Message message;
	message = *msg_builder;

// Send it using the SMS MMS account
	qDebug() << "XXXX sendMessage: sending via account id " << _sms_account_id << " : " << message.recipientAt(0).address();
	MessageKey msg_id = _message_service->send(_sms_account_id, message);
    // the next line finalizes the conversation (meaning that the participant list can no longer be changed). This is generally not required but is included here to workaround a known issue in some
    // BlackBerry 10 versions which results in the sent message not being visible in the Hub.
    _message_service -> setStatus(_sms_account_id,conversation_id, ConversationStatus::Finalized);
	log(QString("Sent: message ID:").append(QString::number(msg_id)));

	showToast("SMS sent OK");

}

void SmsMessageService::messageReceived(bb::pim::account::AccountKey account_key, bb::pim::message::ConversationKey conv, bb::pim::message::MessageKey message_key) {

	qDebug() << "XXXX messageReceived";

	Message message = _message_service->message(_sms_account_id, message_key);

	qDebug() << "XXXX Message mime type:" << message.mimeType();
	if (message.mimeType() == MimeTypes::Sms) {
		qDebug() << "XXXX messageReceived: It's an SMS!!!";
		if (message.isInbound()) {
			qDebug() << "XXXX sender:" << message.sender().displayableName();
			if (message.attachmentCount() > 0) {
				showToast("SMS received");
				bb::pim::message::Attachment attachment = message.attachmentAt(0);
				log(QString("RCVD: SMS\n").append("RCVD: from " + message.sender().displayableName()).append("\nRCVD: ").append(QString(attachment.data())));
			} else {
				qDebug() << "XXXX messageReceived: no attachments";
			}
		} else {
			qDebug() << "XXXX messageReceived: not inbound";
		}
	}  else {
		qDebug() << "XXXX messageReceived: not an SMS";
	}
}

void SmsMessageService::messageUpdated(bb::pim::account::AccountKey accountId, bb::pim::message::ConversationKey conversationId, bb::pim::message::MessageKey message_key,
		bb::pim::message::MessageUpdate data) {

	qDebug() << "XXXX messageUpdated";
	Message message = _message_service->message(_sms_account_id, message_key);

	qDebug() << "XXXX Message mime type:" << message.mimeType();
	if (message.mimeType() == MimeTypes::Sms) {
		qDebug() << "XXXX messageUpdated: It's an SMS!!!";
		if (message.attachmentCount() > 0) {
			QList<StatusReport> status_reports = message.statusReports();
			QList<StatusReport>::iterator i;
			if (status_reports.size() > 0) {
				showToast("Delivery receipt received");
				for (i = status_reports.begin(); i != status_reports.end(); ++i) {
					log(statusReportTypeName(i->type()));
//			qDebug() << "XXXX status report: " << statusReportTypeName(i->type());
					if (i->type() == bb::pim::message::StatusReportType::DeliveryReport) {
						log(QString("Delivery Receipt --------"));
						log(i->address());
						log(i->date().toString("dd.MM.yyyy hh:mm:ss"));
						log(QString::number(i->status()).append(":").append(statusDescription(i->status())));
						log(QString("Delivery Receipt ++++++++"));
					}
				}
			}
			bb::pim::message::Attachment attachment = message.attachmentAt(0);
			log(QString(attachment.data()));
		} else {
			qDebug() << "XXXX messageUpdated: no SMS body; no attachments";
		}
	} else {
		qDebug() << "XXXX messageUpdated: not an SMS message, no Attachments";
	}
}

void SmsMessageService::showToast(QString text) {

	bb::system::SystemToast toast;
	toast.setBody(text);
	toast.setPosition(bb::system::SystemUiPosition::MiddleCenter);
	toast.exec();

}
