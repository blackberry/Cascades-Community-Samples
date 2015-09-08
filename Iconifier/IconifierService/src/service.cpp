/* Copyright (c) 2015 BlackBerry Ltd.
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
 *
 */

#include "service.hpp"

#include <bb/Application>
#include <bb/system/InvokeManager>


using namespace bb::system;

Service::Service() :
        QObject(),
        m_invokeManager(new InvokeManager(this)),
        m_Settings(new QSettings("MSohm_EntSol", "Iconifier")),
        m_accountService(new bb::pim::account::AccountService())
{
    m_invokeManager->connect(m_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)),
            this, SLOT(handleInvoke(const bb::system::InvokeRequest&)));

}

void Service::handleInvoke(const bb::system::InvokeRequest & request)
{

    //Reset saved accounts.
    if (request.action().compare("com.example.IconifierService.RESET") == 0)
    {
        m_Settings->clear();
    }


    qDebug() << "Checking Message accounts.";
    checkAllAccounts(bb::pim::account::Service::Messages);

    qDebug() << "Checking Phone accounts.";
    checkAllAccounts(bb::pim::account::Service::Phone);

}

void Service::checkAllAccounts(bb::pim::account::Service::Type accountType)
{

    qDebug() << "checkAllAccounts starting.";


    //Get all the messaging accounts.
    const QList<bb::pim::account::Account> accounts = m_accountService->accounts(accountType);

    QUrl iconURL;

    bb::platform::HomeScreen homeScreen;

    // Iterate over the list of accounts
    foreach (const bb::pim::account::Account &account, accounts)
    {

        //Configure icons
        if (accountType == bb::pim::account::Service::Messages)
        {
            iconURL = QUrl("asset:///EmailIcon.png");
        }
        else
        {
            iconURL = QUrl("asset:///PhoneIcon.png");
        }

        qDebug() << "Account ID: " << account.id();
        qDebug() << "Account Provider: " << account.provider().name();
        qDebug() << "Account Display Name: " << account.displayName();

        QString accountID = QString::number(account.id());

        //Verify if an icon was already added.
        if (!m_Settings->contains(accountID))
        {
            //Save accountID in QSettings to avoid duplicate icons.
            m_Settings->setValue(accountID, true);

            QString iconTitle;

            //Get a non empty title.
            if (account.displayName().length() > 0)
            {
                iconTitle = account.displayName();
            }
            else
            {
                iconTitle = account.provider().name();
            }

            //Rename Pin to Pin to PIM Messages
            if (iconTitle.contains("Pin"))
            {
                iconTitle = "PIN Messages";
                iconURL = QUrl("asset:///PinIcon.png");
            }
            else if (iconTitle.contains("sms"))
            {
                iconTitle = "Text Messages";
                iconURL = QUrl("asset:///SmsIcon.png");
            }

            QUrl uri = QUrl("pim:application/vnd.blackberry.hub:" + accountID);

            qDebug() << "New account to add: " << accountID << "with name:" << iconTitle << " and URI:" << uri;

            homeScreen.addShortcut(iconURL, iconTitle, uri);

        }
        else
        {
            qDebug() << "Skipping account: " << accountID;
        }
    }

    qDebug() << "checkAllAccounts finished.";

}
