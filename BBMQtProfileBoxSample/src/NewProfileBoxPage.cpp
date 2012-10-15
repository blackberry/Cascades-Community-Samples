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

#include "NewProfileBoxPage.hpp"

#include <bb/cascades/QmlDocument>
#include <bb/cascades/Control>
#include <bb/cascades/Button>

#include "ProfileBoxesManager.hpp"

using namespace bb::cascades;


NewProfileBoxPage::NewProfileBoxPage()
  : m_profileBox()
{
  // Register the profile box icons
  m_profileBox.registerIcons();

  QmlDocument* qmlContent = QmlDocument::create("asset:///NewProfileBoxPage.qml");
  if (qmlContent) {
    Control* content = qmlContent->createRootObject<Control>();
    this->setContent(content);
  }

  m_profileBoxTextField = this->findChild<TextField*>("profileBoxTextField");
  m_iconsRadioGroup = this->findChild<RadioGroup*>("iconsRadioGroup");

  Button* saveProfileBoxButton = this->findChild<Button*>("saveProfileBoxButton");
  QObject::connect(saveProfileBoxButton,
                   SIGNAL(clicked()),
                   this,
                   SLOT(createNewProfileBox()));
}

void NewProfileBoxPage::createNewProfileBox()
{
  m_profileBox.createItem(m_profileBoxTextField->text(),
                          m_iconsRadioGroup->selectedOption()->objectName());
}
