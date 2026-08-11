#pragma once

#include <QObject>
#include <qopenglwidget.h>
#include <qopenglfunctions.h>


class Editor;

// https://doc.qt.io/qt-6/qopenglwidget.html
class Canvas : public QOpenGLWidget, protected QOpenGLFunctions{

	Q_OBJECT

public:
	Canvas(QWidget* parent = nullptr);
	~Canvas();

	bool loadMesh(const QString& fileName);
	bool saveMesh(const QString& path);

	Editor* getEditor() const;

signals:
	// Senial emitida tras inicializacion correcta de Editor, para poder conectar los UI widgets a funciones del editor
	void editorReady(Editor* editor);

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
