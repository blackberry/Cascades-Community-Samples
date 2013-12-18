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

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include "soundmanager.h"
#include <QObject>
#include <bb/system/phone/Phone>
#include <bb/system/phone/Call>
#include <bb/cascades/TextArea>
#include <bb/cascades/TextField>

using namespace bb::cascades;
namespace bb
{
    namespace cascades
    {
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

class ApplicationUI : public QObject
{
    Q_OBJECT
public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI() { }
    Q_INVOKABLE void InvokeSmsComposer(QString PhoneNumber);
    Q_INVOKABLE void SendSMSMessage(QString To, QString BodyStr);
    Q_INVOKABLE void PlaySound(const QString &msg);

private slots:
    void onSystemLanguageChanged();
    void OnInvokeFinished();

public slots:
	void onCallUpdated(const bb::system::phone::Call &call);

private:
    void ShowToast(QString BodyStr);
    QTranslator* m_pTranslator;
    LocaleHandler* m_pLocaleHandler;
    SoundManager *mSoundManager;
    TextArea *m_CallInfoTxtArea;
    TextField *m_PhoneNumberTxtField;
};

#endif /* ApplicationUI_HPP_ */
