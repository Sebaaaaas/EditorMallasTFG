#include "MainWindow.h"

#include <QFileDialog>
#include <qmenubar.h>
#include <QToolBar>
#include <QActionGroup>

#include "Canvas.h"
#include "Editor.h"

MainWindow::MainWindow() {
	
	canvas = new Canvas();
	setCentralWidget(canvas);

	// Crea un menú para abrir archivos
	QMenuBar* menuBar = this->menuBar();
	QMenu* fileMenu = menuBar->addMenu("Archivo");
	QAction* openAction = new QAction("Abrir", this);
	fileMenu->addAction(openAction);

	QObject::connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

	setupSelectionMode();
}

bool MainWindow::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(
		this,				// padre
		"Abrir archivo",	// título del cuadro
		"./Assets/",					// directorio inicial
		"Archivos OBJ (*.obj);;Todos los archivos (*)" // filtros
	);

	if (fileName.isEmpty())
		return false;

	return canvas->loadMesh(fileName);
}

void MainWindow::setupSelectionMode() {

	QToolBar* toolbar = addToolBar("Modo seleccion");

	QAction* vertexAction = toolbar->addAction(QIcon("Assets/icons/vertex.png"), "Vertice");
	QAction* edgeAction = toolbar->addAction(QIcon("Assets/icons/edge.png"), "Segmento");
	QAction* faceAction = toolbar->addAction(QIcon("Assets/icons/face.png"), "Cara");

	vertexAction->setCheckable(true);
	edgeAction->setCheckable(true);
	faceAction->setCheckable(true);

	QActionGroup* selectionGroup = new QActionGroup(this);
	selectionGroup->setExclusive(true);

	selectionGroup->addAction(vertexAction);
	selectionGroup->addAction(edgeAction);
	selectionGroup->addAction(faceAction);

	vertexAction->setChecked(true);

	vertexAction->setShortcut(Qt::Key_1);
	edgeAction->setShortcut(Qt::Key_2);
	faceAction->setShortcut(Qt::Key_3);

	connect(vertexAction, &QAction::triggered, this, [this]() {
		canvas->getEditor()->setSelectionMode(SelectionMode::Vertex);
		});

	connect(edgeAction, &QAction::triggered, this, [this]() {
		canvas->getEditor()->setSelectionMode(SelectionMode::Edge);
		});

	connect(faceAction, &QAction::triggered, this, [this]() {
		canvas->getEditor()->setSelectionMode(SelectionMode::Face);
		});
}
