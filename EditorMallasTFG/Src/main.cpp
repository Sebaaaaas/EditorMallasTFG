#include <QApplication>

#include "MainWindow.h"

#ifdef _WIN32
#include "checkML.h"
#endif

int main(int argc, char* argv[])
{
    // Deteccion de memory leaks en Debug
#ifdef _WIN32
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    QApplication app(argc, argv);

    // Mejora visibilidad de los tooltips de Qt
    qApp->setStyleSheet(
        "QToolTip {"
        "   color: #ffffff;"
        "   background-color: #676767;"
        "   border: 3px solid #333333;"
        "   padding: 4px;"
        "   font-size: 12pt;"
        "   border-radius: 3px;"
        "}"
    );

    MainWindow window;
    window.resize(800, 600);
    window.show();

    return app.exec();
}
