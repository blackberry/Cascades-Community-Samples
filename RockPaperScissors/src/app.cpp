/*
* Copyright (c) 2012 Brian Scheirer
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

#include "app.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

App::App()
{
    QmlDocument *qml = QmlDocument::create("main.qml");
    qml->setContextProperty("app", this);
    
    AbstractPane *root = qml->createRootNode<AbstractPane>();
    Application::setScene(root);
}

int App::clickedRock(int user1, int user2)
{
	int winner;

    if((user1 == 1) && (user2 == 2))
    winner=2;
    else if ((user1 == 1) && (user2 == 3))
    winner=1;
    else if ((user1 == 1) && (user2 == 1))
    winner=0;

	return (winner);

}

int App::clickedPaper(int user1, int user2)
{
	int winner;

    if((user1 == 2) && (user2 == 2))
    winner=0;
    else if ((user1 == 2) && (user2 == 3))
    winner=2;
    else if ((user1 == 2) && (user2 == 1))
    winner=1;

	return (winner);

}

int App::clickedScissors(int user1, int user2)
{
	int winner;

    if((user1 == 3) && (user2 == 2))
    winner=1;
    else if ((user1 == 3) && (user2 == 3))
    winner=0;
    else if ((user1 == 3) && (user2 == 1))
    winner=2;

	return (winner);

}

int App::codeWinning(int record)
{
	int add1;
	add1 = record+1;
	return add1;
}


