#pragma once

#include <QMainWindow>
#include <QDoubleSpinBox>

class Editor;
class Canvas;

class MainWindow : public QMainWindow 
{
public:
	MainWindow();

private:

	Canvas* canvas;

	// Cajas con numeros con flechas para cambiar los valores
	QDoubleSpinBox* xSpin;
	QDoubleSpinBox* ySpin;
	QDoubleSpinBox* zSpin;

	bool openFile();

	// Menu de seleccion para cambiar entre seleccion de vertices, segmentos y caras
	void setupSelectionMode();

	void setupXYZPanel();

	void updateXYZPanel();

	void onEditorReady(Editor* editor);
};