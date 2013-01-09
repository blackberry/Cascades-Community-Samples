/*
* Copyright (c) 2012, 2013 Brian Scheirer
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

// Default empty project template
#include "RPSLite.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

using namespace bb::cascades;

RPSLite::RPSLite(bb::cascades::Application *app)
: QObject(app)
{
    // create scene document from main.qml asset
    // set parent to created document to ensure it exists for the whole application lifetime
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("rpscp", this);

    // create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();
    // set created root object as a scene
    app->setScene(root);
}

int RPSLite::clickedRock(int user1, int user2)
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

int RPSLite::clickedPaper(int user1, int user2)
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

int RPSLite::clickedScissors(int user1, int user2)
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

int RPSLite::codeWinning(int record)
{
	int add1;
	add1 = record+1;
	return add1;
}
