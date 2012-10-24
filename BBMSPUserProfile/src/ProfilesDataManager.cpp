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

#include <QtCore/QVariantList>
#include <QtCore/QVariant>

#include "EventHandler.hpp"
#include "ProfilesDataManager.hpp"
#include <errno.h>

using namespace bb::cascades;

ProfilesDataManager* ProfilesDataManager::m_profilesDataManagerInstance = NULL;


ProfilesDataManager*
ProfilesDataManager::getInstance()
{
  if (!m_profilesDataManagerInstance) {
    m_profilesDataManagerInstance = new ProfilesDataManager();
  }
  return m_profilesDataManagerInstance;
}

ProfilesDataManager::ProfilesDataManager()
{
  EventHandler::getInstance()->getContactList();

  // Initialize the (custom list) data model and the user profile.
  m_contactsDataModel = new QListDataModel<Contact*>();
  m_contactsDataModel->setParent(this);
  m_userProfile = new UserProfile();
}

UserProfile*
ProfilesDataManager::getUserProfile()
{
  return m_userProfile;
}

QListDataModel<Contact*>*
ProfilesDataManager::getDataModel()
{
  return m_contactsDataModel;
}

void
ProfilesDataManager::contactListReceived(bool isSuccessful, bbmsp_contact_list_t* list)
{
  qDebug() << "Contact list callback - contacts received: " << isSuccessful;

  // Take bbmsp_contact_list, parse into Contact qObject and place into
  // QListDataModel for ListView to use

  int size = bbmsp_contact_list_get_size(list);
  bbmsp_contact_t** contactArray = (bbmsp_contact_t**) malloc(
      sizeof(bbmsp_contact_t*) * size);

  qDebug() << "list size" << size << "get all contacts call"
      << bbmsp_contact_list_get_all_contacts(list, contactArray);

  m_contactsDataModel->clear();

  for (int i = 0; i < size; i++) {
    bbmsp_contact_t* currentContact = contactArray[i];
    Contact* bbm_contact = new Contact(currentContact);
    m_contactsDataModel->append(bbm_contact);
    qDebug() << "contact: " << bbm_contact->getDisplayName();
  }
}

void
ProfilesDataManager::updateContact(bbmsp_contact_t* contact, bbmsp_presence_update_types_t updateType){
  qDebug() << "UPDATE CONTACT CALLBACK";

  Contact* bbm_contact = new Contact(contact);
  QString handle = bbm_contact->getHandle();

  switch (updateType) {
  case BBMSP_DISPLAY_NAME:
  case BBMSP_DISPLAY_PICTURE:
  case BBMSP_PERSONAL_MESSAGE:
  case BBMSP_STATUS:
    //replace contact in data model
    for (int i = 0; i<m_contactsDataModel->size(); i++){
      QVariantList currentIndexpath = QVariantList();
      currentIndexpath.append(i);
      QVariant item = m_contactsDataModel->data(currentIndexpath);
      Contact* currentContact = item.value<Contact*>();
      QString currentContactHandle = currentContact->getHandle();

      if (currentContactHandle.compare(handle) == 0) {
        m_contactsDataModel->replace(i, bbm_contact);
        break;
      }
    }

    emit contactUpdated(bbm_contact);
    break;

  case BBMSP_INSTALL_APP:
    //add contact to data model
    m_contactsDataModel->append(bbm_contact);
    break;

  case BBMSP_UNINSTALL_APP:
    //remove contact from data model
    for (int i = 0; i<m_contactsDataModel->size(); i++){
      QVariantList currentIndexpath = QVariantList();
      currentIndexpath.append(i);
      QVariant item = m_contactsDataModel->data(currentIndexpath);
      Contact* currentContact = item.value<Contact*>();
      QString currentContactHandle = currentContact->getHandle();

      if (currentContactHandle.compare(handle) == 0) {
        m_contactsDataModel->removeAt(i);
        break;
      }
    }

    break;

  default:
    qDebug() << "ERROR: Contact Manager - unknown presence event type";
    break;
  }
}
