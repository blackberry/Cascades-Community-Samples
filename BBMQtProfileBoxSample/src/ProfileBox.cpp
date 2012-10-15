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

#include <bb/cascades/Container>
#include <bb/cascades/LongPressHandler>

#include <bb/platform/bbm/ProfileBoxItem>
#include "ProfileBox.hpp"
#include "Global.hpp"

using namespace bb::cascades;

static const QString logPrefix("ProfileBox");

ProfileBox::ProfileBox()
{
  m_profileBox = new bb::platform::bbm::ProfileBox(Global::instance()->getContext(), this);
}

void ProfileBox::registerIcons()
{
  QString imageDir(QDir::currentPath() + "/app/native/assets/images/");
  registerIcon(imageDir + "apple.png", 1);
  registerIcon(imageDir + "pear.png", 2);
  registerIcon(imageDir + "orange.png", 3);
}

void ProfileBox::registerIcon(const QString& path, int iconId)
{
  qDebug() << logPrefix << ": Registering icon id=" << iconId << ", path="
    << path;

  // Read the icon from the file
  QImage image;
  QByteArray iconArray;
  QBuffer buffer(&iconArray);
  buffer.open(QIODevice::WriteOnly);
  if(not image.load(path)) {
    qDebug() << logPrefix << ": Failed to load icon";
    return;
  }
  qDebug() << logPrefix << ": Icon loaded";
  image.save(&buffer, "PNG");

  // Create the icon object and register the icon
  const bool result = m_profileBox->requestRegisterIcon(
    iconId, bb::platform::bbm::ImageType::Png, iconArray);
  qDebug() << logPrefix << ": Icon registered; result=" << result;
}

QString ProfileBox::getText()
{
  return m_text;
}

QString ProfileBox::getIconPath()
{
  return m_iconPath;
}

void ProfileBox::setText(QString text)
{
  m_text = text;
}

void ProfileBox::setIconPath(QString iconPath)
{
  m_iconPath = iconPath;
}

void ProfileBox::createItem(const QString& text, const QString& iconPath)
{
  qDebug() << logPrefix << ": Adding item";
  m_text = text;
  m_iconPath = iconPath;

  // If no icon was selected, add the item right away
  if(m_iconPath == "none") {
    const bool result = m_profileBox->requestAddItem(m_text, QString("cookie"));
    qDebug() << logPrefix << ": Added item with no icon; result=" << result;
    return;
  }


  // Icon was selected. Determine its ID
  int iconId;
  if(m_iconPath == "images/apple.png") {
    iconId = 1;
  } else if(m_iconPath == "images/pear.png") {
    iconId = 2;
  } else if(m_iconPath == "images/orange.png") {
    iconId = 3;
  } else {
    qDebug() << logPrefix << ": Item could not be added because icon ID could not"
      " be determined; iconPath=" << m_iconPath;
    return;
  }

  // Add the item
  const bool result = m_profileBox->requestAddItem(
    m_text, iconId, QString("cookie"));
  qDebug() << logPrefix << ": Added item with icon; result="
    << result;
}
