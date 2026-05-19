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

private:
	Editor* editor;
};

////int main()
////{
////    Editor* editor = new Editor();
////
////    if (!editor->init()) {
////        delete editor;
////        return 1;
////    }
////
////    editor->run();
////
////    delete editor;
////    
////    return 0;
////}
