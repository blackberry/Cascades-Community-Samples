#include "MainMenu.hpp"

#include <bb/cascades/Application>

#include <QLocale>
#include <QTranslator>

using ::bb::cascades::Application;

int main(int argc, char **argv)
{
    Application app(argc, argv);
    
    QTranslator translator;
    QString locale_string = QLocale().name();
    QString filename = QString( "NfcToolLite_%1" ).arg( locale_string );

    if (translator.load(filename, "app/native/qm")) {
        app.installTranslator( &translator );
    }
    
    MainMenu mainApp(&app);

    QObject::connect(&app, SIGNAL(aboutToQuit()), &mainApp, SLOT(cleanUpOnExit()));
    
    return Application::exec();
}
