#include "MainWindow.h"

#include <qmenubar.h>
#include <QActionGroup>
#include <qdockwidget.h>
#include <QFileDialog>
#include <QFormLayout>
#include <QToolBar>

#include "Canvas.h"
#include "Editor.h"
#include "MeshManipulator.h"

MainWindow::MainWindow() {
	
	canvas = new Canvas();
	setCentralWidget(canvas);

	// Crea un menú para abrir archivos
	QMenuBar* menuBar = this->menuBar();
	QMenu* fileMenu = menuBar->addMenu("Archivo");
	QAction* openAction = new QAction("Abrir", this);
	fileMenu->addAction(openAction);

	QObject::connect(openAction, &QAction::triggered, this, &MainWindow::openFile);


	// Seleccion vertice/segmento/cara
	setupSelectionMode();

	// Caja con valores xyz de posicion... de objeto seleccionado
	setupXYZPanel();

	// Una vez el editor este cargado, llamaremos a onEditorReady para conectar elementos de la IU a funciones del editor
	connect(canvas, &Canvas::editorReady, this, &MainWindow::onEditorReady);
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

void MainWindow::setupXYZPanel() { // !! CUIDADO, SI SE CIERRA PANEL NO HAY FORMA DE REABRIRLO

	QDockWidget* dock = new QDockWidget("Transform", this);

	QWidget* panel = new QWidget();
	QFormLayout* layout = new QFormLayout(panel);

	xSpin = new QDoubleSpinBox();
	ySpin = new QDoubleSpinBox();
	zSpin = new QDoubleSpinBox();

	xSpin->setRange(-99999.99, 99999.99);
	ySpin->setRange(-99999.99, 99999.99);
	zSpin->setRange(-99999.99, 99999.99);

	xSpin->setDecimals(2);
	ySpin->setDecimals(2);
	zSpin->setDecimals(2);

	layout->addRow("X", xSpin);
	layout->addRow("Y", ySpin);
	layout->addRow("Z", zSpin);

	dock->setWidget(panel);

	addDockWidget(Qt::RightDockWidgetArea, dock);
	
}

void MainWindow::updateXYZPanel() {

	glm::vec3 p = canvas->getEditor()->getSelectionPosition();

	xSpin->setValue(p.x);
	ySpin->setValue(p.y);
	zSpin->setValue(p.z);
}

void MainWindow::onEditorReady(Editor* editor) {

	MeshManipulator* manipulator = editor->getMeshManipulator();

	connect(xSpin, &QDoubleSpinBox::valueChanged,
		manipulator, &MeshManipulator::setSelectedXPosition);
	
	connect(ySpin, &QDoubleSpinBox::valueChanged,
		manipulator, &MeshManipulator::setSelectedYPosition);
	
	connect(zSpin, &QDoubleSpinBox::valueChanged,
		manipulator, &MeshManipulator::setSelectedZPosition);

	/*connect(manipulator,
		&MeshManipulator::setSelectedXPosition,
		this,
		&MainWindow::updateXYZPanel);*/
}