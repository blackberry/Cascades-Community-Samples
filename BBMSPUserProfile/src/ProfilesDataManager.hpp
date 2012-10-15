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

#ifndef PROFILESDATAMANAGER_HPP_
#define PROFILESDATAMANAGER_HPP_

#include <QtCore/QObject>
#include <bb/cascades/QListDataModel>

#include "Contact.hpp"
#include "UserProfile.hpp"

class ProfilesDataManager : public QObject {
    Q_OBJECT

signals:
  void contactUpdated(Contact* contact);

public:
  static ProfilesDataManager* getInstance();
  UserProfile* getUserProfile();

  bb::cascades::QListDataModel<Contact*>* getDataModel();
  void contactListReceived(bool isSuccessful,bbmsp_contact_list_t* list);
  void updateContact(bbmsp_contact_t* contact, bbmsp_presence_update_types_t updateType);

private:
  ProfilesDataManager();
  virtual ~ProfilesDataManager(){}

  // Singleton which contains all of the user's and user's contacts' Profile data.
  static ProfilesDataManager* m_profilesDataManagerInstance;

  // Using this data model will initiate immediate changes to the UI component
  // displaying it.
  bb::cascades::QListDataModel<Contact*>* m_contactsDataModel;

  // This user's profile
  UserProfile* m_userProfile;
};

#endif /* PROFILESDATAMANAGER_HPP_ */
