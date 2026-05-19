#include "Canvas.h"

#include "Editor.h"
#include <iostream>

Canvas::Canvas(QWidget* parent) {
	editor = nullptr;
}

Canvas::~Canvas() {
	delete editor;
	editor = nullptr;
}

void Canvas::initializeGL() { // !! revisar posibles leaks

	initializeOpenGLFunctions();

    editor = new Editor();

    if (!editor->init()) {
        delete editor;
		editor = nullptr;

		std::cout << "Error inicializando el editor" << std::endl;

		return;
    }

}

void Canvas::paintGL() {

	if (!editor)
		return;

	editor->renderFrame();
	//update();
}

void Canvas::resizeGL(int w, int h) { // !! llamar al editor para que lo haga?
	glViewport(0, 0, w, h);
}
