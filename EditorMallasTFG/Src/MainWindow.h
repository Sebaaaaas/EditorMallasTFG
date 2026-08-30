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

	QToolBar* mainToolBar;

	// Ruta del ultimo archivo abierto o guardado. Vacio si es una malla nueva sin guardar
	QString currentFilePath;

	bool openFile();
	bool saveFile();
	bool saveFileAs();

	// Menu de seleccion para cambiar entre seleccion de vertices, segmentos y caras
	void setupSelectionMode();

	// Menu para elegir entre mover, rotar o escalar
	void setupTransformMode();

	void setupXYZPanel();

	void setupAxes();

	void updateXYZPanel(double x, double y, double z);

	void setupRenderMode(QMenu* viewMenu);

	void setupProjectionMode(QMenu* viewMenu);

	void onEditorReady(Editor* editor);

	void createHelpBox(QMenuBar* menuBar);

};
