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

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <bb/pim/account/AccountService>
#include <bb/pim/account/Account>
#include <bb/pim/message/Keys>
#include <bb/pim/message/MessageUpdate>
#include <bb/pim/message/MessageService>
#include <bb/pim/message/MessageBuilder>
#include <bb/pim/message/ConversationBuilder>
#include <bb/cascades/TextArea>
#include <bb/cascades/TextField>

namespace bb {
namespace cascades {
class Application;
class TextArea;
class TextField;
}
}
namespace bb {
namespace pim {
namespace account {
class AccountService;
class Account;
}
}
}
namespace bb {
namespace pim {
namespace message {
class MessageService;
class MessageBuilder;
class Keys;
class MessageUpdate;
}
}
}

namespace bb {
namespace cascades {
class Application;
class LocaleHandler;
}
}

class QTranslator;

/*!
 * @brief Application object
 *
 *
 */

class ApplicationUI: public QObject {
Q_OBJECT
public:
	ApplicationUI(bb::cascades::Application *app);
	virtual ~ApplicationUI() {
	}

signals:
	void logMessage(const QString& message);

public slots:
	void onLogMessage(const QString& message);
	void sendMessage(const QString & text, const QString & text2, const QString & destination_addr, const QString & attachment_url);
	void messageReceived(bb::pim::account::AccountKey account,
			bb::pim::message::ConversationKey conv,
			bb::pim::message::MessageKey message);
	void messageUpdated(bb::pim::account::AccountKey accountId,
			bb::pim::message::ConversationKey conversationId,
			bb::pim::message::MessageKey messageId,
			bb::pim::message::MessageUpdate data);
private slots:
	void onSystemLanguageChanged();

private:
	QTranslator* m_pTranslator;
	bb::cascades::LocaleHandler* m_pLocaleHandler;

	void log(const QString & message);
	void hexlog(const QByteArray & data);
	void showToast(QString text);
	bb::cascades::TextArea * logArea;
	bb::cascades::TextField * destAddr;

	bb::pim::account::AccountService* _account_service;
	bb::pim::message::MessageService* _message_service;
	QString _phone_number;
	int _sms_account_id;

	bool _initialised_ok;

};

#endif /* ApplicationUI_HPP_ */
