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

#include "Team.h"

namespace data {

Team::Team()
: m_name("Unknown")
, m_conference(Team::East)
, m_playoffrank(1)
, m_powerrank(1)
, m_tweets(0)
{
	m_id = QUuid::createUuid();
}

Team::Team(QString name, Conference conf, int playoffRank, int powerRank, int tweets)
: m_name(name)
, m_conference(conf)
, m_playoffrank(playoffRank)
, m_powerrank(powerRank)
, m_tweets(tweets)
{
	m_id = QUuid::createUuid();
}

Team::~Team() {
}

Team::Team(const Team& rhs) : QObject() {
	operator=(rhs);
}

Team& Team::operator=(const Team& rhs)
{
	if(this != &rhs)
	{
		m_id = rhs.m_id;
		m_tweets = rhs.m_tweets;
		m_name = rhs.m_name;
		m_conference = rhs.m_conference;
		m_playoffrank = rhs.m_playoffrank;
		m_powerrank = rhs.m_powerrank;

		m_roster.clear();
		m_roster = rhs.m_roster;
	}
	return *this;
}


QUuid Team::id() const
{
	return m_id;
}

QString Team::name() const
{
	return m_name;
}

int Team::tweets() const
{
	return m_tweets;
}

int Team::conference() const
{
	return m_conference;
}

int Team::playoffRank() const
{
	return m_playoffrank;
}

int Team::powerRank() const
{
	return m_powerrank;
}

bool Team::getPlayer(QUuid playerid, Player & player)
{
	QMap<QUuid,Player>::iterator it = m_roster.find(playerid);
	if(it != m_roster.end())
	{
		player = it.value();
		return true;
	}
	return false;
}

void Team::setTweets(int value)
{
	if (value != m_tweets) {
		m_tweets = value;
		emit tweetsChanged(value);
	}
}

void Team::setPowerRank(int value)
{
	if (value != m_powerrank) {
		m_powerrank = value;
		emit powerRankChanged(value);
	}
}

void Team::addPlayer(Player player)
{
	QMap<QUuid,Player>::iterator it = m_roster.find(player.id());
	if(it == m_roster.end())
		m_roster[player.id()] = player;
}

QList<Player> Team::getStartingLineup()
{
	QList<Player> starters;
	QMap<QUuid,Player>::iterator it;
	for(it = m_roster.begin(); it!=m_roster.end(); ++it)
	{
		if(it.value().starter())
			starters.push_back(it.value());
	}
	return starters;
}

} /* namespace data */
