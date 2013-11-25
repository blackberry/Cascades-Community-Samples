/* Copyright (c) 2013 BlackBerry Limited.
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
#include "Navigator.hpp"

Navigator* Navigator::_instance;

Navigator::Navigator() {
	qDebug() << "XXXX Constructing Navigator";
	qDebug() << "XXXX Done constructing Navigator";
}

Navigator* Navigator::getInstance() {
	if (_instance == 0) {
		_instance = new Navigator;
	}
	return _instance;
}

void Navigator::setNavigationPane(NavigationPane* navpane) {
	qDebug() << "XXXX setting NavigationPane in Navigator";
	_navpane = navpane;
	qDebug() << "XXXX done";
}

NavigationPane* Navigator::getNavigationPane() {
	return _navpane;
}

