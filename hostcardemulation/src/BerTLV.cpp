/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
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

#define LOG_TO_CONSOLE

#include "BerTLV.h"
#if defined(LOG_TO_CONSOLE)
#include <stdio.h>
#endif //LOG_TO_CONSOLE

BerTLV::BerTLV(const QString& tag, const QString& name)
    : _tag(tag), _name(name) {

    //TODO: should throw exception is tag length is odd
}

void BerTLV::appendTag(QByteArray& buffer) {
    // convert the hexstring to bytes
    QByteArray tagBytes = QByteArray::fromHex(_tag.toAscii());

    buffer.append(tagBytes);
}

bool BerTLV::lengthFitsInSimpleForm(uint32_t length) {
    return length <= 0x7F;
}

// handling the definite form only
void BerTLV::appendLength(QByteArray& buffer, uint32_t length, uint32_t lengthSize) {

    if (0 == lengthSize) {
        // autofit
        lengthSize = 1;
        if (!lengthFitsInSimpleForm(length)) {
            uint32_t shifted = length;
            while (shifted != 0) {  // count number of octets
                shifted >>= 8;
                ++lengthSize;
            }
        }
    }

    if (1 == lengthSize) {
        // short form
        if (!lengthFitsInSimpleForm(length)) {
            //TODO throw
            return;
        }
        uint8_t lengthByte = static_cast<uint8_t>(length);
        buffer.append(lengthByte);
    } else {
        // long form
        uint8_t initialLengthOctet = static_cast<uint8_t>(lengthSize);
        initialLengthOctet |= 0x80;     //bit 8 shall be one
        buffer.append(initialLengthOctet);

        QByteArray lengthOctets;
        while ( lengthOctets.size() < static_cast<int>(lengthSize) ) {
            uint8_t octet = static_cast<uint8_t>(length & 0xFF);
            lengthOctets.push_front(octet);
            length >>= 8;
        }
        buffer.append(lengthOctets);
    }
}

void BerTLV::traceLine(uint32_t indent, const QString& line) const {
    const uint32_t INDENT_WIDTH = 4;
#if defined(LOG_TO_CONSOLE)
    // log to console for unittest debugging
    printf("%s\n", (QString(indent * INDENT_WIDTH, ' ') + line).toAscii().constData());
#endif
}

/*
 * BerSimpleTLV implementation
 */
BerSimpleTLV::BerSimpleTLV(const QString& tag, const QString& name, uint32_t lengthSize, const QByteArray& value)
    : BerTLV(tag, name)
    , _lengthSize(lengthSize)
    , _value(value) {

}

void BerSimpleTLV::appendBytes(QByteArray& buffer) {
    // Type
    appendTag(buffer);

    // Length
    appendLength(buffer, _value.size(), _lengthSize);

    // Value
    buffer.append(_value);
}

void BerSimpleTLV::trace(uint32_t indent) {
    QByteArray line;
    appendBytes(line);

    // TAG, NAME, LENGTH, VALUE
    traceLine(indent, QString("%1 '%2' Len=%3 %4")
            .arg(_tag)
            .arg(_name)
            .arg(_value.size(),0,16)
            .arg(line.toHex().toUpper().constData()));
}

/*
 * BerCompositeTLV implementation
 */
BerCompositeTLV::BerCompositeTLV(const QString& tag, const QString& name, uint32_t lengthSize)
    : BerTLV(tag, name)
    , _lengthSize(lengthSize) {

}


BerCompositeTLV::~BerCompositeTLV() {
    QList<BerTLV*>::iterator it = _childs.begin();
    while (it != _childs.end()) {
        delete *it;
        it = _childs.erase(it);
    }
}

void BerCompositeTLV::appendChilds(QByteArray& serializedChilds) {
    QList<BerTLV*>::const_iterator it = _childs.begin();
    while (it != _childs.end()) {
        (*it)->appendBytes(serializedChilds);
        ++it;
    }
}

void BerCompositeTLV::appendBytes(QByteArray& buffer) {

    // flatten childs
    QByteArray serializedChilds;
    appendChilds(serializedChilds);

    // Type
    appendTag(buffer);

    // Length
    appendLength(buffer, serializedChilds.size(), _lengthSize);

    // Value
    buffer.append(serializedChilds);
}

void BerCompositeTLV::trace(uint32_t indent) {
    // flatten childs
    QByteArray serializedChilds;
    appendChilds(serializedChilds);

    QByteArray line;
    appendBytes(line);

    // TAG, NAME, LENGTH, VALUE
    traceLine(indent, QString("%1 '%2' Len=%3 %4")
            .arg(_tag)
            .arg(_name)
            .arg(serializedChilds.size(),0,16)
            .arg(line.toHex().toUpper().constData()));

    QList<BerTLV*>::const_iterator it = _childs.begin();
    while (it != _childs.end()) {
        (*it)->trace(indent + 1);
        ++it;
    }
}

// ownership of this pointer is transferred
void BerCompositeTLV::addChild(BerTLV* child) {
    _childs.push_back(child);
}

