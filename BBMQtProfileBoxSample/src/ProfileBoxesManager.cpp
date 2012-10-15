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

#include "ProfileBoxesManager.hpp"

#include <QtCore/QVariantList>
#include <QtCore/QVariant>

using namespace bb::cascades;

ProfileBoxesManager* ProfileBoxesManager::m_profileBoxesManagerInstance = NULL;

ProfileBoxesManager &ProfileBoxesManager::getInstance()
{
  if(!m_profileBoxesManagerInstance) {
    m_profileBoxesManagerInstance = new ProfileBoxesManager();
  }

  return *m_profileBoxesManagerInstance;
}

ProfileBoxesManager::ProfileBoxesManager()
{
  m_profileBoxesDataModel = new QListDataModel<ProfileBox*>();
  m_profileBoxesDataModel->setParent(this);
}

QListDataModel<ProfileBox*>* ProfileBoxesManager::getDataModel()
{
  return getInstance().m_profileBoxesDataModel;
}

void ProfileBoxesManager::addNewProfileBox(ProfileBox *newProfileBox) 
{
  getInstance().m_profileBoxesDataModel->append(newProfileBox);
}

void ProfileBoxesManager::removeProfileBox(QVariantList indexPath)
{
  getInstance().m_profileBoxesDataModel->removeAt(indexPath.last().toInt());
}
