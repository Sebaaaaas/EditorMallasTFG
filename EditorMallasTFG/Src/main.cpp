#include <QApplication>

#include "MainWindow.h"
#include "checkML.h"

int main(int argc, char* argv[])
{
    // Deteccion de memory leaks en Debug
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    QApplication app(argc, argv);

    MainWindow window;
    window.resize(800, 600);
    window.show();

    return app.exec();
}