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
#ifndef RPSLite_HPP_
#define RPSLite_HPP_

#include <QObject>

namespace bb { namespace cascades { class Application; }}

/*!
 * @brief Application pane object
 *
 *Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class RPSLite : public QObject
{
    Q_OBJECT
public:
    RPSLite(bb::cascades::Application *app);

    Q_INVOKABLE int clickedRock(int user1, int user2);
    Q_INVOKABLE int clickedPaper(int user1, int user2);
    Q_INVOKABLE int clickedScissors(int user1, int user2);
    Q_INVOKABLE int codeWinning(int record);

    virtual ~RPSLite() {}
};


#endif /* RPSLite_HPP_ */
