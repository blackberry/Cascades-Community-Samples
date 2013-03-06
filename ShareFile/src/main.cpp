/*
*  This sample used the original Filepicker sample and added a share button
*
*  Developed by Steven Kader
*  The Jared Compoany
*
*  You can use this sample for anything that you want in part or in whole
*/

#include <bb/cascades/AbstractPane>
#include <bb/cascades/Application>
#include <bb/cascades/pickers/FilePicker>
#include <bb/cascades/pickers/FilePickerMode>
#include <bb/cascades/pickers/FilePickerSortFlag>
#include <bb/cascades/pickers/FilePickerSortOrder>
#include <bb/cascades/pickers/FileType>
#include <bb/cascades/pickers/ViewMode>
#include <bb/cascades/QmlDocument>

#include <QLocale>
#include <QTranslator>
#include "MainApp.hpp"

using namespace bb::cascades;

int main(int argc, char **argv)
{

    qmlRegisterType<bb::cascades::pickers::FilePicker>("bb.cascades.pickers", 1, 0, "FilePicker");
    qmlRegisterUncreatableType<bb::cascades::pickers::FilePickerMode>("bb.cascades.pickers", 1, 0, "FilePickerMode", "");
    qmlRegisterUncreatableType<bb::cascades::pickers::FilePickerSortFlag>("bb.cascades.pickers", 1, 0, "FilePickerSortFlag", "");
    qmlRegisterUncreatableType<bb::cascades::pickers::FilePickerSortOrder>("bb.cascades.pickers", 1, 0, "FilePickerSortOrder", "");
    qmlRegisterUncreatableType<bb::cascades::pickers::FileType>("bb.cascades.pickers", 1, 0, "FileType", "");


    Application app(argc, argv);

    // localization support
    QTranslator translator;
    const QString locale_string = QLocale().name();
    const QString filename = QString::fromLatin1("filepicker_%1").arg(locale_string);
    if (translator.load(filename, "app/native/qm")) {
        app.installTranslator(&translator);
    }

     new MainApp(&app) ;

     return Application::exec();
}
