#include "Canvas.h"

#include "Editor.h"

Canvas::Canvas(QWidget* parent) {
	editor = nullptr;
}

Canvas::~Canvas()
{
	delete editor;
	editor = nullptr;
}

void Canvas::initializeGL() {

	initializeOpenGLFunctions();

    editor = new Editor();

	if (!editor->initializeGlad()) {
		delete editor;
		editor = nullptr;
		return;
	}

    if (!editor->init()) {
        delete editor;
		editor = nullptr;
		return;
    }

}

void Canvas::paintGL() {

	if (!editor)
		return;

	editor->renderFrame();
	//update();
}

void Canvas::resizeGL(int w, int h) {
	glViewport(0, 0, w, h);
}
