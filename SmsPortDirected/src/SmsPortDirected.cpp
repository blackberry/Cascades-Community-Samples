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
#include "SmsPortDirected.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/TextArea>
#include <bb/cascades/TextField>
#include <bb/cascades/AbstractPane>
#include <bb/pim/message/SmsTransport>
#include <bb/pim/message/Message>
#include <bb/pim/message/MessageService>
#include <bb/pim/account/AccountService>
#include <bb/pim/message/Attachment>
#include <bb/system/SystemToast>
#include <stdio.h>
#include <iostream>
#include <QSettings>


using namespace bb::cascades;
using namespace bb::pim::account;
using namespace bb::pim::message;

static AbstractPane *_root = 0;
static QString KEY_PHONE_NUMBER="phone_number";

QString getRegisterResultReceivedName(bb::pim::message::SmsTransportRegistrationResult::Type status) {

	switch (status) {
	case 0:
		return "Unknown";
	case 1:
		return "Port Registered";
	case 2:
		return "Invalid Port";
	case 3:
		return "Port in Use";
	default:
		"Invalid Register Result Status";
	}
}

QString getMessageSendResultReceivedName(long status) {

	switch (status) {
	case bb::pim::message::MessageTransmissionStatus::Unknown:
		return "Unknown";
	case bb::pim::message::MessageTransmissionStatus::Pending:
		return "Pending";
	case bb::pim::message::MessageTransmissionStatus::Sending:
		return "Sending";
	case bb::pim::message::MessageTransmissionStatus::Sent:
		return "Sent";
	case bb::pim::message::MessageTransmissionStatus::DownloadFailed:
		return "DownloadFailed";
	case bb::pim::message::MessageTransmissionStatus::SyncFailed:
		return "SyncFailed";
	case bb::pim::message::MessageTransmissionStatus::FailedToSent:
		return "FailedToSent";
	case bb::pim::message::MessageTransmissionStatus::TooLarge:
		return "TooLarge";
	case bb::pim::message::MessageTransmissionStatus::AttachTooLarge:
		return "AttachTooLarge";
	case bb::pim::message::MessageTransmissionStatus::AcknowledgmentNotReceived:
		return "AcknowledgmentNotReceived";
	case bb::pim::message::MessageTransmissionStatus::BodyDownloadFailedMessageNoLongerExists:
		return "BodyDownloadFailedMessageNoLongerExists";
	case bb::pim::message::MessageTransmissionStatus::BodyDownloadFailedGeneral:
		return "BodyDownloadFailedGeneral";
	default:
		"Invalid Message Send Result Status";
	}
}

SmsPortDirected::SmsPortDirected(bb::cascades::Application *app) :
		QObject(app), _initialised_ok(false), _sms_transport(new bb::pim::message::SmsTransport(this)) {
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	_message_service = new MessageService();
	_account_service = new AccountService();

    QSettings settings;
	_phone_number = settings.value(KEY_PHONE_NUMBER,"").toString();

	qDebug() << "XXXX getting SMS-MMS account";
	QList<Account> account_list = _account_service->accounts(Service::Messages, "sms-mms");
	_sms_account_id = account_list.first().id();
	qDebug() << "XXXX got SMS-MMS account. id=" << _sms_account_id;

	_root = qml->createRootObject<AbstractPane>();
	app->setScene(_root);

	logArea = _root->findChild<TextArea*>("logArea");
	destAddr = _root->findChild<TextField*>("destAddr");
	destAddr->setText(_phone_number);


	QObject *mainPage = _root->findChild<QObject*>((const QString) "mainPage");

	connect(mainPage, SIGNAL(sendMessage(const QString &, const QString &, const int)), this, SLOT(sendMessage(const QString &, const QString &, const int)));

	connect(_sms_transport, SIGNAL(registerResultReceived (unsigned int, bb::pim::message::SmsTransportRegistrationResult::Type)), this,
			SLOT(onRegisterResultReceived(unsigned int, bb::pim::message::SmsTransportRegistrationResult::Type)));

	connect(_sms_transport, SIGNAL(messageSendResultReceived(bb::pim::message::MessageKey)), this, SLOT(onMessageSendResultReceived(bb::pim::message::MessageKey)));

	connect(_sms_transport, SIGNAL(messageReceived (unsigned int, const Message &)), this, SLOT(onMessageReceived(unsigned int, const Message &)));

	connect(this, SIGNAL(logMessage(const QString&)), this, SLOT(onLogMessage(const QString&)));

	qDebug() << "XXXX registering for receiving message on port 16000";
	_sms_transport->registerPort(16000);

}

SmsPortDirected::~SmsPortDirected() {

}

void SmsPortDirected::onRegisterResultReceived(unsigned int port, bb::pim::message::SmsTransportRegistrationResult::Type status) {
	qDebug() << "XXXX onRegisterResultReceived: status=" << status << ":" << getRegisterResultReceivedName(status);
	if (status == SmsTransportRegistrationResult::PortRegistered) {
		log(QString("Port %1 registered OK").arg(port));
	} else {
		log("Failed to register port:" + status);
		log("<b>Error:</b>" + getRegisterResultReceivedName(status));
	}
}

void SmsPortDirected::onMessageSendResultReceived(bb::pim::message::MessageKey message_key) {
	qDebug() << "XXXX onMessageSendResultReceived";
	Message message = _message_service->message(_sms_account_id, message_key);
	long transmission_status = message.transmissionStatus();
	qDebug() << "XXXX transmission_status=" << transmission_status;
	// if status is unknown assume it was sent....
	if (transmission_status == bb::pim::message::MessageTransmissionStatus::Sent || transmission_status == bb::pim::message::MessageTransmissionStatus::Unknown) {
		log(QString("Message sent OK"));
		showToast("Port directed SMS sent OK");
	} else {
		showToast("Port directed SMS sent failed: "+getMessageSendResultReceivedName(transmission_status));
	}
}

void SmsPortDirected::onMessageReceived(unsigned int port, const Message& message) {
	qDebug() << "XXXX onMessageReceived: port=" << port;
	showToast("Received port directed SMS");
	bb::pim::message::Attachment attachment = message.attachmentAt(0);
	hexlog(attachment.data());
}

void SmsPortDirected::sendMessage(const QString & text, const QString & destination_addr, const int port) {

	if (destination_addr.length() < 3) {
		return;
	}

	qDebug() << "XXXX sendMessage:" << text << " to " << destination_addr << " using port " << port;

    QSettings settings;
	settings.setValue(KEY_PHONE_NUMBER,destination_addr);

	MessageBuilder* msg_builder = _sms_transport->createMessageBuilder();
	msg_builder->addAttachment(Attachment("text/plain", "", text));
	MessageContact recipient = MessageContact(-1, MessageContact::To, destination_addr, destination_addr);
	msg_builder->addRecipient(recipient);

	Message message;
	message = *msg_builder;

	_sms_transport->send(port, message);

}

void SmsPortDirected::log(const QString & message) {
	qDebug() << message;
	emit logMessage(message);
}

void SmsPortDirected::hexlog(const QByteArray & data) {
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

void SmsPortDirected::onLogMessage(const QString & message) {
	qDebug() << "XXXX onLogMessage:[" << message << "]";
	QString current_log_content = logArea->text();
	if (!current_log_content.startsWith("<html><p>")) {
		current_log_content = "<html><p>" + current_log_content + "</p>";
	}
	// remove </html>
	QString new_log_content = current_log_content.mid(0, current_log_content.length() - 7);
	new_log_content = new_log_content + "<p>" + message + "</p>";
	new_log_content = new_log_content + "</html>";
	qDebug() << "XXXX onLogMessage: [" << new_log_content << "]";
	logArea->setText(new_log_content);
}

void SmsPortDirected::showToast(QString text) {

	bb::system::SystemToast toast;
	toast.setBody(text);
	toast.setPosition(bb::system::SystemUiPosition::MiddleCenter);
	toast.exec();

}
