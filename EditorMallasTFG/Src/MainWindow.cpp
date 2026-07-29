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

	// Modo transformacion mover/rotar/escalar
	setupTransformMode();

	// Caja con valores xyz de posicion... de objeto seleccionado
	setupXYZPanel();

	setupAxes();

	// Una vez el editor este cargado, llamaremos a onEditorReady para conectar elementos de la IU a funciones del editor
	connect(canvas, &Canvas::editorReady, this, &MainWindow::onEditorReady);
}

bool MainWindow::openFile() {

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

void MainWindow::setupTransformMode() {

	QToolBar* toolbar = addToolBar("Modo transform");

	QAction* moveAction = toolbar->addAction(QIcon("Assets/icons/move.png"), "Mover");
	QAction* rotateAction = toolbar->addAction(QIcon("Assets/icons/rotate.png"), "Rotar");
	QAction* scaleAction = toolbar->addAction(QIcon("Assets/icons/scale.png"), "Escalar");

	moveAction->setCheckable(true);
	rotateAction->setCheckable(true);
	scaleAction->setCheckable(true);

	QActionGroup* selectionGroup = new QActionGroup(this);
	selectionGroup->setExclusive(true);

	selectionGroup->addAction(moveAction);
	selectionGroup->addAction(rotateAction);
	selectionGroup->addAction(scaleAction);

	moveAction->setChecked(true);

	moveAction->setShortcut(Qt::Key_G);
	rotateAction->setShortcut(Qt::Key_R);
	scaleAction->setShortcut(Qt::Key_S);

	connect(moveAction, &QAction::triggered, this, [this]() {
		canvas->getEditor()->getMeshManipulator()->setTransformMode(TransformMode::Translate);
		});

	connect(rotateAction, &QAction::triggered, this, [this]() {
		canvas->getEditor()->getMeshManipulator()->setTransformMode(TransformMode::Rotate);
		});

	connect(scaleAction, &QAction::triggered, this, [this]() {
		canvas->getEditor()->getMeshManipulator()->setTransformMode(TransformMode::Scale);
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

void MainWindow::setupAxes() {
	QToolBar* toolbar = addToolBar("Modo transform");

	QAction* X = toolbar->addAction(QIcon("Assets/icons/x.png"), "Mover");
	QAction* Y = toolbar->addAction(QIcon("Assets/icons/y.png"), "Rotar");
	QAction* Z = toolbar->addAction(QIcon("Assets/icons/z.png"), "Escalar");
	QAction* All = toolbar->addAction(QIcon("Assets/icons/all.png"), "Todos");

	X->setCheckable(true);
	Y->setCheckable(true);
	Z->setCheckable(true);
	All->setCheckable(true);

	QActionGroup* selectionGroup = new QActionGroup(this);
	selectionGroup->setExclusive(true);

	selectionGroup->addAction(X);
	selectionGroup->addAction(Y);
	selectionGroup->addAction(Z);
	selectionGroup->addAction(All);

	X->setChecked(true);

	X->setShortcut(Qt::Key_X);
	Y->setShortcut(Qt::Key_Y);
	Z->setShortcut(Qt::Key_Z);
	All->setShortcut(Qt::Key_A);

	connect(X, &QAction::triggered, this, [this]() {
		canvas->getEditor()->getMeshManipulator()->setTransformAxis(TransformAxis::X);
		});

	connect(Y, &QAction::triggered, this, [this]() {
		canvas->getEditor()->getMeshManipulator()->setTransformAxis(TransformAxis::Y);
		});

	connect(Z, &QAction::triggered, this, [this]() {
		canvas->getEditor()->getMeshManipulator()->setTransformAxis(TransformAxis::Z);
		});
	
	connect(All, &QAction::triggered, this, [this]() {
		canvas->getEditor()->getMeshManipulator()->setTransformAxis(TransformAxis::All);
		});
}

void MainWindow::updateXYZPanel(double x, double y, double z) {

	QSignalBlocker blockerX(xSpin);
	QSignalBlocker blockerY(ySpin);
	QSignalBlocker blockerZ(zSpin);
	
	xSpin->setValue(x);
	ySpin->setValue(y);
	zSpin->setValue(z);
}

void MainWindow::onEditorReady(Editor* editor) {

	MeshManipulator* manipulator = editor->getMeshManipulator();

	connect(xSpin, &QDoubleSpinBox::valueChanged,
		manipulator, &MeshManipulator::setSelectedXPosition);
	
	connect(ySpin, &QDoubleSpinBox::valueChanged,
		manipulator, &MeshManipulator::setSelectedYPosition);
	
	connect(zSpin, &QDoubleSpinBox::valueChanged,
		manipulator, &MeshManipulator::setSelectedZPosition);

	connect(manipulator,
		&MeshManipulator::selectedPositionChanged,
		this,
		&MainWindow::updateXYZPanel);
}