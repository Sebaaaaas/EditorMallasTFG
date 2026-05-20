#include "MainWindow.h"

#include <QFileDialog>
#include <qmenubar.h>

#include "Canvas.h"

MainWindow::MainWindow() {
	
	canvas = new Canvas();
	setCentralWidget(canvas);

	// Crea un menú para abrir archivos
	QMenuBar* menuBar = this->menuBar();
	QMenu* fileMenu = menuBar->addMenu("Archivo");
	QAction* openAction = new QAction("Abrir", this);
	fileMenu->addAction(openAction);

	QObject::connect(openAction, &QAction::triggered, this, &MainWindow::openFile);
	
}

bool MainWindow::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(
		this,				// padre
		"Abrir archivo",	// título del cuadro
		"",					// directorio inicial
		"Archivos OBJ (*.obj);;Todos los archivos (*)" // filtros
	);

	if (fileName.isEmpty())
		return false;

	return canvas->loadMesh(fileName);
}
