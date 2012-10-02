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

#include "Carousel.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include <QPropertyAnimation>

#include <bb/ImageData>
#include <QImage>
#include <QRgb>
#include <QImage>
#include <bb/cascades/Image>

#include <unistd.h>

using namespace bb::cascades;

Carousel::Carousel(bb::cascades::Application *app) :
    QObject(app)
{
  qmlRegisterType<QTimer>("utils", 1, 0, "QTimer");
  qmlRegisterType<QPropertyAnimation>("bb.cascades", 1, 0,
      "QPropertyAnimation");
  // create scene document from main.qml asset
  // set parent to created document to ensure it exists for the whole application lifetime
  QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

  qml->setContextProperty("app", this);
  // create root object for the UI
  AbstractPane *root = qml->createRootObject<AbstractPane>();
  // set created root object as a scene
  app->setScene(root);
}

// Getting the byte array of the string
QByteArray Carousel::getBytes(QString str)
{
  return str.toAscii();
}

// We only want the OutCubic easing-curve, try others,  dare you!
QEasingCurve Carousel::getEase()
{
  return QEasingCurve::OutCubic;
}

// This function is needed by the mirroring algo.
static bb::cascades::Image fromQImage(const QImage &origQImage,
    const QImage &mirroredQImage)
{

  bb::ImageData imageData(bb::PixelFormat::RGBA_Premultiplied,
      origQImage.width(), (origQImage.height() * 1.25) + 2);
  int y = 0;

  unsigned char *dstLine = imageData.pixels();

  for (y = 0; y < origQImage.height(); y++)
  {
    unsigned char * dst = dstLine;
    for (int x = 0; x < imageData.width(); x++)
    {
      QRgb srcPixel = origQImage.pixel(x, y);

      *dst++ = qRed(srcPixel) * qAlpha(srcPixel) / 255;
      *dst++ = qGreen(srcPixel) * qAlpha(srcPixel) / 255;
      *dst++ = qBlue(srcPixel) * qAlpha(srcPixel) / 255;
      *dst++ = qAlpha(srcPixel);
    }
    dstLine += imageData.bytesPerLine();
  }

  for (; y < origQImage.height() + 2; y++)
  {
    unsigned char * dst = dstLine;
    for (int x = 0; x < imageData.width(); x++)
    {
      *dst++ = 0;
      *dst++ = 0;
      *dst++ = 0;
      *dst++ = 0;
    }
    dstLine += imageData.bytesPerLine();
  }

  for (; y < imageData.height(); y++)
  {
    unsigned char * dst = dstLine;
    for (int x = 0; x < imageData.width(); x++)
    {
      QRgb srcPixel = mirroredQImage.pixel(x, (y - 2 - origQImage.height()));
      *dst++ = qRed(srcPixel);
      *dst++ = qGreen(srcPixel);
      *dst++ = qBlue(srcPixel);
      *dst++ = qAlpha(srcPixel);

    }
    dstLine += imageData.bytesPerLine();
  }

  return Image(imageData);

}

// Let's not have all the images mirrored, let's do that in code, and some alpha on them aswell
QVariant Carousel::createMirrorImage(QString inputFName)
{

  if (inputFName.isEmpty())
    return QVariant::fromValue(0);

  char buff[1024];
  QString prefix = QString(getcwd(buff, 1024));
  inputFName = prefix + "/app/native/assets/" + inputFName;

  QImage inputQImage(inputFName);
  QImage mirrored_part = inputQImage.mirrored(false, true);
  QPoint start(0, 0);
  QPoint end(0, mirrored_part.height());
  QLinearGradient gradient(start, end);

  gradient.setColorAt(0.0, Qt::gray);
  gradient.setColorAt(0.22, Qt::black);
  gradient.setColorAt(1.0, Qt::black);
  QImage mask = mirrored_part;
  QPainter painter(&mask);
  painter.fillRect(mirrored_part.rect(), gradient);
  painter.end();

  mirrored_part.setAlphaChannel(mask);
  bb::cascades::Image mirrored_image = fromQImage(inputQImage, mirrored_part);
  return QVariant::fromValue(mirrored_image);

}
