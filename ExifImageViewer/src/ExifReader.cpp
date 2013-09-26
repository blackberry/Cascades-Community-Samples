/* Copyright (c) 2013 BlackBerry Limited.
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

#include "ExifReader.hpp"
#include "ExifReader_p.hpp"

#include <bb/cascades/Image>

#include <QDebug>
#include <QtConcurrentRun>

#include <libexif/exif-data.h>
#include <libexif/exif-tag.h>

namespace bb {
namespace extensions {

ExifReaderPrivate::ExifReaderPrivate(ExifReader *parent) :
		QObject(parent), q_ptr(parent), rotation(0) {
}
ExifReaderPrivate::~ExifReaderPrivate() {
}

void ExifReaderPrivate::readImage(QUrl imageSource) {
	// WARNING: this is run from another (non-ui) thread, use only thread-safe calls
	qreal rotation = ExifReader::getImageRotation(imageSource);
	QMetaObject::invokeMethod(this, "readImageFinished", Qt::QueuedConnection, Q_ARG(qreal, rotation));
}
void ExifReaderPrivate::readImageFinished(qreal r) {
	Q_Q(ExifReader);
	// slot will be scheduled on the main thread's event loop, so we are now safe to access/modify instance variables

	if (r < 0) {
		rotation = 0;
		qDebug() << "Unable to read rotation for" << imageSource;
	} else
		rotation = r;

	emit q->rotationChanged();
}

ExifReader::ExifReader(QObject * parent) :
		QObject(parent), d_ptr(new ExifReaderPrivate(this)) {
}

ExifReader::~ExifReader() {
}

qreal ExifReader::rotation() const {
	Q_D(const ExifReader);
	return d->rotation;
}

QUrl ExifReader::imageSource() const {
	Q_D(const ExifReader);
	return d->imageSource;
}
void ExifReader::setImageSource(QUrl imageSource) {
	Q_D(ExifReader);
	if (imageSource == d->imageSource)
		return;

	d->imageSource = imageSource;
	emit imageSourceChanged();

	if (imageSource.isEmpty()) {
		d->rotation = 0;
		emit rotationChanged();
	}
	else {
		QtConcurrent::run(d, &ExifReaderPrivate::readImage, imageSource);
	}
}

qreal ExifReader::getImageRotation(QUrl _url) {
	qreal rotation = -1;
	QString url = _url.toLocalFile();
	if (url.endsWith("jpg", Qt::CaseInsensitive) || url.endsWith("tiff", Qt::CaseInsensitive)) {

		// hold onto this so constData() is valid
		QByteArray ba = url.toLocal8Bit();
		ExifData* exifData = exif_data_new_from_file(ba.constData());
		if (exifData != NULL) {
			ExifEntry* exifEntry = exif_content_get_entry(exifData->ifd[EXIF_IFD_0],
					EXIF_TAG_ORIENTATION);

			if (exifEntry != NULL) {
				char value[256] = { 0, };
				memset(value, 0, sizeof(value));
				exif_entry_get_value(exifEntry, value, sizeof(value));

				//qDebug() << "Orientation: " << value << endl;
				QString orient = QString::fromLocal8Bit(value);
				if (orient.compare("bottom-right", Qt::CaseInsensitive) == 0) {
					rotation = 180.0;
				} else if (orient.compare("right-top", Qt::CaseInsensitive) == 0) {
					rotation = 90.0;
				} else if (orient.compare("left-bottom", Qt::CaseInsensitive) == 0) {
					rotation = 270.0;
				}

				delete exifEntry;
			}
			delete exifData;
		}
	}
	return rotation;
}

} /* namespace extensions */
} /* namespace bb */
