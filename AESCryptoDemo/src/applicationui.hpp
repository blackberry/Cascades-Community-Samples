/**
   Copyright (c) 2013 BlackBerry Limited.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**/
#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>

#include "GlobalContext.hpp"

namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
    }
}

class QTranslator;

class ApplicationUI : public QObject
{
    Q_OBJECT
public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI() { }

    Q_INVOKABLE QString generate();

    Q_INVOKABLE void setKey(const QString val);
    Q_INVOKABLE QString key();

    Q_INVOKABLE void setIV(const QString val);
    Q_INVOKABLE QString iv();

    Q_INVOKABLE void setPlainText(const QString val);
    Q_INVOKABLE QString plainText();

    Q_INVOKABLE void setCipherText(const QString val);
    Q_INVOKABLE QString cipherText();

    Q_INVOKABLE QString recoveredPlainText();
    Q_INVOKABLE void setRecoveredPlainText(const QString val);

    Q_INVOKABLE bool encrypt();
    Q_INVOKABLE bool decrypt();

    Q_PROPERTY (bool cryptoAvailable READ isCryptoAvailable NOTIFY cryptoAvailableChanged);

    Q_INVOKABLE bool isCryptoAvailable() {
    	return globalContext.isValid();
    }

signals:
	void cryptoAvailableChanged();

private:
    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;

    QString _plainText,_key,_iv,_cipherText,_recoveredPlainText;

    bool fromHex(const QString in, QByteArray & out);
    QString toHex(const QByteArray & in);

    void pad(QByteArray & in);
    bool removePadding(QByteArray & out);

    bool crypt(bool isEncrypt, const QByteArray & in, QByteArray & out);
    char nibble(char);

    GlobalContext globalContext;

    void toast(const QString & message);
    void toast(const char * message);
};

#endif /* ApplicationUI_HPP_ */
