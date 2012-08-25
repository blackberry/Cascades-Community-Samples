/*
 * Copyright (c) 2011-2012 Research In Motion Limited.
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

#include "Tweet.hpp"
#include <qDebug>

/*
 * Constructor - initialises class members to their default values
 */
Tweet::Tweet(QObject* parent)
    : QObject(parent)
    , m_text("")
    , m_tweetDate(QDateTime::currentDateTimeUtc())
{
}

/*
 * Constructor - initialises class members to default values and
 *               text and tweetDates to passed parameters
 */
Tweet::Tweet(QString text, QDateTime date)
    : QObject()
    , m_text(text)
    , m_tweetDate(date)
{
}

/*
 * Deconstructor
 */
Tweet::~Tweet()
{
}

/*
 * Tweet::getText()
 *
 * returns the text of the tweet
 */
QString Tweet::getText() const
{
    return m_text;
}

/*
 * Tweet::setText(QString text)
 *
 * sets or changes the text of the tweet
 * Emits a textChanged signal if the text is changed
 */
void Tweet::setText(QString text)
{
    if (m_text != text)
    {
        m_text = text;
        emit textChanged(m_text);
    }
}

/*
 * Tweet::getDate()
 *
 * returns the date of the tweet
 */
QDateTime Tweet::getDate() const
{
    return m_tweetDate;
}

/*
 * Tweet::setDate(QDateTime tweetDate)
 *
 * sets or changes the date of the tweet
 * Emits a dateChanged signal if the date is changed
 */
void Tweet::setDate(QDateTime tweetDate)
{
    if (m_tweetDate != tweetDate)
    {
        m_tweetDate = tweetDate;
        emit dateChanged(m_tweetDate);
    }
}
