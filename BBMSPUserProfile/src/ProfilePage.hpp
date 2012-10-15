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

#ifndef PROFILEPAGE_HPP_
#define PROFILEPAGE_HPP_

#include <QtCore/QObject>

#include <bb/cascades/AbstractPane>
#include <bb/cascades/TextArea>
#include <bb/cascades/Page>
#include <bb/cascades/CheckBox>
#include <bb/cascades/ImageView>
#include <bb/cascades/TextField>
#include <bb/cascades/Label>
#include <bb/cascades/ActionItem>

#include "bbmsp/bbmsp_userprofile.h"

class ProfilePage : public bb::cascades::Page
{
  Q_OBJECT

public slots:
  void profileChanged();
  void updateProfile();

public:
  ProfilePage();

private:
  bb::cascades::AbstractPane* m_profileView;

  bb::cascades::TextArea* m_displayName;
  bb::cascades::TextArea* m_statusMessageLabel;
  bb::cascades::TextArea* m_personalMessageLabel;
  bb::cascades::ImageView* m_busyIconImageView;
  bb::cascades::ImageView* m_avatarImageView;
};

#endif /* PROFILEPAGE_HPP_ */
