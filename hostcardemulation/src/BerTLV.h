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

#ifndef BERTLV_H_
#define BERTLV_H_

#include <QObject>
#include <stdint.h>

/*
 * Abstract base class
 *
 * References:
 * https://en.wikipedia.org/wiki/Basic_Encoding_Rules#BER_encoding
 */

class BerTLV {
public:
    BerTLV(const QString& tag, const QString& name);
    virtual ~BerTLV() {};
    virtual void appendBytes(QByteArray& buffer) = 0;
    virtual void trace(uint32_t indent = 0) = 0;
protected:
    void appendTag(QByteArray& buffer);
    void appendLength(QByteArray& buffer, uint32_t _length, uint32_t _lengthSize);
    bool lengthFitsInSimpleForm(uint32_t length);
    void traceLine(uint32_t indent, const QString& line) const;

    const QString _tag;
    const QString _name;
};

/*
 * Simple node containing values, but no sub-nodes
 */
class BerSimpleTLV : public BerTLV {
public:
    BerSimpleTLV(const QString& tag, const QString& name, uint32_t lengthSize, const QByteArray& value);
    virtual void appendBytes(QByteArray& buffer);
    virtual void trace(uint32_t indent = 0);
private:
    uint32_t _lengthSize;
    QByteArray _value;
};

/*
 * Node consisting of sub-nodes
 */
class BerCompositeTLV : public BerTLV {
public:
    BerCompositeTLV(const QString& tag, const QString& name, uint32_t lengthSize = 0);
    virtual ~BerCompositeTLV();
    virtual void appendBytes(QByteArray& buffer);
    virtual void trace(uint32_t indent = 0);
    void addChild(BerTLV* child);
private:
    void appendChilds(QByteArray& serializedChilds);

    uint32_t _lengthSize;
    QList<BerTLV*> _childs;
};

#endif /* BERTLV_H_ */
