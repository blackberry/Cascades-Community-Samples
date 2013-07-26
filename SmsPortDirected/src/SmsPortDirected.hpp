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
#ifndef SmsPortDirected_HPP_
#define SmsPortDirected_HPP_

#include <QObject>
#include <bb/pim/account/AccountService>
#include <bb/pim/message/MessageService>
#include <bb/pim/message/SmsTransport.hpp>
#include <bb/pim/message/SmsTransportRegistrationResult.hpp>
#include <bb/pim/message/SmsTransportSendResult.hpp>

namespace bb { namespace cascades { class Application; class TextArea; class TextField; }}
namespace bb { namespace pim { namespace account {class AccountService; class Account; }}}

using namespace bb::pim::message;

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class SmsPortDirected : public QObject
{
    Q_OBJECT
public:
    SmsPortDirected(bb::cascades::Application *app);
    virtual ~SmsPortDirected();

	signals:
	void logMessage(const QString & message);

public slots:
	void onMessageReceived(unsigned int, const Message &);
	void onMessageSendResultReceived(bb::pim::message::MessageKey message_key);
	void onRegisterResultReceived(unsigned int port, bb::pim::message::SmsTransportRegistrationResult::Type status );
	void onLogMessage(const QString & message);
	void sendMessage(const QString & text, const QString & destination_addr, const int port);

private:
	void log(const QString & message);
	void hexlog(const QByteArray & data);
	void showToast(QString text);
    bb::pim::account::AccountService* _account_service;
    bb::pim::message::MessageService* _message_service;
    QString _phone_number;
    int _sms_account_id;
	bb::pim::message::SmsTransport* _sms_transport;
    bb::cascades::TextArea * logArea;
	bb::cascades::TextField * destAddr;

    bool _initialised_ok;

};

#endif /* SmsPortDirected_HPP_ */
