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

	// Menu de seleccion para cambiar entre seleccion de vertices, segmentos y caras
	void setupSelectionMode();

};