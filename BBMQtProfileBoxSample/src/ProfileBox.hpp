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

#ifndef PROFILEBOX_HPP_
#define PROFILEBOX_HPP_

#include <bb/platform/bbm/ProfileBox>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QMetaType>
#include <QtCore/QVariant>

class ProfileBox : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString text READ getText WRITE setText NOTIFY textChanged)
  Q_PROPERTY(QString iconPath READ getIconPath WRITE setIconPath NOTIFY iconPathChanged)

signals:
  void textChanged(QVariant);
  void iconPathChanged(QVariant);


public:
  ProfileBox();

  // Registers all of the icons.
  void registerIcons();

  QString getText();
  QString getIconPath();

  void setText(QString text);
  void setIconPath(QString iconPath);

  // Creates an item with \a text and \a iconPath.
  void createItem(const QString& text, const QString& iconPath);

private:
  // Registers the icon at \a path as \a iconId.
  void registerIcon(const QString& path, int iconId);

  QString m_text;
  QString m_iconPath;
  bb::platform::bbm::ProfileBox* m_profileBox;
};

Q_DECLARE_METATYPE(ProfileBox *);

#endif /* PROFILEBOX_HPP_ */
