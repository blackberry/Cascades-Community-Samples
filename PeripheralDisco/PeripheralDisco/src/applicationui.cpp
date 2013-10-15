/**
 * Copyright (c) 2013 BlackBerry Limited.
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
#include "applicationui.hpp"

#include <QDateTime>
#include <QMetaObject>
#include <QSet>

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/TextArea>
#include <bb/cascades/TextField>

#include <bb/system/SystemToast>

#include "PeripheralOracle.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>

using namespace bb::cascades;

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
		QObject(app) {
	_serialConnected = false;
	_serialPossible = false;
	serialFd = -1;

	// prepare the localization
	m_pTranslator = new QTranslator(this);
	m_pLocaleHandler = new LocaleHandler(this);
	if (!QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()),
			this, SLOT(onSystemLanguageChanged()))) {
		// This is an abnormal situation! Something went wrong!
		// Add own code to recover here
		qWarning() << "Recovering from a failed connect()";
	}
	// initial load
	onSystemLanguageChanged();

	// Create scene document from main.qml asset, the parent is set
	// to ensure the document gets destroyed properly at shut down.
	QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);

	qml->setContextProperty("app", this);

	// Create root object for the UI
	AbstractPane *root = qml->createRootObject<AbstractPane>();

	// Set created root object as the application scene
	app->setScene(root);

	// Create a PeripheralOracle - and connect all the signals.
	peripheralOracle = new PeripheralOracle(this);

	bool connects = true;

	connects &= connect(peripheralOracle,
			SIGNAL(peripheralConnected(int,PeripheralDetail)), this,
			SLOT(onPeripheralConnected(int,PeripheralDetail)));
	connects &= connect(peripheralOracle,
			SIGNAL(peripheralDisconnected(int,PeripheralDetail)), this,
			SLOT(onPeripheralDisconnected(int,PeripheralDetail)));

	qDebug() << "Connects worked: " << connects;

	// Register interest - and check supported busses.
	QString failMessage;

	QSet<pd_bus_t> supported(peripheralOracle->getSupportedBusses());
	if (!supported.contains(PD_BUS_USB_HOST_MODE)) {
		failMessage += "This device does not support USB host mode. ";
	}

	QSet<pd_class_t> toRegister;
	for (int i = 0; i < PD_CLASS_NUM_SUPPORTED; ++i) {
		toRegister << (pd_class_t)i;
	}

	QSet<pd_class_t> registered(peripheralOracle->registerInterest(toRegister));
	if (registered.size()==0) {
		failMessage += "Could not register for notification on any events. ";
	}

	if (failMessage!="") {
		failMessage += "This application may not be very impressive on this device.";
		toast(failMessage);
	}

}

void ApplicationUI::onSystemLanguageChanged() {
	QCoreApplication::instance()->removeTranslator(m_pTranslator);
	// Initiate, load and install the application translation files.
	QString locale_string = QLocale().name();
	QString file_name = QString("PeripheralDisco_%1").arg(locale_string);
	if (m_pTranslator->load(file_name, "app/native/qm")) {
		QCoreApplication::instance()->installTranslator(m_pTranslator);
	}
}

void ApplicationUI::onPeripheralConnected(int,
		PeripheralDetail detail) {
	qDebug() << "Connected";
	QString now = QDateTime::currentDateTime().toString();
	addToLog("<div><b>Connected " + now + "</b></div>");
	addToLog(detail.toHTML());
	addToLog("<div>&nbsp;</div>");

	checkSerial();
}

void ApplicationUI::onPeripheralDisconnected(int,
		PeripheralDetail detail) {
	qDebug() << "Disconnected";
	QString now = QDateTime::currentDateTime().toString();
	addToLog("<div><b>Disonnected " + now + "</b></div>");
	addToLog(detail.toHTML());
	addToLog("<div>&nbsp;</div>");

	checkSerial();
}

void ApplicationUI::checkSerial() {
	// Ok - this code is lazy. It's hunting for the FIRST serial device it sees.
	// For any non-trivial application - you should check a little harder!
	QMap<int, PeripheralDetail> peripherals = peripheralOracle->peripherals();
	bool serial = false;
	QMap<int, PeripheralDetail>::iterator i(peripherals.begin());
	QMap<int, PeripheralDetail>::iterator end(peripherals.end());

	for (; !serial && i != end; ++i) {
		PeripheralDetail detail(i.value());
		serial |= detail.isUSBSerial();
	}
	if (serial != _serialPossible) {
		_serialPossible = serial;
		emit serialPossibleChanged(serial);
		closeSerial();
	}
}

void ApplicationUI::addToLog(QString toAdd) {
	emit log(toAdd);
}

void ApplicationUI::openSerial() {
	if (serialFd != -1) {
		toast("Serial connection already open...");
		return;
	}

	// We can't actually write this value - so it's our nothing written value.
	lastSerialWrite = 256;

	// Lazily look for the first serial device. You sould be more thorough.
	QMap<int, PeripheralDetail> peripherals = peripheralOracle->peripherals();
	QMap<int, PeripheralDetail>::iterator i(peripherals.begin());
	QMap<int, PeripheralDetail>::iterator end(peripherals.end());

	QString path;

	for (; i != end; ++i) {
		PeripheralDetail detail(i.value());
		if (detail.isUSBSerial()) {
			path = detail.getValue("path").toString();
			break;
		}
	}

	// The path is the location on the filesystem we have to open to get access to
	// that serial device.
	if (path.isEmpty() || path.isNull()) {
		toast("Could not find a serial device with a path...");
		return;
	}
	qDebug() << "Going to open"<< path;

	serialFd = open(path.toAscii().constData(), O_RDWR);
	if (serialFd == -1) {
		toast("Could not open " + path);
		return;
	}

	// This section here sets the input and output baud rate.
	// Do you know what the default is? Does your appliance care?
	struct termios attributes;

	QString error;

	tcgetattr(serialFd, &attributes);
	if (cfsetispeed(&attributes, B9600) < 0) {
		error += "Could not set input speed. ";
	}
	if (cfsetospeed(&attributes, B9600) < 0) {
		error += "Could not set output speed. ";
	}
	if (tcsetattr(serialFd, TCSANOW, &attributes) < 0) {
		error += "Could not apply speeds settings. ";
	}

	if (!error.isEmpty()) {
		toast(error);
	}

	// Using QSocketNotifier so we don't need to poll the file.
	QSocketNotifier * notifier = new QSocketNotifier(serialFd,
			QSocketNotifier::Read, this);
	bool connected = connect(notifier, SIGNAL(activated(int)), this,
			SLOT(dataAvailable(int)));
	qDebug() << "Connect activate" << connected;
	notifier->setEnabled(true);

	QSocketNotifier * errorNotifier = new QSocketNotifier(serialFd, QSocketNotifier::Exception, this);
	connect(notifier,SIGNAL(activated(int)),this, SLOT(dataAvailable(int)));
	errorNotifier->setEnabled(true);

	emit serialConnectedChanged(true);

	// Check the arduino code for an idea of this 'protocol'.
	write(serialFd,"\n\n\n\n\n\n\n\n\n\nRESET\n",16);
}

void ApplicationUI::closeSerial() {
	qDebug() << "closeSerial";
	if (serialFd!=-1) {
		// We send a BYE so that the Arduino knows we aren't connected anymore.
		write(serialFd,"BYE\n",4);
		close(serialFd);
		serialFd = -1;
		emit serialConnectedChanged(false);
	}
}

void ApplicationUI::dataAvailable(int fd) {
	qDebug() << "dataAvailable";
	if (fd != serialFd) {
		// uh oh...
		qDebug() << "Wrong fd";
	} else {
		char buffer[500];
		ssize_t readLength = read(fd, buffer, sizeof(buffer));
		qDebug() << "Read " << readLength;
		if (readLength > 0) {
//			QByteArray message(buffer, readLength);
//			QString toLog("<div><em>In:&nbsp;</em>");
//			toLog.append(message);
//			toLog.append("</div>");
//			addToLog(toLog);
		} else {
			toast(QString("Read %1 bytes").arg(readLength));
		}
	}
}

void ApplicationUI::writeSerial(float f) {
	if (serialFd != -1) {
		char value = (char) (f * 180);
		if (value != lastSerialWrite) {
			// In this protocol, we say SET x\n where x is a byte value, between 0 and 180 (ish)
			lastSerialWrite = value;
			int wrote = write(serialFd, "SET ",4);
			int wrote2 = write(serialFd,&value,1);
			int wrote3 = write(serialFd,"\n",1);
			if (wrote == -1 || wrote2==-1 || wrote3==-1) {
				close(serialFd);
				serialFd = -1;
				emit serialConnectedChanged(false);
				toast("Write failed...");
			}
		}
	}
}

void ApplicationUI::toast(QString message) {
	qDebug() << "Toast" << message;
	bb::system::SystemToast * t = new bb::system::SystemToast(this);
	t->setBody(message);
	t->show();
}
