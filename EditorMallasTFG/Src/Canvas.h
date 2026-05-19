#pragma once

#include <qopenglwidget.h>
#include <qopenglfunctions.h>

class Editor;

class Canvas : public  QOpenGLWidget, protected QOpenGLFunctions{

public:
	Canvas(QWidget* parent = nullptr);
	~Canvas();

protected:
	void initializeGL() override;

	void paintGL() override;

	void resizeGL(int w, int h) override;

private:
	Editor* editor;
};