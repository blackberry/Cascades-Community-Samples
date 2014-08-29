/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
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

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/pim/message/StatusReportType>
#include <bb/pim/message/MimeTypes>
#include <bb/system/SystemToast>

using namespace bb::cascades;
using namespace bb::pim::account;
using namespace bb::pim::message;

static AbstractPane *_root = 0;

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

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
        QObject(app)
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

	_message_service = new MessageService();
	_account_service = new AccountService();

    QSettings settings;
	_phone_number = settings.value(KEY_PHONE_NUMBER,"").toString();


    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	_root = qml->createRootObject<AbstractPane>();

	QObject *mainPage = _root->findChild<QObject*>((const QString) "mainPage");

	logArea = _root->findChild<TextArea*>("logArea");
	destAddr = _root->findChild<TextField*>("destAddr");
	destAddr->setText(_phone_number);

	connect(this, SIGNAL(logMessage(const QString&)), this, SLOT(onLogMessage(const QString&)));

	connect(mainPage, SIGNAL(sendMessage(const QString &, const QString &, const QString &, const QString &)), this, SLOT(sendMessage(const QString &, const QString &, const QString &, const QString &)));

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


    // Set created root object as the application scene
    app->setScene(_root);
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("MmsSample_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

void ApplicationUI::log(const QString & message) {
	emit logMessage(message);
}

void ApplicationUI::hexlog(const QByteArray & data) {
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

void ApplicationUI::onLogMessage(const QString & message) {
	QString current_log_content = logArea->text();
	QString new_log_content = current_log_content;
	new_log_content = new_log_content + message + "\n";
	logArea->setText(new_log_content);
}

void ApplicationUI::sendMessage(const QString & text,const QString & text2, const QString & destination_addr, const QString & file_url) {
	if (destination_addr.length() < 3) {
		return;
	}
	qDebug() << "XXXX sendMessage:" << text << " to " << destination_addr << " with attachment:" << file_url;
	log("Sending message...");

    QSettings settings;
	settings.setValue(KEY_PHONE_NUMBER,destination_addr);

//	First create a conversation, as MMS messages are dealt with as a conversation.
	qDebug() << "XXXX sendMessage: creating Conversation";
	ConversationBuilder* conversation_builder = ConversationBuilder::create();
	conversation_builder->accountId(_sms_account_id);
	QList<MessageContact> participants;

	qDebug() << "XXXX sendMessage: creating MessageContact";
	MessageContact recipient = MessageContact(-1, MessageContact::To, "", destination_addr);
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

	qDebug() << "XXXX sendMessage: setting text attachment 1";
	msg_builder->addAttachment(Attachment("text/plain", "", text));

	if (text2.length() > 0) {
		qDebug() << "XXXX sendMessage: setting text attachment 2";
		msg_builder->addAttachment(Attachment("text/plain", "", text2));
	}


	QUrl file_qurl = QUrl::fromLocalFile(file_url);
	qDebug() << "XXXX sendMessage: adding photo attachment: " << file_qurl.toString();
	msg_builder->addAttachment(Attachment("image/jpeg", "photo.jpg", file_qurl));


	qDebug() << "XXXX sendMessage: recipient=" << recipient.address();
	msg_builder->addRecipient(recipient);

	qDebug() << "XXXX sendMessage: setting conversation ID";
	msg_builder->conversationId(conversation_id);

	msg_builder->subject("This is the subject");

	Message message;
	message = *msg_builder;

// Send it using the SMS MMS account
	qDebug() << "XXXX sendMessage: sending via account id " << _sms_account_id << " : " << message.recipientAt(0).address();
	MessageKey msg_id = _message_service->send(_sms_account_id, message);
	// the next line finalizes the conversation (meaning that the participant list can no longer be changed). This is generally not required but is included here to workaround a known issue in some
	// BlackBerry 10 versions which results in the sent message not being visible in the Hub.
    _message_service -> setStatus(_sms_account_id,conversation_id, ConversationStatus::Finalized);
	log(QString("Sent: message ID:").append(QString::number(msg_id)));


}

// connected to the messageAdded signal
void ApplicationUI::messageReceived(bb::pim::account::AccountKey account_key, bb::pim::message::ConversationKey conv, bb::pim::message::MessageKey message_key) {

	qDebug() << "XXXX messageReceived";

	Message message = _message_service->message(_sms_account_id, message_key);

	qDebug() << "XXXX messageReceived: Message mime type:" << message.mimeType();
	if (message.mimeType() == MimeTypes::Mms) {
		qDebug() << "XXXX messageReceived: It's an MMS!!!";
		if (message.isInbound()) {
			qDebug() << "XXXX messageReceived: sender:" << message.sender().displayableName();
			if (message.attachmentCount() > 0) {
				showToast("MMS received");
				bb::pim::message::Attachment attachment = message.attachmentAt(0);
				log(QString("RCVD: SMS\n").append("RCVD: from " + message.sender().displayableName()).append("\nRCVD: ").append(QString(attachment.data())));
                log(QString("sender displayableName:" + message.sender().displayableName()));
                log(QString("sender name:" + message.sender().name()));
                log(QString("sender address:" + message.sender().address()));
                int num_recipients = message.recipientCount();
                for (int i=0;i<num_recipients;i++) {
                    log(QString("%1 : recipient displayableName:" + message.recipientAt(i).displayableName()).arg(i));
                    log(QString("%1 : recipient name:" + message.recipientAt(i).name()).arg(i));
                    log(QString("%1 : recipient address:" + message.recipientAt(i).address()).arg(i));
                }
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

// expect this to be called for the mms download/retrieval of the "attachment" irrespective of it being deferred one or not
void ApplicationUI::messageUpdated(bb::pim::account::AccountKey accountId, bb::pim::message::ConversationKey conversationId, bb::pim::message::MessageKey message_key,
		bb::pim::message::MessageUpdate data) {

	qDebug() << "XXXX messageUpdated";
	Message message = _message_service->message(_sms_account_id, message_key);

	qDebug() << "XXXX messageUpdated: Message mime type:" << message.mimeType();
	if (message.mimeType() == MimeTypes::Mms) {
		qDebug() << "XXXX messageUpdated: It's an MMS";
		if (message.attachmentCount() > 0) {
			QList<StatusReport> status_reports = message.statusReports();
			QList<StatusReport>::iterator i;
			if (status_reports.size() > 0) {
				showToast("Delivery receipt received");
				for (i = status_reports.begin(); i != status_reports.end(); ++i) {
					log(statusReportTypeName(i->type()));
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
		} else {
			qDebug() << "XXXX messageUpdated: no MMS body; no attachments";
		}
	} else {

	}
}

void ApplicationUI::showToast(QString text) {

	bb::system::SystemToast toast;
	toast.setBody(text);
	toast.setPosition(bb::system::SystemUiPosition::MiddleCenter);
	toast.exec();

}
