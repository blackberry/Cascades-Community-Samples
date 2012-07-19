/* Copyright (c) 2012 Research In Motion Limited.
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
#ifndef NAVIGATOR_H
#define NAVIGATOR_H


#include <bb/cascades/NavigationPane>

using namespace bb::cascades;

namespace bb
{
    namespace cascades
    {
        class NavigationPane;
    }
}

class Navigator {

private:
	Navigator();
	static Navigator* _instance;
	NavigationPane* _navpane;

public:
	static Navigator* getInstance();

	void setNavigationPane(NavigationPane* navpane);
	NavigationPane* getNavigationPane();

};

#endif // ifndef NAVIGATOR_H
