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

#include "Matchups.h"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <sstream>
#include <fstream>
#include "qDebug.h"
using namespace std;

namespace {
	const int maxScreenWidth = 1170;
	const int maxScreenHeight = 72;
	const int minScreenHeight = 24;
	const int titleHeight = 50;
}
namespace data {

Matchups* Matchups::m_instance = NULL;

Matchups* Matchups::instance()
{
	if(!m_instance)
		m_instance = new Matchups();
	return m_instance;
}

Matchups::Matchups() {
	initialize();
}

Matchups::~Matchups() {
	QMap<QUuid,Team*>::iterator it;
	for(it = m_teams.begin(); it!=m_teams.end(); ++it)
	{
		delete it.value();
	}
}

void Matchups::initialize() {
	srand(time(0));

	//east
	Team* team = new Team("Bulls",Team::East,1,80,1056);
	m_teams[team->id()] = team;

	team = new Team("Heat",Team::East,2,68,1369);
	m_teams[team->id()] = team;

	team = new Team("Pacers",Team::East,3,67,415);
	m_teams[team->id()] = team;

	team = new Team("Celtics",Team::East,4,55,981);
	m_teams[team->id()] = team;

	team = new Team("Hawks",Team::East,5,45,344);
	m_teams[team->id()] = team;

	team = new Team("Magic",Team::East,6,36,521);
	m_teams[team->id()] = team;

	team = new Team("Sixers",Team::East,7,25,800);
	m_teams[team->id()] = team;

	team = new Team("Knicks",Team::East,8,20,777);
	m_teams[team->id()] = team;

	//west
	team = new Team("Thunder",Team::West,1,78,1011);
	m_teams[team->id()] = team;

	team = new Team("Spurs",Team::West,2,73,899);
	m_teams[team->id()] = team;

	team = new Team("Lakers",Team::West,3,69,1400);
	m_teams[team->id()] = team;

	team = new Team("Clippers",Team::West,4,55,500);
	m_teams[team->id()] = team;

	team = new Team("Suns",Team::West,5,42,200);
	m_teams[team->id()] = team;

	team = new Team("Mavericks",Team::West,6,33,650);
	m_teams[team->id()] = team;

	team = new Team("Nuggets",Team::West,7,24,480);
	m_teams[team->id()] = team;

	team = new Team("Grizzlies",Team::West,8,18,741);
	m_teams[team->id()] = team;

	populatePlayers("bulls","app/native/assets/teams/bulls_lineup.csv");
	populatePlayers("sixers","app/native/assets/teams/sixers_lineup.csv");
	populatePlayers("spurs","app/native/assets/teams/spurs_lineup.csv");
	populatePlayers("suns","app/native/assets/teams/suns_lineup.csv");
	populatePlayers("heat","app/native/assets/teams/heat_lineup.csv");
	populatePlayers("knicks","app/native/assets/teams/knicks_lineup.csv");
	populatePlayers("thunder","app/native/assets/teams/thunder_lineup.csv");
	populatePlayers("mavericks","app/native/assets/teams/mavericks_lineup.csv");
	populatePlayers("pacers","app/native/assets/teams/pacers_lineup.csv");
	populatePlayers("magic","app/native/assets/teams/magic_lineup.csv");
	populatePlayers("lakers","app/native/assets/teams/lakers_lineup.csv");
	populatePlayers("nuggets","app/native/assets/teams/nuggets_lineup.csv");
	populatePlayers("celtics","app/native/assets/teams/celtics_lineup.csv");
	populatePlayers("hawks","app/native/assets/teams/hawks_lineup.csv");
	populatePlayers("grizzlies","app/native/assets/teams/grizzlies_lineup.csv");
	populatePlayers("clippers","app/native/assets/teams/clippers_lineup.csv");
}

void Matchups::populatePlayers(QString team, QString file)
{
    QMap<QUuid,Team*>::iterator it = m_teams.find(findByName(team));
    if(it != m_teams.end())
    {
    	qDebug() << "--- " << team << " ---";
    	std::ifstream fs(file.toStdString().c_str());
    	while(fs.good())
    	{
			it.value()->addPlayer(Player(fs));
    	}
    }
}

void Matchups::firstRound(QMap<QUuid,QUuid> & east, QMap<QUuid,QUuid> & west)
{
	east.clear();
	east.insert(findByPlayoffRank(Team::East,1),findByPlayoffRank(Team::East,8));
	east.insert(findByPlayoffRank(Team::East,2),findByPlayoffRank(Team::East,7));
	east.insert(findByPlayoffRank(Team::East,3),findByPlayoffRank(Team::East,6));
	east.insert(findByPlayoffRank(Team::East,4),findByPlayoffRank(Team::East,5));

	west.clear();
	east.insert(findByPlayoffRank(Team::West,1),findByPlayoffRank(Team::West,8));
	east.insert(findByPlayoffRank(Team::West,2),findByPlayoffRank(Team::West,7));
	east.insert(findByPlayoffRank(Team::West,3),findByPlayoffRank(Team::West,6));
	east.insert(findByPlayoffRank(Team::West,4),findByPlayoffRank(Team::West,5));
}

int Matchups::getPixelWidth(QUuid teamid)
{
	QMap<QUuid,Team*>::iterator it = m_teams.find(teamid);
	if(it == m_teams.end())
		return 0;

	int maxTweets = getMaxTweets();
	double activitylevel = (double(it.value()->tweets())/double(maxTweets))*100;
	int width = (activitylevel/100)*maxScreenWidth;
	return width;
}

int Matchups::getPixelHeight(QUuid teamid)
{
	QMap<QUuid,Team*>::iterator it = m_teams.find(teamid);
	if(it == m_teams.end())
		return 0;

	int maxPowerRank = getMaxPowerRank();
	double ranklevel = (double(it.value()->powerRank())/double(maxPowerRank))*100;
	int height = (ranklevel/100)*maxScreenHeight;
	if(height < minScreenHeight)
		height = minScreenHeight;
	return height;
}

QUuid Matchups::findByPlayoffRank(Team::Conference conference, int rank)
{
	QMap<QUuid,Team*>::iterator it;
	for(it = m_teams.begin(); it!=m_teams.end(); ++it)
	{
		Team * team = it.value();
		if(team->conference()==conference && team->playoffRank()==rank)
			return team->id();
	}
	return QUuid();
}

QUuid Matchups::findByName(QString name)
{
	QMap<QUuid,Team*>::iterator it;
	for(it = m_teams.begin(); it!=m_teams.end(); ++it)
	{
		Team * team = it.value();
		if(team->name().compare(name,Qt::CaseInsensitive)==0)
			return team->id();
	}
	return QUuid();
}

int Matchups::getMaxTweets()
{
	int maxtweets = 0;
	QMap<QUuid,Team*>::iterator it;
	for(it = m_teams.begin(); it!=m_teams.end(); ++it)
	{
		if(it.value()->tweets()>maxtweets)
			maxtweets = it.value()->tweets();
	}
	return maxtweets;
}

int Matchups::getTotalTweets(){
	int totalTweets = 0;
	QMap<QUuid,Team*>::iterator it;
	for(it = m_teams.begin(); it!=m_teams.end(); ++it)
	{
		totalTweets += it.value()->tweets();
	}
	return totalTweets;
}

void Matchups::refreshSocial(){
	QMap<QUuid,Team*>::iterator it;
	for(it = m_teams.begin(); it!=m_teams.end(); ++it)
	{
		it.value()->setTweets(it.value()->tweets() * (1 + (0.30 * getRandomPercent())));
		it.value()->setPowerRank(it.value()->powerRank() * (1.1 - (0.20 * getRandomPercent())));
	}
}

int Matchups::getMaxPowerRank()
{
	int maxpowerrank = 0;
	QMap<QUuid,Team*>::iterator it;
	for(it = m_teams.begin(); it!=m_teams.end(); ++it)
	{
		if(it.value()->powerRank()>maxpowerrank)
			maxpowerrank = it.value()->powerRank();
	}
	return maxpowerrank;
}

float Matchups::getRandomPercent()
{
	return rand() / (float) (RAND_MAX + 1);
}

Team* Matchups::getTeam(QUuid teamid)
{
	QMap<QUuid,Team*>::iterator it = m_teams.find(teamid);
	if(it != m_teams.end())
		return it.value();

	return NULL;
}

} /* namespace data */

