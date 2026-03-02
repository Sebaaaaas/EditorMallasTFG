#include "Editor.h"

#include <iostream>
#include "checkML.h"


int main()
{
    // Deteccion de memory leaks en Debug
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    Editor* editor = new Editor();

    if (!editor->init()) {
        delete editor;
        return 1;
    }

    editor->run();

    editor->release();

    delete editor;
    
    return 0;
}