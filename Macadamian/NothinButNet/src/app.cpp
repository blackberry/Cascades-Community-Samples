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

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/ActionItem>
#include <bb/cascades/Page>
#include <bb/cascades/Label>
#include <bb/cascades/Container>
#include <bb/cascades/TranslateTransition>
#include <bb/cascades/LayoutProperties>
#include <bb/cascades/AbsoluteLayoutProperties>

#include <sstream>
#include <iomanip>

#include "Matchups.h"
#include "app.hpp"
#include "customTimer.hpp"
#include "openGL/ReboundsGraph.hpp"
#include "openGL/TwitterStreamGraph.hpp"
#include "openGL/MinutesPerGameGraph.hpp"
#include "openGL/TwitterLiveData.h"
#include "openGL/GLThread.h"

using namespace bb::cascades;

namespace {
	std::string toString( float value )
	{
		std::ostringstream ss;
		ss << std::setprecision(3) << value;
		return ss.str();
	}

	float scale(float val, float min, float max, float newMin, float newMax)
	{
		float f = (newMax-newMin);
		float g = (val-min);
		float t = (max - min);
		float fin = ((f * g) / t) + newMin;
		return fin;
	}
}

App::App()
: m_teamSetIndex(0)
{
    GLThread::initInstance(1280,768);
    GLThread::instance()->start();

	qmlRegisterType<CustomTimer>("CustomTimer", 1, 0, "CustomTimer");
	qmlRegisterType<ReboundsGraph>("ReboundsGraph", 1, 0, "ReboundsGraph");
	qmlRegisterType<TwitterStreamGraph>("TwitterStreamGraph", 1, 0, "TwitterStreamGraph");
    qmlRegisterType<MinutesPerGameGraph>("MinutesPerGameGraph", 1, 0, "MinutesPerGameGraph");

    QmlDocument *qml = QmlDocument::create("home.qml");
    qml->setContextProperty("cs", this);

    AbstractPane *root = qml->createRootNode<AbstractPane>();
	navigationPane = NavigationPane::create();
	navigationPane->setBackButtonsVisible(false);
	navigationPane->push(root);

	Application::setScene(navigationPane);

    teamABezier = TeamCurve(100, QPoint(650,620), QPoint(1000,500), QPoint(960,300), QPoint(800,100));
    teamBBezier = TeamCurve(100, QPoint(320,620), QPoint(-20,500), QPoint(00,300), QPoint(180,100));
	data::Matchups::instance()->firstRound(m_east,m_west);
}

int App::getWidth(QString team)
{
	QUuid teamid = data::Matchups::instance()->findByName(team);
	if(!team.isNull())
		return data::Matchups::instance()->getPixelWidth(teamid);

	return 600;
}

int App::getHeight(QString team)
{
	QUuid teamid = data::Matchups::instance()->findByName(team);
	if(!team.isNull())
		return data::Matchups::instance()->getPixelHeight(teamid);

	return 48;
}

int App::getTweetTotal()
{
	return data::Matchups::instance()->getTotalTweets();
}

void  App::refresh()
{
	data::Matchups::instance()->refreshSocial();
}

void App::selectTeamsFromRotation(float rotation) {
//	qDebug() << " setTeam.r: " << rotation;
	m_teamSetIndex = ( (int)(-22.5-rotation)/45 + 800000 ) % 8;
//	qDebug() << " setTeam.i: " << m_teamSetIndex;
	TwitterLiveData::setTeamSetIndex(m_teamSetIndex);
}

int App::value()
{
    return m_iValue;
}

void App::setValue(int i)
{
    m_iValue = i;
    emit valueChanged(m_iValue);
}

void App::reset()
{
    m_iValue = 0;
    emit valueChanged(m_iValue);
}

void App::goForward(QString screenname)
{
	qDebug() << " going forward to: " << screenname;

	loadPage(screenname);
}

void App::goBack()
{
	qDebug() << " going back";
	navigationPane->popAndDelete();
	GLThread::instance()->previousRenderer();
}

void App::loadPage(QString pagename)
{
    QmlDocument *qml = QmlDocument::create(pagename);
    qml->setContextProperty("cs", this);
    AbstractPane *root = qml->createRootNode<AbstractPane>();
    navigationPane->push(root);

    if(pagename.compare("stats.qml",Qt::CaseInsensitive)==0)
    {
    	QUuid teamAid, teamBid;
    	switch(m_teamSetIndex)
    	{
    	case 0:
			teamAid = data::Matchups::instance()->findByName("heat");
			teamBid = data::Matchups::instance()->findByName("knicks");
			break;
    	case 1:
			teamAid = data::Matchups::instance()->findByName("pacers");
			teamBid = data::Matchups::instance()->findByName("magic");
			break;
    	case 2:
			teamAid = data::Matchups::instance()->findByName("celtics");
			teamBid = data::Matchups::instance()->findByName("hawks");
			break;
    	case 3:
			teamAid = data::Matchups::instance()->findByName("spurs");
			teamBid = data::Matchups::instance()->findByName("suns");
			break;
    	case 4:
			teamAid = data::Matchups::instance()->findByName("thunder");
			teamBid = data::Matchups::instance()->findByName("mavericks");
			break;
    	case 5:
			teamAid = data::Matchups::instance()->findByName("lakers");
			teamBid = data::Matchups::instance()->findByName("nuggets");
			break;
    	case 6:
			teamAid = data::Matchups::instance()->findByName("clippers");
			teamBid = data::Matchups::instance()->findByName("grizzlies");
			break;
    	case 7:
			teamAid = data::Matchups::instance()->findByName("bulls");
			teamBid = data::Matchups::instance()->findByName("sixers");
			break;
    	}

    	data::Team* teamA = data::Matchups::instance()->getTeam(teamAid);
    	data::Team* teamB = data::Matchups::instance()->getTeam(teamBid);
    	if(!teamA || !teamB)
    	{
    		qDebug() << "teams not found";
    		return;
    	}

    	// get the name for Team A
        Label* teamAName = root->findChild<Label*>("teamAName");
        if(teamAName)
        {
        	teamAName->setProperty("text", teamA->name());
        }

        // get the name for Team B
        Label* teamBName = root->findChild<Label*>("teamBName");
        if(teamBName)
        {
        	teamBName->setProperty("text", teamB->name());
        }

    	PrepMinutesChart(root, teamA, teamB);
    	PrepReboundsChart(root, teamA, teamB);
    }
}

void App::PrepMinutesChart(AbstractPane *root, data::Team *teamA, data::Team *teamB)
{
	QList<data::Player> teamAStarters = teamA->getStartingLineup();
	QList<data::Player> teamBStarters = teamB->getStartingLineup();
	QList<data::Player>::iterator it;
	float min=100, max=0;
	for(it=teamAStarters.begin(); it!=teamAStarters.end(); it++)
	{
		if(it->averageMinutes() < min)
			min = it->averageMinutes();
		if(it->averageMinutes() > max)
			max = it->averageMinutes();
	}
	for(it=teamBStarters.begin(); it!=teamBStarters.end(); it++)
	{
		if(it->averageMinutes() < min)
			min = it->averageMinutes();
		if(it->averageMinutes() > max)
			max = it->averageMinutes();
	}
	UpdateMinutesPlayer(TeamA,root,"teamAPlayer1Container",teamAStarters[0].name(),teamAStarters[0].averageMinutes(),scale(teamAStarters[0].averageMinutes(),min,max,10,90));
	UpdateMinutesPlayer(TeamA,root,"teamAPlayer2Container",teamAStarters[1].name(),teamAStarters[1].averageMinutes(),scale(teamAStarters[1].averageMinutes(),min,max,10,90));
	UpdateMinutesPlayer(TeamA,root,"teamAPlayer3Container",teamAStarters[2].name(),teamAStarters[2].averageMinutes(),scale(teamAStarters[2].averageMinutes(),min,max,10,90));
	UpdateMinutesPlayer(TeamA,root,"teamAPlayer4Container",teamAStarters[3].name(),teamAStarters[3].averageMinutes(),scale(teamAStarters[3].averageMinutes(),min,max,10,90));
	UpdateMinutesPlayer(TeamA,root,"teamAPlayer5Container",teamAStarters[4].name(),teamAStarters[4].averageMinutes(),scale(teamAStarters[4].averageMinutes(),min,max,10,90));
	UpdateMinutesPlayer(TeamB,root,"teamBPlayer1Container",teamBStarters[0].name(),teamBStarters[0].averageMinutes(),scale(teamBStarters[0].averageMinutes(),min,max,10,90));
	UpdateMinutesPlayer(TeamB,root,"teamBPlayer2Container",teamBStarters[1].name(),teamBStarters[1].averageMinutes(),scale(teamBStarters[1].averageMinutes(),min,max,10,90));
	UpdateMinutesPlayer(TeamB,root,"teamBPlayer3Container",teamBStarters[2].name(),teamBStarters[2].averageMinutes(),scale(teamBStarters[2].averageMinutes(),min,max,10,90));
	UpdateMinutesPlayer(TeamB,root,"teamBPlayer4Container",teamBStarters[3].name(),teamBStarters[3].averageMinutes(),scale(teamBStarters[3].averageMinutes(),min,max,10,90));
	UpdateMinutesPlayer(TeamB,root,"teamBPlayer5Container",teamBStarters[4].name(),teamBStarters[4].averageMinutes(),scale(teamBStarters[4].averageMinutes(),min,max,10,90));
}

void App::PrepReboundsChart(AbstractPane *root, data::Team *teamA, data::Team *teamB)
{
	ReboundsGraph* reboundsGraph = root->findChild<ReboundsGraph*>("reboundsGraph");
	if (reboundsGraph)
	{
		reboundsGraph->setTeamA(teamA->name());
		reboundsGraph->setTeamB(teamB->name());
	}
}

void App::UpdateMinutesPlayer(TeamType type, AbstractPane *root, QString qmlId, QString playername, float minutes, int pct)
{
    if(pct<0)
        pct = 0;

    if(pct>99)
        pct = 99;

    QObject *player = root->findChild<QObject*>(qmlId);
    if (player)
    {
        Label* nameLabel = player->findChild<Label*>("name");
        if(nameLabel)
        {
            nameLabel->setProperty("text", playername);
            if(type==TeamA)
            	nameLabel->textStyle()->setColor(Color::Red);
            else
            	nameLabel->textStyle()->setColor(Color::Yellow);
        }

        Label* minutesLabel = player->findChild<Label*>("minutes");
        if(minutesLabel)
        {
            minutesLabel->setProperty("text", toString(minutes).c_str());
            if(type==TeamA)
            	minutesLabel->textStyle()->setColor(Color::Red);
            else
            	minutesLabel->textStyle()->setColor(Color::Yellow);
        }

        GLfloat toX = teamABezier[pct*2];
        GLfloat toY = teamABezier[(pct*2)+1];
        if(type==TeamB)
        {
            toX = teamBBezier[pct*2];
            toY = teamBBezier[(pct*2)+1];
        }

        ((AbsoluteLayoutProperties*)((Container*)player)->layoutProperties())->setPositionX(toX);
        ((AbsoluteLayoutProperties*)((Container*)player)->layoutProperties())->setPositionY(toY);
        /*
        float curX = ((AbsoluteLayoutProperties*)((Container*)player)->layoutProperties())->positionX();
        float curY = ((AbsoluteLayoutProperties*)((Container*)player)->layoutProperties())->positionY();

        TranslateTransition* translateAnimation = TranslateTransition::create((Container*)player)
                               .toX(toX-curX)
                               .toY(toY-curY)
                               .duration(2700)
                               .autoDeleted(true);
        translateAnimation->play();
        */

    }
}

QPoint App::CalculateBezierPoint(GLfloat t,QPoint p0, QPoint p1, QPoint p2, QPoint p3)
{
    GLfloat u = 1-t;
    GLfloat tt = t*t;
    GLfloat uu = u*u;
    GLfloat uuu = uu * u;
    GLfloat ttt = tt * t;

    QPoint p = uuu * p0; //first term
    p += 3 * uu * t * p1; //second term
    p += 3 * u * tt * p2; //third term
    p += ttt * p3; //fourth term

    return p;
}

GLfloat* App::TeamCurve(int segments, QPoint p0, QPoint p1, QPoint p2, QPoint p3)
{
    GLfloat *vertices = new GLfloat[segments*2];

    for(int i = 0, j=0; i <= segments; i++)
    {
      GLfloat t = i / (float) segments;
      QPoint pixel = CalculateBezierPoint(t, p0, p1, p2, p3);
      vertices[j++] = pixel.x();
      vertices[j++] = pixel.y();
    }
    return vertices;
}


