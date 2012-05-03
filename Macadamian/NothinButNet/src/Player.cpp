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

#include "Player.h"
#include "QDebug.h"
#include <stdlib.h>
#include <sstream>
#include <vector>

namespace data {

Player::Player()
{
	m_id = QUuid::createUuid();
}

Player::Player(QString name, float avgmins, float avgrebounds, float avgassists, bool starter)
: m_name(name)
, m_avgmins(avgmins)
, m_avgrebounds(avgrebounds)
, m_avgassists(avgassists)
, m_starter(starter)
{
	m_id = QUuid::createUuid();
}

Player::Player(std::istream& str)
{
	m_id = QUuid::createUuid();
	deserialize(str);
}

Player::~Player() {
}

Player::Player(const Player& rhs) : QObject() {
	operator=(rhs);
}

Player& Player::operator=(const Player& rhs)
{
	if(this != &rhs)
	{
		m_id = rhs.m_id;
		m_avgmins = rhs.m_avgmins;
		m_avgrebounds = rhs.m_avgrebounds;
		m_avgassists = rhs.m_avgassists;
		m_starter = rhs.m_starter;
		m_name = rhs.m_name;
	}
	return *this;
}

QUuid Player::id() const
{
	return m_id;
}

float Player::averageMinutes() const
{
	return m_avgmins;
}

float Player::averageRebounds() const
{
    return m_avgrebounds;
}

float Player::averageAssists() const
{
    return m_avgassists;
}

bool Player::starter() const
{
    return m_starter;
}

QString Player::name() const
{
	return m_name;
}

void Player::setName(QString value)
{
	m_name = value;
}

void Player::deserialize(std::istream& str)
{
	std::string         line;
	std::getline(str,line);

	std::stringstream   lineStream(line);
	std::string         cell;

	std::vector<std::string> data;
	while(std::getline(lineStream,cell,','))
	{
		data.push_back(cell);
	}
	int i=0;
	for(std::vector<std::string>::iterator it = data.begin(); it != data.end(); it++, i++)
	{
		switch(i)
		{
		case 0:
			m_name = it->c_str();
			break;
		case 3:
			m_avgmins = atof(it->c_str());
			break;
		case 4:
			m_avgrebounds = atof(it->c_str());
			break;
		case 5:
			m_avgassists = atof(it->c_str());
			break;
		case 11:
			m_starter = atoi(it->c_str());
			break;
		}
	}
	//qDebug() << "name=" << m_name << " mins=" << m_avgmins << " rebs=" << m_avgrebounds << " ast=" << m_avgassists << " starter=" << m_starter;
}

} /* namespace data */
