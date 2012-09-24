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


#ifndef APP_H
#define APP_H

#include <QObject>


class App : public QObject
{
    Q_OBJECT

public:
    App();

Q_INVOKABLE int clickedRock(int user1, int user2);
Q_INVOKABLE int clickedPaper(int user1, int user2);
Q_INVOKABLE int clickedScissors(int user1, int user2);
Q_INVOKABLE int codeWinning(int record);


};

#endif // ifndef APP_H
