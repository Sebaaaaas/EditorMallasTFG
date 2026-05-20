#include "Canvas.h"

#include "Editor.h"
#include "Input.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include <iostream>

Canvas::Canvas(QWidget* parent) {

	editor = nullptr;

	setFocusPolicy(Qt::StrongFocus); // Permite recibir input de teclado cuando hacemos click o tab a la ventana
	setMouseTracking(true);
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

	/*Input::beginFrame();
	Input::update();*/

	editor->renderFrame();
	update();
}

void Canvas::resizeGL(int w, int h) { // !! llamar a Editor para que lo haga?
	glViewport(0, 0, w, h);

    if (editor)
        editor->setWindowSize(w, h);
}

void Canvas::keyPressEvent(QKeyEvent* event)
{
    Input::setKey(event->key(), true);
}

void Canvas::keyReleaseEvent(QKeyEvent* event)
{
    Input::setKey(event->key(), false);
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    int button = -1;

    if (event->button() == Qt::LeftButton)  button = 0;
    if (event->button() == Qt::RightButton) button = 1;
    if (event->button() == Qt::MiddleButton) button = 2;

    if (button != -1)
        Input::setMouseButton(button, true);
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    int button = -1;

    if (event->button() == Qt::LeftButton)  button = 0;
    if (event->button() == Qt::RightButton) button = 1;
    if (event->button() == Qt::MiddleButton) button = 2;

    if (button != -1)
        Input::setMouseButton(button, false);
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    Input::setMousePosition(event->position().x(),
        event->position().y());
}

void Canvas::wheelEvent(QWheelEvent* event) // !! revisar
{
    Input::addScrollDelta(event->angleDelta().y() / 120.0);
}
