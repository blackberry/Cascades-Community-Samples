/*
 * Copyright (c) 2011-2015 BlackBerry Limited.
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

#include "Utilities.hpp"

#include <QObject>

#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/Application>
#include <bb/cascades/LocaleHandler>

#include <bb/cascades/TouchEvent>
#include <bb/cascades/ImageView>

#include <huctx.h>
#include <hurandom.h>
#include <sbreturn.h>
#include <hugse56.h>
#include <husha1.h>
#include <husha2.h>
#include <huseed.h>
#include <hursa.h>
#include <tpid.h>
#include <tpnamec.h>
#include <tpcerte.h>
#include <tpcertd.h>
#include <tpgnamee.h>
#include <tpsigndef.h>
#include <tpkpukie.h>
#include <tpp8d.h>
#include <hukdf.h>
#include <huarc4.h>

#include <smartcard/sc_data_types.h>

#define _CHECKRC( __rc, ... ) \
{  \
    if( __rc == SB_SUCCESS ) \
    { \
        emit message("SUCCESS " __VA_ARGS__); \
    } else { \
        emit message("ERROR " __VA_ARGS__ " " + Utilities::intToHex(__rc)); \
        return; \
    } \
}

#define _CHECKRCEXT( __rc, __ext, ... ) \
{  \
    if( __rc == SB_SUCCESS ) \
    { \
        emit message("SUCCESS " __VA_ARGS__); \
        __ext \
    } else { \
        emit message("ERROR " __VA_ARGS__ " " + Utilities::intToHex(__rc)); \
        return; \
    } \
}

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

struct KeySlot
{
    sc_crypto_algorithm_t cryptoAlgorithm;
    sb_Params params;
    sb_PrivateKey privateKey;
    sb_PublicKey publicKey;
    void *certificate;
    size_t certificateLength;
};

class ApplicationUI : public QObject
{
    Q_OBJECT

public:
    ApplicationUI();
    virtual ~ApplicationUI() {}

private slots:
    void onSystemLanguageChanged();

public slots:
    void onInitRandomNumber();
    void onGetRandomNumber();
    void onEndRandomNumber();

    void onInitHash();
    void onEndHash();
    void onMakeHash(int);

    void onInitRsa();
    void onEndRsa();
    void onDoRsa();

    void onInitKdf();
    void onEndKdf();
    void onDoKdf();

    void onInitStream();
    void onEndStream();
    void onDoStream();

signals:
    void message(const QVariant &text);

private:
    QTranslator* _translator;
    bb::cascades::LocaleHandler* _localeHandler;
    QmlDocument *_qml;
    AbstractPane *_root;
    QObject *_mainPage;
    sb_GlobalCtx _sbCtx;
    sb_RNGCtx _rngCtx;
    sb_Context _sha256Context;
    sb_Context _hmacContext;
    sb_Context _sha1Context;

    ImageView *_imageView;
    QObject *_captureContainer;
    QTimer *_expireTimer;
    KeySlot _keySlot;
    size_t _keyBitLength;
    size_t _keyPubExp;

    sb_Params _arc4Params;
    sb_Key _arc4Key;
    sb_Context _arc4Context;
};

#endif /* ApplicationUI_HPP_ */
