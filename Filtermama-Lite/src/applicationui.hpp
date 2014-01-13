/* Copyright (c) 2013 Chad Tetreault
 * http://palebanana.com - @chadtatro
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

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <bb/system/InvokeManager>

namespace bb {
namespace cascades {
class Application;
class LocaleHandler;
}
}

namespace bb {
namespace system {
class InvokeManager;
}
}

class QTranslator;

/*!
 * @brief Application object
 *
 *
 */

class ApplicationUI: public QObject {
	Q_OBJECT

public:
	ApplicationUI(bb::cascades::Application *app);
	virtual ~ApplicationUI() {
	}

	// Converts the passed QString to an UTF-8 encoded QByteArray
	Q_INVOKABLE
	QByteArray encodeQString(const QString& toEncode) const;

	// convert junk to base64, and stuff
	Q_INVOKABLE
	QString copyToData(const QString& path) const;

	// save the photo
	Q_INVOKABLE
	QString savePhoto(const QString& base64);

	// get file listing for feed
	Q_INVOKABLE
	QString getPhotos() const;

	// when new photo is saved... for the thumbnail generation
	Q_SIGNAL
	void photosChanged(QString path);

	// tells qml to turn off the spinner
	Q_SIGNAL
	void spinnerOff();

	// invoke photo viewer card
	Q_INVOKABLE
	void invokePhotoViewer(const QString& path) const;

public Q_SLOTS:
	void shareFile(QString fileName);
	void onArmed();

	// This method is called to invoke another application with the current configuration
	void invoke(const QString &target, const QString &action,
			const QString &mimetype, const QString &uri);

private slots:
	void onSystemLanguageChanged();

private:
	QTranslator* m_pTranslator;
	bb::cascades::LocaleHandler* m_pLocaleHandler;
	bb::system::InvokeManager* m_invokeManager;
};

#endif /* ApplicationUI_HPP_ */
