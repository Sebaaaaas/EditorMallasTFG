#pragma once

#include <qopenglwidget.h>
#include <qopenglfunctions.h>

class Editor;

// https://doc.qt.io/qt-6/qopenglwidget.html
class Canvas : public  QOpenGLWidget, protected QOpenGLFunctions{

public:
	Canvas(QWidget* parent = nullptr);
	~Canvas();

protected:
	void initializeGL() override;

	void paintGL() override;

	void resizeGL(int w, int h) override;

	// Deteccion de input por la ventana, que enviaremos a nuestra clase "Input"
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

private:
	Editor* editor;
};
