#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include <QLocale>
#include <QTranslator>
#include <Qt/qdeclarativedebug.h>

#include "NfcSharing.hpp"

using namespace bb::cascades;

Q_DECL_EXPORT int main(int argc, char **argv)
{
    Application app(argc, argv);

    QTranslator translator;
    QString locale_string = QLocale().name();
    QString filename = QString( "NfcSharing_%1" ).arg( locale_string );
    if (translator.load(filename, "app/native/qm")) {
        app.installTranslator( &translator );
    }

    NfcSharing *nfcSharing = new NfcSharing(&app);

    QObject::connect(&app, SIGNAL(aboutToQuit()), nfcSharing, SLOT(cleanUpOnExit()));

    return Application::exec();
}
