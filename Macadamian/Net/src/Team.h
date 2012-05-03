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

#ifndef TEAM_H_
#define TEAM_H_
#include <QObject>
#include <QString>
#include <QUuid>
#include <QMap>
#include <QList>
#include "Player.h"

namespace data {

class Team : public QObject {
	Q_OBJECT
	Q_PROPERTY( QUuid id READ id )
	Q_PROPERTY( int playoffrank READ playoffRank )
	Q_PROPERTY( QString name READ name )
	Q_PROPERTY( int tweets READ tweets WRITE setTweets )
public:
	enum Conference {
		East,	//eastern conf
		West	//western conf
	};

public:
	/*!
	 * Constructor
	 */
	Team();

    /*!
     * Constructor
     *
     * @param name - name of the team
     * @param conference - conference team is in
     * @param rank - final rank of team in the conference (1-8)
     * @param tweeks - twitter activity for the team
     */
	Team(QString name, Conference conf, int playoffrank, int powerrank, int tweets);

	/*!
	 * Destructor
	 */
	virtual ~Team();

	/*!
	 * Copy constructor
	 */
	Team(const Team& rhs);

	/*!
	 * Assignment operator
	 */
	Team& operator=(const Team&);

public:
    /*!
     * Returns the id of the team.
     */
	QUuid id() const;

    /*!
     * Returns the name of the team
     */
	QString name() const;

    /*!
     * Returns the tweet count for the team on twitter
     */
	int tweets() const;

	/*!
     * Returns the conference rank of the team
     */
	int playoffRank() const;

	/*!
     * Returns the power rank of the team
     */
	int powerRank() const;

	/*!
     * Returns the conference the team
     */
	int conference() const;

    /*!
     * Finds a player based using the player id
     *
     * @param playerid - the id of the player
     * @param[out] player - the player if found
     * @return bool - true if found, false otherwise.
     */
	bool getPlayer(QUuid playerid, Player & player);
	QList<Player> getStartingLineup();

signals:
   /*!
	* Emitted when the tweets for the team changes
	*
	* @param tweets The new tweet count
	*/
	void tweetsChanged(int tweets);

	/*
	 * Emitted when the power rank for the team changes
	 *
	 * @param powerRank  The new power rank
	 */
	void powerRankChanged(int powerRank);

public slots:
	/*!
	 * Sets the number of tweets this team has received
	 *
	 * @param value - the number of tweets
	 */
	void setTweets(int value);

	/*!
	 * Sets the power rank of the team
	 *
	 * @param value - the new power rank
	 */
	void setPowerRank(int value);

	/*!
	 * Adds a player to the roster
	 *
	 * @param player - the player to add
	 */
	void addPlayer(Player player);

private:
	QUuid m_id;						//unique team id
	QString m_name;					//name of the team
	Conference m_conference;		//conference team is in
	int m_playoffrank;				//final team standing in the conference 1-8
	int m_powerrank;				//team power ranking
	int m_tweets;					//number of tweets for the team
	QMap<QUuid,Player> m_roster;	//team roster
};

} /* namespace data */
#endif /* TEAM_H_ */
