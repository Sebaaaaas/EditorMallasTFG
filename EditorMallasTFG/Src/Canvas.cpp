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

bool Canvas::loadMesh(const QString& fileName)
{
    if (!editor)
        return false;

    makeCurrent();   // Necesitamos esto para que la malla se renderice

    bool loaded = editor->loadMesh(fileName.toStdString());

    doneCurrent();   // Soltamos el contexto cogido con makeCurrent

    return loaded;
}

Editor* Canvas::getEditor() const {
    return editor;
}

void Canvas::initializeGL() {

	initializeOpenGLFunctions();

    editor = new Editor();

    if (editor->init()) {
        emit editorReady(editor);
    }
    else {
        delete editor;
        editor = nullptr;

        std::cout << "Error inicializando el editor" << std::endl;

        return;
    }

}

void Canvas::paintGL() {

	if (!editor)
		return;

	editor->run();

    // Funcion de Qt
	update();
}

void Canvas::resizeGL(int w, int h) {
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

void Canvas::mousePressEvent(QMouseEvent* event) {

    int button = -1;

    if (event->button() == Qt::LeftButton)  button = 0;
    if (event->button() == Qt::RightButton) button = 1;
    if (event->button() == Qt::MiddleButton) button = 2;

    if (button != -1)
        Input::setMouseButton(button, true);
}

void Canvas::mouseReleaseEvent(QMouseEvent* event) {

    int button = -1;

    if (event->button() == Qt::LeftButton)  button = 0;
    if (event->button() == Qt::RightButton) button = 1;
    if (event->button() == Qt::MiddleButton) button = 2;

    if (button != -1)
        Input::setMouseButton(button, false);
}

void Canvas::mouseMoveEvent(QMouseEvent* event) {
    Input::setMousePosition(event->position().x(), event->position().y());
}

void Canvas::wheelEvent(QWheelEvent* event)
{
    Input::addScrollDelta(event->angleDelta().y() / 120.0); // Dividido entre 120 porque el evento devuelve ese valor cuando haces scroll
}
