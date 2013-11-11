/**
   Copyright (c) 2013 BlackBerry Limited.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**/
#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <ids.h>

#include <QObject>

#include "BlackBerryProfile.hpp"
#include "BlackBerryID.hpp"

namespace bb {
namespace cascades {
class Application;
class LocaleHandler;
class DataModel;
class Sheet;
class GroupDataModel;
}
}

class QTranslator;

/**
 * Application class is exposing the properties retrieved via the Identity Service.
 */
class ApplicationUI: public QObject {
Q_OBJECT
public:
	ApplicationUI(bb::cascades::Application *app);
	virtual ~ApplicationUI() {
	}

	Q_PROPERTY (QString userName READ getUserName WRITE setUserName NOTIFY userNameChanged)
	;Q_INVOKABLE
	QString getUserName() {
		return _userName;
	}
	void setUserName(const QString & v) {
		if (v != _userName) {
			_userName = v;
			emit userNameChanged(v);
		}
	}

	Q_PROPERTY (QString screenName READ getScreenName WRITE setScreenName NOTIFY screenNameChanged)
	;Q_INVOKABLE
	QString getScreenName() {
		return _screenName;
	}
	void setScreenName(const QString & v) {
		if (v != _screenName) {
			_screenName = v;
			emit screenNameChanged(v);
		}
	}

	Q_PROPERTY (QString firstName READ getFirstName WRITE setFirstName NOTIFY firstNameChanged)
	;Q_INVOKABLE
	QString getFirstName() {
		return _firstName;
	}
	void setFirstName(const QString & v) {
		if (v != _firstName) {
			_firstName = v;
			emit firstNameChanged(v);
		}
	}

	Q_PROPERTY (QString lastName READ getLastName WRITE setLastName NOTIFY lastNameChanged)
	;Q_INVOKABLE
	QString getLastName() {
		return _lastName;
	}
	void setLastName(const QString & v) {
		if (v != _lastName) {
			_lastName = v;
			emit lastNameChanged(v);
		}
	}

	Q_INVOKABLE
	void getBlackBerryID();

	Q_PROPERTY (bb::cascades::GroupDataModel* propertyData READ getPropertyData NOTIFY propertyDataChanged)
	;Q_INVOKABLE
	bb::cascades::GroupDataModel * getPropertyData() {
		return m_propertyData;
	}

	Q_INVOKABLE
	void addProperty();

	Q_INVOKABLE
	void editProperty(QString scope, QString name);

	Q_INVOKABLE
	void deleteProperty(QString scope, QString name);

	Q_PROPERTY (QString propertyScope READ propertyScope WRITE setPropertyScope NOTIFY propertyScopeChanged)
	;Q_INVOKABLE
	QString propertyScope() {
		return m_propertyScope;
	}
	Q_INVOKABLE
	void setPropertyScope(QString scope) {
		if (scope != m_propertyScope) {
			m_propertyScope = scope;
			emit propertyScopeChanged(scope);
		}
	}

	Q_PROPERTY (QString propertyName READ propertyName WRITE setPropertyName NOTIFY propertyNameChanged)
	;Q_INVOKABLE
	QString propertyName() {
		return m_propertyName;
	}
	Q_INVOKABLE
	void setPropertyName(QString name) {
		if (name != m_propertyName) {
			m_propertyName = name;
			emit propertyNameChanged(name);
		}
	}

	Q_PROPERTY (bool propertyIsEdit READ propertyIsEdit WRITE setPropertyIsEdit NOTIFY propertyIsEditChanged)
	;
	bool propertyIsEdit() {
		return m_propertyIsEdit;
	}
	Q_INVOKABLE
	void setPropertyIsEdit(bool isEdit) {
		if (isEdit != m_propertyIsEdit) {
			m_propertyIsEdit = isEdit;
			emit propertyIsEditChanged(isEdit);
		}
	}

	Q_PROPERTY (QString propertyValue READ propertyValue WRITE setPropertyValue NOTIFY propertyValueChanged)
	;Q_INVOKABLE
	QString propertyValue() {
		return m_propertyValue;
	}
	Q_INVOKABLE
	void setPropertyValue(QString value) {
		if (value != m_propertyValue) {
			m_propertyValue = value;
			emit propertyValueChanged(value);
		}
	}

	Q_INVOKABLE
	bool saveProperty(QString scope, QString name, QString value);

	Q_INVOKABLE
	void refreshProperties();

signals:
	void screenNameChanged(const QString &);
	void firstNameChanged(const QString &);
	void lastNameChanged(const QString &);
	void userNameChanged(const QString &);

	void propertyDataChanged(bb::cascades::GroupDataModel * data);
	void propertyCountChanged(int);

	void propertyScopeChanged(const QString &);
	void propertyNameChanged(const QString &);
	void propertyValueChanged(const QString &);
	void propertyIsEditChanged(const bool);

private:
	QTranslator* m_pTranslator;
	bb::cascades::LocaleHandler* m_pLocaleHandler;

	bb::cascades::GroupDataModel * m_propertyData;
	bb::cascades::Sheet * m_propertyEdit;

	QString _screenName;
	QString _firstName;
	QString _lastName;
	QString _userName;

	bool m_propertyIsEdit;
	QString m_propertyScope;
	QString m_propertyName;
	QString m_propertyValue;
	QVariantList m_whichEdit;
	QList<QVariantMap> m_properties;

	void toast(QString s);

	BlackBerryProfile * m_blackBerryProfile;
	BlackBerryID * m_blackBerryID;

private slots:
	void onAllData(QList<QVariantMap> entries);
	void onIDProperties(QList<QVariantMap> properties);
	void onError(QString message, int errno);
};

#endif /* ApplicationUI_HPP_ */
