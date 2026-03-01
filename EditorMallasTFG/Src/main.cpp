#include "Editor.h"

#include <iostream>
#include "checkML.h"


int main()
{
    // Deteccion de memory leaks en Debug
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    Editor* editor = new Editor();

    if (editor->init())
        std::cout << "success" << std::endl;
    else
        std::cout << "failure" << std::endl;

    editor->run();

    editor->release();

    delete editor;
    
    return 0;
}