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

#include "applicationui.hpp"

ApplicationUI::ApplicationUI()
    : QObject()
    , _sbCtx(NULL)
    , _rngCtx(NULL)
    , _sha256Context(NULL)
    , _hmacContext(NULL)
    , _sha1Context(NULL)
    , _expireTimer(new QTimer(this))
    , _keyBitLength(2048)
    , _keyPubExp(3)
{
    _translator = new QTranslator(this);
    _localeHandler = new LocaleHandler(this);

    bool res = QObject::connect(_localeHandler, SIGNAL(systemLanguageChanged()),
                                           this, SLOT(onSystemLanguageChanged()));
    Q_ASSERT(res);
    Q_UNUSED(res);

    onSystemLanguageChanged();

    _qml = QmlDocument::create("asset:///main.qml").parent(this);
    _root = _qml->createRootObject<AbstractPane>();
    _mainPage = _root->findChild<QObject*>("mainPage");

    // ============== Message to be sent to page

    QObject::connect(       this, SIGNAL(message(QVariant)),
                       _mainPage, SLOT(onMessage(QVariant)));

    // ============== Hook up buttons

    QObject::connect(  _mainPage, SIGNAL(initRandomNumber()),
                            this, SLOT(onInitRandomNumber()));
    QObject::connect(  _mainPage, SIGNAL(getRandomNumber()),
                            this, SLOT(onGetRandomNumber()));
    QObject::connect(  _mainPage, SIGNAL(endRandomNumber()),
                            this, SLOT(onEndRandomNumber()));

    QObject::connect(  _mainPage, SIGNAL(initHash()),
                            this, SLOT(onInitHash()));
    QObject::connect(  _mainPage, SIGNAL(endHash()),
                            this, SLOT(onEndHash()));
    QObject::connect(  _mainPage, SIGNAL(makeHash(int)),
                            this, SLOT(onMakeHash(int)));

    QObject::connect(  _mainPage, SIGNAL(initRsa()),
                            this, SLOT(onInitRsa()));
    QObject::connect(  _mainPage, SIGNAL(endRsa()),
                            this, SLOT(onEndRsa()));
    QObject::connect(  _mainPage, SIGNAL(doRsa()),
                            this, SLOT(onDoRsa()));

    QObject::connect(  _mainPage, SIGNAL(initKdf()),
                            this, SLOT(onInitKdf()));
    QObject::connect(  _mainPage, SIGNAL(endKdf()),
                            this, SLOT(onEndKdf()));
    QObject::connect(  _mainPage, SIGNAL(doKdf()),
                            this, SLOT(onDoKdf()));

    Application::instance()->setScene(_root);
}

void ApplicationUI::onInitRandomNumber()
{
    emit message("onInitRandomNumber");

    int rc = hu_GlobalCtxCreateDefault(&_sbCtx);
    _CHECKRC( rc, "onInitRandomNumber hu_GlobalCtxCreateDefault()" );

    rc = hu_RegisterSbg56(_sbCtx);
    _CHECKRC( rc, "onInitRandomNumber hu_RegisterSbg56()" );

    rc = hu_RegisterSystemSeed(_sbCtx);
    _CHECKRC( rc, "onInitRandomNumber hu_RegisterSystemSeed()" );

    rc = hu_InitSbg56(_sbCtx);
    if (rc != SB_FAIL_LIBRARY_ALREADY_INIT) {
        _CHECKRC( rc, "onInitRandomNumber hu_InitSbg56()" );
    }

    uchar mySeed[4];
    size_t seedLen = 4;

    rc = hu_SeedGet(&seedLen, mySeed, _sbCtx);
    _CHECKRC( rc, "onInitRandomNumber hu_SeedGet()" );

    rc = hu_RngCreate(seedLen, mySeed, NULL, NULL, NULL, &_rngCtx, _sbCtx);
    _CHECKRC( rc, "onInitRandomNumber hu_RngCreate()" );
}

void ApplicationUI::onGetRandomNumber()
{
    emit message("onGetRandomNumber");

    int buffer = 0;

    int rc = hu_RngGetBytes(_rngCtx, sizeof(buffer), (uchar *)&buffer, _sbCtx);
    _CHECKRC( rc, "onGetRandomNumber hu_RngGetBytes()" );
    emit message("Random number=" + QString::number(buffer));
}

void ApplicationUI::onEndRandomNumber()
{
    emit message("onEndRandomNumber");

    int rc = hu_RngDestroy(&_rngCtx, _sbCtx);
    _CHECKRC( rc, "onEndRandomNumber hu_RngDestroy()" );

    _rngCtx = NULL;

    rc = hu_GlobalCtxDestroy(&_sbCtx);
    _CHECKRC( rc, "onEndRandomNumber hu_GlobalCtxDestroy()" );

    _sbCtx = NULL;
}

void ApplicationUI::onInitHash() {

    emit message("onInitHash");

    int rc = SB_SUCCESS; /* Return Code */

    /* Create SB Contexts */

    rc = hu_GlobalCtxCreateDefault(&_sbCtx);
    _CHECKRC( rc, "onInitHash hu_GlobalCtxCreateDefault()" );

    rc = hu_RegisterSbg56(_sbCtx);
    _CHECKRC( rc, "onInitHash hu_RegisterSbg56()" );

    rc = hu_InitSbg56(_sbCtx);
    if (rc != SB_FAIL_LIBRARY_ALREADY_INIT) {
        _CHECKRC( rc, "onInitHash hu_InitSbg56()" );
    }
}

void ApplicationUI::onEndHash() {
    emit message("onEndHash");
    hu_GlobalCtxDestroy(&_sbCtx);

    _sbCtx = NULL;
}

void ApplicationUI::onMakeHash(int hashType) {

    bool doSha1 = (hashType == 0);
    bool doSha256 = (hashType == 1);
    bool doHmac = (hashType == 2);

    qDebug() << "hashType:" << hashType << endl;

    QString input_data("1234567890");
    unsigned char messageDigestSha1[SB_SHA1_DIGEST_LEN];
    unsigned char messageDigestSha2[SB_SHA256_DIGEST_LEN];
    unsigned char messageDigestHMAC[SB_HMAC_SHA256_256_TAG_LEN];
    unsigned char hmacKey[SB_HMAC_SHA256_256_TAG_LEN] = {0x1C, 0x85, 0xC9, 0xBB, 0x25, 0xBA, 0xFB, 0xE9,
                                                         0xFC, 0xE7, 0xB1, 0x3B, 0x9B, 0x53, 0x6F, 0x40,
                                                         0x79, 0x4F, 0x13, 0x3B, 0x60, 0x96, 0xF7, 0x44,
                                                         0x0B, 0x62, 0x39, 0xFB, 0xCA, 0x4A, 0xA3, 0xB6
                                                        };
    /* Initialize variables */

    int rc = SB_SUCCESS; /* Return Code */

    QByteArray input_bytes = input_data.toUtf8();
    unsigned char* hash_input = reinterpret_cast<unsigned char*>(input_bytes.data());

    if (doHmac) {

        emit message("onMakeHash - input data:" + input_data + " (" + QString::number(input_data.length()) + ")");

        QString digest;
        for (int i = 0; i < SB_HMAC_SHA256_256_TAG_LEN; ++i) {
            digest.append(QString("%1").arg(QString::number((uint)hmacKey[i], 16), 2, QChar('0')));
        }
        emit message("HMAC Key = " + digest);

        rc = hu_HMACSHA256Begin(SB_HMAC_SHA256_256_TAG_LEN, hmacKey, NULL, &_hmacContext, _sbCtx);
        _CHECKRC( rc, "onMakeHash hu_HMACSHA256Begin()" );

        /* Hash Message */

        rc = hu_HMACSHA256Hash(_hmacContext, (size_t) input_bytes.length(), hash_input, _sbCtx);
        _CHECKRC( rc, "onMakeHash hu_HMACSHA256Hash()" );

        /* Complete HMAC-SHA-256 Hashing */

        rc = hu_HMACSHA256End(&_hmacContext, SB_HMAC_SHA256_256_TAG_LEN, messageDigestHMAC, _sbCtx);
        _CHECKRCEXT( rc,
            {
                QString digest;
                for (int i = 0; i < SB_HMAC_SHA256_256_TAG_LEN; ++i) {
                    digest.append(QString("%1").arg(QString::number((uint)messageDigestHMAC[i], 16), 2, QChar('0')));
                }
                emit message("Result HMAC = " + digest);
            },
            "onMakeHash hu_HMACSHA256End()" );

    } else if (doSha1) {

        emit message("onMakeHash - input data:" + input_data + " (" + QString::number(input_data.length()) + ")");

        rc = hu_SHA1Begin((size_t) SB_SHA1_DIGEST_LEN, NULL, &_sha1Context, _sbCtx);
        _CHECKRC( rc, "onMakeHash hu_SHA1Begin()" );

        /* Hash Message */

        rc = hu_SHA1Hash(_sha1Context, (size_t) input_bytes.length(), hash_input, _sbCtx);
        _CHECKRC( rc, "onMakeHash hu_SHA1Hash()" );

        /* Complete SHA-1 Hashing */

        rc = hu_SHA1End(&_sha1Context, messageDigestSha1, _sbCtx);
        _CHECKRCEXT( rc,
            {
                QString digest;
                for (int i = 0; i < SB_SHA1_DIGEST_LEN; ++i) {
                    digest.append(QString("%1").arg(QString::number((uint)messageDigestSha1[i], 16), 2, QChar('0')));
                }
                emit message("Result SHA-1 hash = " + digest);
            },
            "onMakeHash hu_SHA1End()" );

    } else if (doSha256) {

        emit message("onMakeHash - input data:" + input_data + " (" + QString::number(input_data.length()) + ")");

        rc = hu_SHA256Begin((size_t) SB_SHA256_DIGEST_LEN, NULL, &_sha256Context, _sbCtx);
        _CHECKRC( rc, "onMakeHash hu_SHA256Begin()" );

        /* Hash Message */

        rc = hu_SHA256Hash(_sha256Context, (size_t) input_bytes.length(), hash_input, _sbCtx);
        _CHECKRC( rc, "onMakeHash hu_SHA256Hash()" );

        /* Complete SHA-256 Hashing */

        rc = hu_SHA256End(&_sha256Context, messageDigestSha2, _sbCtx);
        _CHECKRCEXT( rc,
            {
                QString digest;
                for (int i = 0; i < SB_SHA256_DIGEST_LEN; ++i) {
                    digest.append(QString("%1").arg(QString::number((uint)messageDigestSha2[i], 16), 2, QChar('0')));
                }
                emit message("Result SHA-256 hash = " + digest);
            },
            "onMakeHash hu_SHA256End()" );
    }
}

void ApplicationUI::onInitRsa() {

    emit message("onInitRsa");

    void* subName = 0;
    size_t subNameLen = 0;
    tp_X509CertEncodeCtx encCtx = 0;
    void* gname = NULL;
    size_t gnameLen = 0;
    tp_X509GeneralNamesEncodeCtx gnameEncCtx = 0;
    tp_DateTime startDate;
    tp_DateTime endDate;

    startDate.year = 2015;
    startDate.mon = 3;
    startDate.mday = 24;
    startDate.hour = 0;
    startDate.min = 0;
    startDate.sec = 0;
    startDate.offHour = 0;
    startDate.offMin = 0;
    startDate.offDir = 0;

    const char *RSA_CERTIFICATE_SUBJECT = "E = me@foobar.com, CN = Mock RSA User, "
                                          "OU = GB, OU = EMEA, DC = foobar, DC = net";

    const char * CERTIFICATE_EMAIL = "me@foobar.com";

    const unsigned char* EXTENDED_KEY_USAGE_OID[] = { TP_OID_PKIX_KP_CLIENT_AUTH, TP_OID_PKIX_KP_EMAIL_PROTECTION };

    int rc = SB_SUCCESS; /* Return Code */

    /* Create SB Contexts */

    onInitRandomNumber();

    _keySlot.cryptoAlgorithm = SC_CRYPTO_ALGORITHM_RSA;
    _keySlot.params = 0;
    _keySlot.privateKey = 0;
    _keySlot.publicKey = 0;
    _keySlot.certificate = 0;
    _keySlot.certificateLength = 0;

    rc = hu_RSAParamsCreate( _keyBitLength , _rngCtx, NULL, &(_keySlot.params), _sbCtx );
    _CHECKRC( rc, "onInitRsa hu_RSAParamsCreate()" );

    rc = hu_RSAKeyGen( _keySlot.params, _keyPubExp, NULL, &( _keySlot.privateKey ), &( _keySlot.publicKey ), _sbCtx );
    _CHECKRCEXT( rc, {
            emit message("onInitRsa: key pair generated");
    }, "onInitRsa hu_RSAKeyGen()" );

    rc = tp_X509NameFromStringAlloc( 0, RSA_CERTIFICATE_SUBJECT, &subName, &subNameLen, _sbCtx );
    _CHECKRC( rc, "onInitRsa tp_X509NameFromStringAlloc()" );

    rc = tp_X509CertEncodeBegin( TP_SIGN_FLAG_NO_SPECIFIED, subName, subNameLen, CIC_ALLOC_KEEP, NULL,
                                         _keySlot.params, _keySlot.publicKey, NULL, 0, CIC_ALLOC_KEEP,
                                         TP_SIGN_RSA_SHA256_ID, NULL, _keySlot.params, _keySlot.privateKey, NULL, &encCtx, _sbCtx );
    _CHECKRC( rc, "onInitRsa tp_X509CertEncodeBegin()" );

    endDate = startDate;
    endDate.year = endDate.year + 10;

    rc = tp_X509CertEncodeSetValidityDates( encCtx, 0, &startDate, &endDate );
    _CHECKRC( rc, "onInitRsa tp_X509CertEncodeSetValidityDates()" );

    rc = tp_X509CertEncodeSetBasicConstraints( encCtx, 1, 0, 1 );
    _CHECKRC( rc, "onInitRsa tp_X509CertEncodeSetBasicConstraints()" );

    rc = tp_X509CertEncodeSetKeyUsage( encCtx, 1,
                                               TP_X509_KEY_USAGE_KEY_ENCIPHERMENT | TP_X509_KEY_USAGE_DIGITAL_SIGNATURE
                                             | TP_X509_KEY_USAGE_KEY_CERT_SIGN | TP_X509_KEY_USAGE_DATA_ENCIPHERMENT
                                             | TP_X509_KEY_USAGE_KEY_AGREEMENT | TP_X509_KEY_USAGE_DATA_ENCIPHERMENT );
    _CHECKRC( rc, "onInitRsa tp_X509CertEncodeSetKeyUsage()" );

    rc = tp_X509CertEncodeSetExtKeyUsage( encCtx, 1, EXTENDED_KEY_USAGE_OID, 2 );
    _CHECKRC( rc, "onInitRsa tp_X509CertEncodeSetExtKeyUsage()" );

    rc = tp_X509GeneralNamesEncodeBegin( 0, &gnameEncCtx, _sbCtx );
    _CHECKRC( rc, "onInitRsa tp_X509GeneralNamesEncodeBegin()" );

    rc = tp_X509GeneralNamesEncodeAddRfc822Name( gnameEncCtx, CERTIFICATE_EMAIL, 0 );
    _CHECKRC( rc, "onInitRsa tp_X509GeneralNamesEncodeAddRfc822Name()" );

    rc = tp_X509GeneralNamesEncodeEndAlloc( &gnameEncCtx, &gname, &gnameLen );
    _CHECKRC( rc, "onInitRsa tp_X509GeneralNamesEncodeEndAlloc()" );

    rc = tp_X509CertEncodeSetSubjectAltName( encCtx, 0, gname, gnameLen, CIC_ALLOC_KEEP );
    _CHECKRC( rc, "onInitRsa tp_X509CertEncodeSetSubjectAltName()" );

    rc = tp_X509CertEncodeEndAlloc( &encCtx, &( _keySlot.certificate ), &( _keySlot.certificateLength ) );
    _CHECKRCEXT( rc, {
            emit message("onInitRsa: Certificate Generated from: " + QString(RSA_CERTIFICATE_SUBJECT));
    }, "onInitRsa tp_X509CertEncodeEndAlloc()" );

    QString certHexString;
    qDebug() << "XXXX RSA Cert length:" << _keySlot.certificateLength << endl;

    size_t segmentLen = 40;
    size_t numSegments = _keySlot.certificateLength / segmentLen;
    size_t lastSegment = _keySlot.certificateLength % segmentLen;

    qDebug() << "XXXX RSA Cert:" << endl;

    for (size_t j = 0; j < numSegments; ++j) {
        certHexString.clear();
        for (size_t i = j*segmentLen; i < (j+1)*segmentLen; ++i) {
            certHexString.append(QString("%1").arg(QString::number((((uint8_t *)(_keySlot.certificate))[i]), 16), 2, QChar('0')));
        }
        qDebug() << certHexString << endl;
    }
    certHexString.clear();
    for (size_t i = _keySlot.certificateLength-lastSegment; i < _keySlot.certificateLength; ++i) {
        certHexString.append(QString("%1").arg(QString::number((((uint8_t *)(_keySlot.certificate))[i]), 16), 2, QChar('0')));
    }
    qDebug() << certHexString << endl;
}

void ApplicationUI::onEndRsa() {
    emit message("onEndRsa");

    sb_free( _keySlot.certificate, _sbCtx );
    _keySlot.certificate = 0;

    hu_RSAKeyDestroy( _keySlot.params, &( _keySlot.privateKey ), &( _keySlot.publicKey ), _sbCtx );
    hu_RSAParamsDestroy( &( _keySlot.params ), _sbCtx );

    onEndRandomNumber();
}

void ApplicationUI::onDoRsa() {

    emit message("onDoRsa");

    char textToEncrypt[] = "This is some plaintext! Hello Plaintext";

    unsigned char plaintext[_keyBitLength/8];
    unsigned char ciphertext[_keyBitLength/8];

    memset(plaintext, 0, sizeof(plaintext));
    memset(ciphertext, 0, sizeof(ciphertext));
    memcpy (plaintext, textToEncrypt, strlen(textToEncrypt)+1 );

    int rc = SB_SUCCESS; /* Return Code */

    emit message(QString("onDoRsa plaintext: %1").arg(textToEncrypt));

    rc = hu_RSAPublicEncrypt( _keySlot.params, _keySlot.publicKey, plaintext, ciphertext, _sbCtx );
    _CHECKRC( rc, "onDoRsa hu_RSAPublicEncrypt()" );

    QString cipherTextHex;

    for (size_t i = 0; i < sizeof(ciphertext); ++i) {
        cipherTextHex.append(QString("%1").arg(QString::number(((uint8_t *)ciphertext)[i], 16), 2, QChar('0')));
    }
    qDebug() << cipherTextHex << endl;
    emit message("Ciphertext: " + cipherTextHex);

    // and back again

    memset(plaintext, 0, sizeof(plaintext));

    rc = hu_RSAPrivateDecrypt( _keySlot.params, _keySlot.privateKey, ciphertext, plaintext, _sbCtx );
    _CHECKRC( rc, "onDoRsa hu_RSAPrivateDecrypt()" );

    qDebug() << (char *)plaintext << endl;
    emit message(QString("onDoRsa plaintext: %1").arg((char *)plaintext));
}

void ApplicationUI::onInitKdf() {

    emit message("onInitKdf - Derive a key using SHA1+KDF2");

    int rc = SB_SUCCESS; /* Return Code */

    /* Create SB Contexts */

    rc = hu_GlobalCtxCreateDefault(&_sbCtx);
    _CHECKRC( rc, "onInitKdf hu_GlobalCtxCreateDefault()" );

    rc = hu_RegisterSbg56(_sbCtx);
    _CHECKRC( rc, "onInitKdf hu_RegisterSbg56()" );

    rc = hu_InitSbg56(_sbCtx);
    if (rc != SB_FAIL_LIBRARY_ALREADY_INIT) {
        _CHECKRC( rc, "onInitKdf hu_InitSbg56()" );
    }
}

void ApplicationUI::onEndKdf() {
    emit message("onEndKdf");
    hu_GlobalCtxDestroy(&_sbCtx);

    _sbCtx = NULL;
}

void ApplicationUI::onDoKdf() {

    unsigned char sha1Kdf2SharedSecret[] = {'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ',
                                            's', 'h', 'a', 'r', 'e', 'd', ' ', 's', 'e', 'c', 'r', 'e', 't', ' ',
                                            'p', 'a', 's', 's', 'p', 'h', 'r', 'a', 's', 'e', '!', '\0'};

    size_t sha1Kdf2SharedSecretLen = sizeof(sha1Kdf2SharedSecret)-1;
    unsigned char sha1Kdf2GeneratedKey[2048/8];

    emit message(QString("onDoKdf - doSha1Kdf2 shared secret: %1").arg(reinterpret_cast<const char*>(sha1Kdf2SharedSecret)));

    QString generatedKeyHex;

    int rc = hu_KDFDerive(
            HU_KDF_IEEE_KDF2_SHA1,
            sha1Kdf2SharedSecretLen, sha1Kdf2SharedSecret,
            0, NULL,
            sizeof(sha1Kdf2GeneratedKey), sha1Kdf2GeneratedKey, _sbCtx);
    _CHECKRCEXT( rc,
        {
            for (size_t i = 0; i < sizeof(sha1Kdf2GeneratedKey); ++i) {
                generatedKeyHex.append(QString("%1").arg(QString::number(((uint8_t *)sha1Kdf2GeneratedKey)[i], 16), 2, QChar('0')));
            }
            qDebug() << generatedKeyHex << endl;
            emit message("Generated SHA1+KDF2 Key: " + generatedKeyHex);
        },
        "onMakeHash onDoKdf()" );
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(_translator);
    QString locale_string = QLocale().name();
    QString file_name = QString("TestRandom_%1").arg(locale_string);
    if (_translator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(_translator);
    }
}
