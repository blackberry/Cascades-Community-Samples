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

#include "Contact.hpp"
#include <bb/platform/bbm/Contact>



Contact::Contact(bb::platform::bbm::Contact contact) : m_bbmspContact(contact){
}

QString Contact::getHandle() {
	return m_bbmspContact.handle();
}

QString Contact::getPpid() {
	return m_bbmspContact.ppId();
}

QString Contact::getAppVersion() {
	return m_bbmspContact.applicationVersion();
}

int Contact::getBbmSdkVersion() {
	return m_bbmspContact.sdkVersion();
}

QString Contact::getDisplayName() {
	return m_bbmspContact.displayName();
}

QString Contact::getPersonalMessage() {
	return m_bbmspContact.personalMessage();
}

QString Contact::getStatusMessage() {
	return m_bbmspContact.statusMessage();
}
