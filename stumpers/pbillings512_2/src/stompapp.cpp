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
#include "stompapp.h"

#include <bb/UIToolkitSupport>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/Page>
#include <QSettings>

using namespace bb::cascades;

stompapp::stompapp()
{

// Normal loading of files.

  QmlDocument *qml = QmlDocument::create("asset:///main.qml");
  qml->setContextProperty("stomp", this);

  if (!qml->hasErrors())
  {
    Page *appPage = qml->createRootObject<Page>();
    if (appPage)
    {
      Application::instance()->setScene(appPage);
      mHiddenContainer = appPage->findChild<Container*>("hiddenContainer");
      qDebug() << mHiddenContainer;

    }
  }

  // Let's disable the Application menu so that it does not interfere with our costume menu
  Application::instance()->setMenuEnabled(false);

  // Getting the signal forom UI Toolkit support
  bb::UIToolkitSupport *m_pUIToolkitSupport = new bb::UIToolkitSupport(this);
  connect(m_pUIToolkitSupport, SIGNAL(swipedDown (bb::SwipeDownMode::Type)),
      this, SLOT(onSwipedDown (bb::SwipeDownMode::Type)));

}

void stompapp::onSwipedDown(bb::SwipeDownMode::Type swipeDownMode)
{
  // When we get the onSwiped down signal, let's tell the QML we want to show the menu
  if (mHiddenContainer->isVisible())
    mHiddenContainer->setVisible(false);
  else
    mHiddenContainer->setVisible(true);

}

stompapp::~stompapp()
{
}
