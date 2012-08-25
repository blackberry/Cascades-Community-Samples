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

#ifndef TWEET_H_
#define TWEET_H_

#include <QTCore/QObject>
#include <QDateTime>

/*
 * This class represents a single tweet. It only holds the tweet text and date
 * but can be expanded to have additional info like who it was tweet to or retweeted
 * from, images, a url etc.
 */
class Tweet : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ getText WRITE setText NOTIFY textChanged FINAL)
    Q_PROPERTY(QDateTime date READ getDate WRITE setDate NOTIFY dateChanged FINAL)

public:
    /*
     * Ctor & Dtor
     */
    Tweet(QObject* parent);
    Tweet(QString text, QDateTime date);
    virtual ~Tweet();

    /*
     * Setter and Getter methods for m_text
     */
    QString getText() const;
    void setText(QString text);

    /*
     * Setter and Getter methods for m_text
     */
    QDateTime getDate() const;
    void setDate(QDateTime datetime);

Q_SIGNALS:
    /*
     * signals emitted when text and date members are changed
     */
    void textChanged(QString text);
    void dateChanged(QDateTime tweetDate);

private:
    QString m_text;     //stores the tweet text
    QDateTime m_tweetDate; //stores the tweet date

    Q_DISABLE_COPY(Tweet)
};

#endif /* TWEET_H_ */
