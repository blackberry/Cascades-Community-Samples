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

#ifndef MATCHUPS_H_
#define MATCHUPS_H_
#include <QObject>
#include <QMap>
#include "Team.h"
namespace data {

class Matchups : public QObject{
	Q_OBJECT
private:
	static Matchups* m_instance;
	Matchups();
public:
	static Matchups* instance();
	virtual ~Matchups();

	void initialize();
	void firstRound(QMap<QUuid,QUuid> & east, QMap<QUuid,QUuid> & west);
	int getPixelWidth(QUuid teamid);
	int getPixelHeight(QUuid teamid);
	QUuid findByPlayoffRank(Team::Conference conference, int rank);
	QUuid findByName(QString name);
	int getTotalTweets();
	void refreshSocial();
	Team* getTeam(QUuid teamid);

private:
	void populatePlayers(QString team, QString file);
	int getMaxPowerRank();
	int getMaxTweets();

	float getRandomPercent();

	QMap<QUuid, Team*> m_teams;
};

} /* namespace data */
#endif /* MATCHUPS_H_ */
