/* Copyright (c) 2013 Research In Motion Limited.
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

#include <bb/cascades/Container>
#include <bb/cascades/Color>
#include <bb/cascades/Label>
#include <bb/cascades/SystemDefaults>

#include "MyLibrary.hpp"

using namespace bb::cascades;

namespace custom {

MyLibrary::MyLibrary(QObject *parent)
	: Page(parent)
{
	// TODO Auto-generated constructor stub
	Label * lbl = Label::create().textStyle(SystemDefaults::TextStyles::bigText())
		.vertical(VerticalAlignment::Center).horizontal(HorizontalAlignment::Center)
		.text("Hello from MyLibrary");

	Container * c = Container::create().parent(this).background(Color::LightGray).add(lbl);
	Page::setContent(c);
}

MyLibrary::~MyLibrary() {
	// TODO Auto-generated destructor stub
}

} /* end namespace custom */
