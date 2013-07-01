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

#ifndef MYLIBRARY_HPP_
#define MYLIBRARY_HPP_

#include <bb/cascades/Page>
#include <QObject>

namespace custom {

class Q_DECL_EXPORT MyLibrary : public bb::cascades::Page {
	Q_OBJECT
public:
	MyLibrary(QObject * parent = NULL);
	virtual ~MyLibrary();
};

} /* end namespace custom */

#endif /* MYLIBRARY_HPP_ */
