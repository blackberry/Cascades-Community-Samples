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

#ifndef GLOBAL_HPP_
#define GLOBAL_HPP_

#include <bb/platform/bbm/Context>
#include <bb/cascades/NavigationPane>



class Global {

public:
	static Global* instance();
	bb::platform::bbm::Context* getContext();
	void setContext(bb::platform::bbm::Context *context);

private:
	bb::platform::bbm::Context * m_context;
	  QString m_uuid;
};


#endif /* GLOBAL_HPP_ */
