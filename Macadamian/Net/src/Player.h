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

#ifndef PLAYER_H_
#define PLAYER_H_
#include <QObject>
#include <QString>
#include <QUuid>

namespace data {

class Player : public QObject {
	Q_OBJECT
	Q_PROPERTY( QUuid id READ id )
	Q_PROPERTY( QString name READ name WRITE setName )
public:
    /*!
     * Constructor
     */
	Player();

	/*!
     * Constructor for reading info from csv file
     */
	Player(std::istream& str);

	/*!
     * Constructor
     *
     * @param name - name of the team
     * @param avgmins - average minutes
     * @param avgrebounds - average rebounds
     * @param avgassists - average assists
     * @param starter - whehter player is in starting line up
     */
	Player(QString name, float avgmins, float avgrebounds, float avgassists, bool starter=false);

	/*!
	 * Destructor
	 */
	virtual ~Player();

	/*!
	 * Copy constructor
	 */
	Player(const Player& rhs);

	/*!
	 * Assignment operator
	 */
	Player& operator=(const Player&);

public:
    /*!
     * Returns the id of the player.
     */
	QUuid id() const;

    /*!
     * Returns the average mins played
     */
	float averageMinutes() const;

    /*!
     * Returns the average rebounds per game
     */
	float averageRebounds() const;

    /*!
     * Returns the average assists per game
     */
	float averageAssists() const;

    /*!
     * Returns if the player is a starter
     */
	bool starter() const;

    /*!
     * Returns the name of the player
     */
	QString name() const;

	void deserialize(std::istream& str);

public slots:
	/*!
	 * Sets the player name
	 *
	 * @param value - the name of the player
	 */
	void setName(QString value);

private:
	QUuid m_id;			//unique player id
	QString m_name;		//name of the player
    float m_avgmins;
    float m_avgrebounds;
    float m_avgassists;
    bool m_starter;
};

} /* namespace data */
#endif /* PLAYER_H_ */
