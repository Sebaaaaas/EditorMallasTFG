#pragma once

#include <QMainWindow>

class Canvas;

class MainWindow : public QMainWindow 
{
public:
	MainWindow();

private:

	Canvas* canvas;

	bool openFile();

};