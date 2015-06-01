#include "mysqldatasource.hpp"

#include <QFile>
#include <QDir>

#include <bb/cascades/Application>
#include <bb/data/SqlDataAccess>
#include <bb/system/SystemDialog>
#include <bb/data/DataAccessError>

#include <QtSql/QSqlDatabase>
#include <QtSql/QtSql>

using namespace bb::cascades;
using namespace bb::data;
using namespace bb::system;

MySqlDataSource::MySqlDataSource(QObject *parent) :
        QObject(parent)
{
    connect(this, SIGNAL(loadSQLData(const QString&)), this, SLOT(onLoadData(const QString&)),
            Qt::QueuedConnection);

    // Create the directory, if it doens't exist yet
    QDir d;
    d.mkdir(QDir::home().absoluteFilePath("data"));

    // Create the SQL database
    createDatabase();
}

MySqlDataSource::~MySqlDataSource()
{
}

void MySqlDataSource::createDatabase()
{
    // Define the path, where the database will be located
    // The path must always remain the same as it saves the database to the device
    // & the application reads from this database
    QDir home = QDir::home();
    SqlDataAccess sda(home.absoluteFilePath("data/clientDatabase.db"));

    // The following functions create 'tables' within the database
    // Within each table are 'fields' - firstName is a field
    // clientID will work as the unique ID for each item added to this table in the
    // database
    const QString createClientsTable = QString("CREATE TABLE IF NOT EXISTS Clients ( "
            "                clientID INTEGER PRIMARY KEY AUTOINCREMENT, "
            "                firstName VARCHAR, "
            "                lastName VARCHAR, "
            "                phoneNumber VARCHAR, "
            "                emailAddress VARCHAR "
            ");");

    sda.execute(createClientsTable);
    const QString createFilesTable = QString("CREATE TABLE IF NOT EXISTS Files ( "
            "                clientID INTEGER, "
            "                fileID INTEGER PRIMARY KEY AUTOINCREMENT,"
            "                fileTitle VARCHAR, "
            "                dateHeader VARCHAR, "
            "                comment VARCHAR "
            ");");

    sda.execute(createFilesTable);
    // Close the database to prevent memory leaks
    sda.connection().close();
    // Check to see if there was an error creating the database
    qDebug() << "createDatabase(): " << sda.error().errorMessage();
}

void MySqlDataSource::addClient(const QString vFirstName, const QString vLastName,
        const QString vPhoneNumber, const QString vEmailAddress)
{
    // Make reference to the directory again to add to the database
    QDir home = QDir::home();
    SqlDataAccess sda(home.absoluteFilePath("data/clientDatabase.db"));

    // To add to the database, you will 'insert' a 'record'
    // The function below takes values from QML and adds them to the database
    const QString addClientQuery =
            QString("INSERT OR REPLACE INTO Clients ") +
            " (firstName, lastName, phoneNumber, emailAddress) " +
            "VALUES ('" + vFirstName + "', '"
            + vLastName + "', '"
            + vPhoneNumber + "', '"
            + vEmailAddress + "');";

    sda.execute(addClientQuery);

    //Check to see if there was an error when inserting the record
    qDebug() << "client row inserted: " << addClientQuery << sda.error().errorMessage();
}

void MySqlDataSource::editClient(const QString vFirstName, const QString vLastName,
        const QString vClientID, const QString vPhoneNumber, const QString vEmailAddress)
{
    QDir home = QDir::home();
    SqlDataAccess sda(home.absoluteFilePath("data/clientDatabase.db"));

    //To be able to edit, in the database, you must 'Update' your 'record'
    const QString updateClientQuery = QString("UPDATE Clients ") +
            " SET firstName='" + vFirstName + "'," +
            "     lastName='" + vLastName + "'," +
            "     phoneNumber='" + vPhoneNumber + "'," +
            "     emailAddress='" + vEmailAddress + "'" +
            "    WHERE clientID=" + vClientID;

    sda.execute(updateClientQuery);

    qDebug() << "client row inserted: " << updateClientQuery << sda.error().errorMessage();
}

void MySqlDataSource::deleteClient(int clientID)
{
    QDir home = QDir::home();
    SqlDataAccess sda(home.absoluteFilePath("data/clientDatabase.db"));

    // To delete a specific 'record', the 'clientID' is used to identify the 'record'
    // & then remove it from the database
    const QString deleteRow = "DELETE FROM Clients WHERE clientID=" + QString::number(clientID)
            + ";";
    sda.execute(deleteRow);
}

int MySqlDataSource::addFile(const QString vClientID, const QString vFileTitle,
        const QString vDateHeader, const QString vComment)
{
    QVariant ret;
    QDir home = QDir::home();
    SqlDataAccess sda(home.absoluteFilePath("data/clientDatabase.db"));

    const QString addFileQuery = QString("INSERT OR REPLACE INTO Files ")
            + " (clientID, fileTitle, dateHeader, comment) "
            + "VALUES ('"
            + vClientID + "','"
            + vFileTitle + "', '"
            + vDateHeader + "', '"
            + vComment + "');";

    sda.execute(addFileQuery);

    qDebug() << "file row inserted: " << addFileQuery << sda.error().errorMessage() << ret.toList();
}

void MySqlDataSource::editFile(const QString vFileTitle, const QString vDateHeader,
        const QString vComment, const QString vFileID)
{
    QDir home = QDir::home();
    SqlDataAccess sda(home.absoluteFilePath("data/clientDatabase.db"));

    // You must specify the fileID to 'update' the correct 'record'
    const QString updateClientQuery = QString("UPDATE Files ") +
            " SET fileTitle='" + vFileTitle + "',"
            + " dateHeader='" + vDateHeader + "',"
            + " comment='" + vComment + "'"
            + " WHERE fileID=" + vFileID;

    sda.execute(updateClientQuery);

    qDebug() << "file row edited: " << updateClientQuery << sda.error().errorMessage();
}

void MySqlDataSource::deleteFile(int fileID)
{
    QDir home = QDir::home();
    SqlDataAccess sda(home.absoluteFilePath("data/clientDatabase.db"));

    const QString deleteRow = "DELETE FROM Files WHERE fileID=" + QString::number(fileID) + ";";
    sda.execute(deleteRow);
}

void MySqlDataSource::loadData()
{

    if (mQuery.isEmpty() == false) {
        emit loadSQLData(mQuery);
    }
}

void MySqlDataSource::onLoadData(const QString& sql)
{

    SqlDataAccess sda(mSource);
    QVariant list = sda.execute(sql);

    modelList = list.value<QVariantList>();
    qDebug() << "loadData() : " << mSource << list << modelList;
    emit dataLoaded(modelList);
}

void MySqlDataSource::copyFileToDataFolder(const QString fileName)
{
    // Since we need read and write access to the file, it has
    // to be moved to a folder where we have access to it. First,
    // we check if the file already exists (previously copied).
    QString dataFolder = QDir::homePath();
    QString newFileName = dataFolder + "/" + fileName;
    QFile newFile(newFileName);
    newFile.open(QIODevice::ReadWrite);

    if (!newFile.exists()) {

        // If the file is not already in the data folder, we copy it from the
        // assets folder (read only) to the data folder (read and write).
        QString appFolder(QDir::homePath());
        appFolder.chop(4);
        QString originalFileName = appFolder + fileName;
        QFile originalFile(originalFileName);
        originalFile.open(QIODevice::ReadWrite);

        if (originalFile.exists()) {

            // Create sub folders if any creates the SQL folder for a file path like e.g. sql/quotesdb
            QFileInfo fileInfo(newFileName);
            QDir().mkpath(fileInfo.dir().path());

            if (!originalFile.copy(newFileName)) {
                qDebug() << "Failed to copy file to path: " << newFileName;
            }

        } else {
            qDebug() << "Failed to copy file data base file does not exists.";
        }
        qDebug() << "Copied Successfully!";
    }
}

void MySqlDataSource::setSource(const QString source)
{
    if (mSource.compare(source) != 0) {
        // Copy the file to the data folder to get read and write access.
        copyFileToDataFolder(source);

        // Setup the path to database
        QDir home = QDir::home();
        QString filePath = home.absoluteFilePath("data/clientDatabase.db");
        mSource = filePath;

        emit sourceChanged(mSource);
    }
}

QString MySqlDataSource::source()
{
    return mSource;
}

void MySqlDataSource::setQuery(const QString query)
{
    if (mQuery.compare(query) != 0) {
        mQuery = query;

        emit queryChanged(mQuery);
    }
}

QString MySqlDataSource::query()
{
    return mQuery;
}

void MySqlDataSource::alert(const QString &message)
{
    SystemDialog *dialog; // SystemDialog uses the BB10 native dialog.
    dialog = new SystemDialog(tr("OK"), 0); // New dialog with on 'Ok' button, no 'Cancel' button
    dialog->setTitle(tr("Alert")); // set a title for the message
    dialog->setBody(message); // set the message itself
    dialog->setDismissAutomatically(true); // Hides the dialog when a button is pressed.

    // Setup slot to mark the dialog for deletion in the next event loop after the dialog has been accepted.
    bool ok = connect(dialog, SIGNAL(finished(bb::system::SystemUiResult::Type)), dialog,
            SLOT(deleteLater()));
    Q_ASSERT(ok);
    Q_UNUSED(ok);
    dialog->show();
}
