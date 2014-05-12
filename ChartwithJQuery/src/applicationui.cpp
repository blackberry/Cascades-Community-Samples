/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
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

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/data/XmlDataAccess>
#include <bb/data/SqlDataAccess>
#include <bb/data/DataAccessError>
#include <QtSql/QtSql>
#include <bb/system/SystemDialog>
#include <bb/system/SystemToast>

using namespace bb::cascades;
using namespace bb::data;
using namespace bb::system;

	QString Database_PATH = "";
	QString Sparky = "";
	QString Sparky_Full = "";

ApplicationUI::ApplicationUI(bb::cascades::Application *app) :
        QObject(app)
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();
    createDatabase();
    createTable();
    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("_app",this);
    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    app->setScene(root);
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("Chart_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

bool ApplicationUI::createDatabase()
{

    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    bool success = false;

    database.setDatabaseName("./data/chart.sqlite");
    Database_PATH = database.databaseName();

    if (database.open()) {
        alert(tr("Database created/registered. ")+ Database_PATH);

        success = true;
    } else {

        const QSqlError error = database.lastError();
        alert(tr("Error opening connection to the database: %1").arg(error.text()));
    }
    return success;
}


void ApplicationUI::createTable()
{
    QSqlDatabase database = QSqlDatabase::database();
    const QString createSQL = "CREATE TABLE IF NOT EXISTS water ( "
                                  "                rowID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                  "                data VARCHAR, "
                                  "                quantity VARCHAR"
                                  ");";
    QSqlQuery query(database);

	   if (query.exec(createSQL)) {
		   alert(tr("Table water creation query execute successfully"));
	   } else {
		   const QSqlError error = query.lastError();
		   alert(tr("Create water table error: %1").arg(error.text()));
	   }
}

bool ApplicationUI::createRecord(const QString &data, const QString &quantity)
{
	QSqlDatabase database = QSqlDatabase::database();
    SqlDataAccess *sqlda = new SqlDataAccess(database.databaseName());

    QVariantList drink;
    drink << data << quantity;
    sqlda->execute("INSERT INTO water"
                  "    (data, quantity) "
                  "    VALUES (:data, :quantity)", drink);
    bool success = false;
    if(!sqlda->hasError()) {
        success = true;
    } else {
        const DataAccessError error = sqlda->error();
        alert(tr("Create record error: %1").arg(error.errorMessage()));
    }
    return success;
}

bool ApplicationUI::deleteRecord(const QString &data)
{

	QSqlDatabase database = QSqlDatabase::database();
    SqlDataAccess *sqlda = new SqlDataAccess(database.databaseName());
    const QString sqlVerify = "SELECT * FROM water WHERE data = :data";
    QVariantList argsList;
    argsList << data;
    QVariant verificationResult = sqlda->execute(sqlVerify, argsList);
    if(verificationResult.isNull() || verificationResult.value<QVariantList>().size() == 0) {
        alert(tr("item with id=%1 was not found.").arg(data));
        return false;
    }
    QVariantMap bindings;
    bindings["data"] = data;
    sqlda->execute("DELETE FROM water WHERE data=:data", bindings);
    bool deleted = false;
    if (!sqlda->hasError()) {
        verificationResult = sqlda->execute(sqlVerify, argsList);
        if (!verificationResult.isNull() && verificationResult.value<QVariantList>().size() == 0) {
        	alert(tr("Item removed!"));
            deleted = true;
        } else {
            alert(tr("Item with id=%1 was not found.").arg(data));
        }
    } else {
        alert(tr("SQL error: %1").arg(sqlda->error().errorMessage()));
    }

    return deleted;
}

void ApplicationUI::readWater(QString search)
{
	QSqlDatabase database = QSqlDatabase::database();
    SqlDataAccess *sqlda = new SqlDataAccess(database.databaseName());
    QString appo = search;
    QVariant result = sqlda->execute(appo);
    Sparky_Full = "";
    Sparky = "[";
    if (!sqlda->hasError()) {
        int recordsRead = 0;
        if( !result.isNull() ) {
            QVariantList list = result.value<QVariantList>();
            recordsRead = list.size();
            for(int i = 0; i < recordsRead; i++) {
                QVariantMap mapWater = list.at(i).value<QVariantMap>();
                if (i == 0){
                	Sparky = Sparky +  mapWater.value("quantity").toString();
                }
                else {
                	Sparky = Sparky + "," + mapWater.value("quantity").toString();
                }
                emit gotWater(mapWater);
            }
            Sparky = Sparky + "]";
            Sparky_Full = QString("$('.sparkline').sparkline(")+ Sparky + QString(",{type: 'line', chartRangeMin: '0', fillColor: false, normalRangeColor: 'red', width: '720', height: '250'});");
            emit gotSparky(Sparky_Full);
        }
    } else {
        alert(tr("Read records failed: %1").arg(sqlda->error().errorMessage()));
    }
}

// Alert Dialog Box Functions
void ApplicationUI::alert(const QString &message)
{
    SystemDialog *dialog; // SystemDialog uses the BB10 native dialog.
    dialog = new SystemDialog(tr("OK"), 0); // New dialog with on 'Ok' button, no 'Cancel' button
    dialog->setTitle(tr("Chart Sample Alert")); // set a title for the message
    dialog->setBody(message); // set the message itself
    dialog->setDismissAutomatically(true); // Hides the dialog when a button is pressed.

    // Setup slot to mark the dialog for deletion in the next event loop after the dialog has been accepted.
    bool ok = connect(dialog, SIGNAL(finished(bb::system::SystemUiResult::Type)), dialog, SLOT(deleteLater()));
    Q_ASSERT(ok);
    dialog->show();
}

void ApplicationUI::showToast(QString testoToast)
{
    SystemToast *toast = new SystemToast(this);
    toast->setBody(testoToast);
    toast->setPosition(SystemUiPosition::MiddleCenter);
    toast->show();
}
