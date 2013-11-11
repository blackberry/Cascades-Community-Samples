#include <bb/cascades/Application>

#include <QLocale>
#include <QTranslator>
#include "HeartMonitor.hpp"

#include <Qt/qdeclarativedebug.h>

using namespace bb::cascades;

Q_DECL_EXPORT int main(int argc, char **argv)
{
    Application app(argc, argv);

    new HeartMonitor(&app);

    // Enter the application main event loop.
    return Application::exec();
}
