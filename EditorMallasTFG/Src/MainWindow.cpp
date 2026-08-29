#include "MainWindow.h"

#include <QActionGroup>
#include <QDockWidget>
#include <QFileDialog>
#include <QFormLayout>
#include <QMessageBox>
#include <QMenuBar>
#include <QToolBar>

#include "Canvas.h"
#include "Editor.h"
#include "qmenubar.h"

MainWindow::MainWindow() {

	canvas = new Canvas();
	setCentralWidget(canvas);

	// Crea un menu para abrir archivos
	QMenuBar* menuBar = this->menuBar();
	QMenu* fileMenu = menuBar->addMenu("&Archivo");
	QAction* openAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), "&Abrir", this);
	openAction->setShortcuts(QKeySequence::Open);
	fileMenu->addAction(openAction);

	QObject::connect(openAction, &QAction::triggered, this, &MainWindow::openFile);

	// Crea un menu para guardar archivos
	QAction* saveAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave), "&Guardar", this);
	saveAction->setShortcuts(QKeySequence::Save);
	fileMenu->addAction(saveAction);

	QObject::connect(saveAction, &QAction::triggered, this, &MainWindow::saveFile);  // TODO: con el mismo nombre, lo que había antes

	QAction* saveAsAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSaveAs), "Guardar &como...", this);
	saveAsAction->setShortcuts(QKeySequence::SaveAs);
	fileMenu->addAction(saveAsAction);

	QObject::connect(saveAsAction, &QAction::triggered, this, &MainWindow::saveFile);

	QAction* quitAction = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::WindowClose), "&Salir", this);
	quitAction->setShortcuts(QKeySequence::Quit);
	fileMenu->addAction(quitAction);

	QObject::connect(quitAction, &QAction::triggered, this, &MainWindow::close);

	// Menú Ver
	QMenu* viewMenu = menuBar->addMenu("&Ver");

	mainToolBar = addToolBar("Main Toolbar");

	// Seleccion vertice/segmento/cara
	setupSelectionMode();

	// Modo transformacion mover/rotar/escalar
	setupTransformMode();

	// Caja con valores xyz de posicion... de objeto seleccionado
	setupXYZPanel();

	setupAxes();

	setupRenderMode(viewMenu);

	setupProjectionMode(viewMenu);

	createHelpBox(menuBar);

	// Una vez el editor este cargado, llamaremos a onEditorReady para conectar elementos de la IU a funciones del editor
	connect(canvas, &Canvas::editorReady, this, &MainWindow::onEditorReady);
}

bool MainWindow::openFile() {

	QString fileName = QFileDialog::getOpenFileName(
		this,				// padre
		"Abrir archivo",	// titulo del cuadro
		"./Assets/",					// directorio inicial
		"Archivos OBJ (*.obj);;Todos los archivos (*)" // filtros
	);

	if (fileName.isEmpty())
		return false;

	return canvas->loadMesh(fileName);
}

bool MainWindow::saveFile() {

	QString path = QFileDialog::getSaveFileName(
		nullptr,
		"Guardar malla",
		"Assets/ourNewLilSave.obj",
		"Wavefront OBJ (*.obj)"
	);

	if (path.isEmpty())
		return false;

	return canvas->saveMesh(path);
}

void MainWindow::setupSelectionMode() {

	QAction* vertexAction = mainToolBar->addAction(QIcon("Assets/icons/vertex.png"), "Vertice (1)");
	QAction* edgeAction = mainToolBar->addAction(QIcon("Assets/icons/edge.png"), "Segmento (2)");
	QAction* faceAction = mainToolBar->addAction(QIcon("Assets/icons/face.png"), "Cara (3)");

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

	QAction* moveAction = mainToolBar->addAction(QIcon("Assets/icons/move.png"), "Mover (G)");
	QAction* rotateAction = mainToolBar->addAction(QIcon("Assets/icons/rotate.png"), "Rotar (R)");
	QAction* scaleAction = mainToolBar->addAction(QIcon("Assets/icons/scale.png"), "Escalar (S)");

	mainToolBar->insertSeparator(moveAction);

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
		canvas->getEditor()->setTransformMode(TransformMode::Translate);
		});

	connect(rotateAction, &QAction::triggered, this, [this]() {
		canvas->getEditor()->setTransformMode(TransformMode::Rotate);
		});

	connect(scaleAction, &QAction::triggered, this, [this]() {
		canvas->getEditor()->setTransformMode(TransformMode::Scale);
		});
}

void MainWindow::setupXYZPanel() {

	QDockWidget* dock = new QDockWidget("Transform", this);

	// Impide que se cierre el panel haciendo click en la x
	dock->setFeatures(dock->features() & ~QDockWidget::DockWidgetClosable);

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

	QAction* All = mainToolBar->addAction(QIcon("Assets/icons/all.png"), "Todos (A)");
	QAction* X = mainToolBar->addAction(QIcon("Assets/icons/x.png"), "Eje X (X)");
	QAction* Y = mainToolBar->addAction(QIcon("Assets/icons/y.png"), "Eje Y (Y)");
	QAction* Z = mainToolBar->addAction(QIcon("Assets/icons/z.png"), "Eje Z (Z)");

	mainToolBar->insertSeparator(All);

	All->setCheckable(true);
	X->setCheckable(true);
	Y->setCheckable(true);
	Z->setCheckable(true);

	QActionGroup* selectionGroup = new QActionGroup(this);
	selectionGroup->setExclusive(true);

	selectionGroup->addAction(All);
	selectionGroup->addAction(X);
	selectionGroup->addAction(Y);
	selectionGroup->addAction(Z);

	All->setChecked(true);

	All->setShortcut(Qt::Key_A);
	X->setShortcut(Qt::Key_X);
	Y->setShortcut(Qt::Key_Y);
	Z->setShortcut(Qt::Key_Z);

	connect(All, &QAction::triggered, this, [this]() {
		canvas->getEditor()->setTransformAxis(TransformAxis::All);
		});

	connect(X, &QAction::triggered, this, [this]() {
		canvas->getEditor()->setTransformAxis(TransformAxis::X);
		});

	connect(Y, &QAction::triggered, this, [this]() {
		canvas->getEditor()->setTransformAxis(TransformAxis::Y);
		});

	connect(Z, &QAction::triggered, this, [this]() {
		canvas->getEditor()->setTransformAxis(TransformAxis::Z);
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

void MainWindow::setupRenderMode(QMenu* viewMenu) {

	QAction* solid = mainToolBar->addAction(QIcon("Assets/icons/solid.png"), "Sólido");

	QAction* wire = mainToolBar->addAction(QIcon("Assets/icons/wireframe.png"), "Wireframe");

	mainToolBar->insertSeparator(solid);

	solid->setCheckable(true);
	wire->setCheckable(true);

	QActionGroup* group = new QActionGroup(this);
	group->setExclusive(true);

	group->addAction(solid);
	group->addAction(wire);

	solid->setChecked(true);

	solid->setShortcut(Qt::Key_Q);
	wire->setShortcut(Qt::Key_W);

	connect(solid, &QAction::triggered, this, [this]() {
			canvas->getEditor()->setRenderMode(RenderMode::Solid);
		});

	connect(wire, &QAction::triggered, this, [this]() {
			canvas->getEditor()->setRenderMode(RenderMode::Wireframe);
		});

	// Añade las acciones también al menú
	viewMenu->addAction(solid);
	viewMenu->addAction(wire);
}

void MainWindow::setupProjectionMode(QMenu* viewMenu) {

	QAction* perspective = mainToolBar->addAction(QIcon("Assets/icons/perspective.png"), "Perspectiva");

	QAction* orthogonal = mainToolBar->addAction(QIcon("Assets/icons/orthogonal.png"), "Ortogonal");

	mainToolBar->insertSeparator(perspective);

	perspective->setCheckable(true);
	orthogonal->setCheckable(true);

	QActionGroup* group = new QActionGroup(this);
	group->setExclusive(true);

	group->addAction(perspective);
	group->addAction(orthogonal);

	perspective->setChecked(true);

	perspective->setShortcut(Qt::Key_P);
	orthogonal->setShortcut(Qt::Key_O);

	connect(perspective, &QAction::triggered, this, [this]() {
		canvas->getEditor()->setProjectionMode(ProjectionMode::Perspective);
		});

	connect(orthogonal, &QAction::triggered, this, [this]() {
		canvas->getEditor()->setProjectionMode(ProjectionMode::Orthographic);
		});

	// Añade las acciones también al menú
	viewMenu->addSeparator();
	viewMenu->addAction(perspective);
	viewMenu->addAction(orthogonal);
}

void MainWindow::onEditorReady(Editor* editor) {

	connect(xSpin, &QDoubleSpinBox::editingFinished,
		this, [this]() {
			canvas->getEditor()->setSelectedXPosition(xSpin->value());
		});

	connect(ySpin, &QDoubleSpinBox::editingFinished,
		this, [this]() {
			canvas->getEditor()->setSelectedYPosition(ySpin->value());
		});

	connect(zSpin, &QDoubleSpinBox::editingFinished,
		this, [this]() {
			canvas->getEditor()->setSelectedZPosition(zSpin->value());
		});

	connect(editor,
		&Editor::selectedPositionChanged,
		this,
		&MainWindow::updateXYZPanel);
}

void MainWindow::createHelpBox(QMenuBar* menuBar) {

	QMenu* helpMenu = menuBar->addMenu("&Ayuda");

	QAction* cameraHelpAction = new QAction("&Controles del editor", this);
	cameraHelpAction->setShortcuts(QKeySequence::HelpContents);

	helpMenu->addAction(cameraHelpAction);

	connect(cameraHelpAction, &QAction::triggered, this, [this]() { 

		QMessageBox helpBox(this); 

		helpBox.setWindowTitle("Ayuda"); 
		helpBox.setIcon(QMessageBox::Information); 
		helpBox.setText("<b>Controles del editor</b>"); 

		helpBox.setInformativeText(
			"<table>"
			"<tr><th colspan='2' align='left'>Selecci\u00F3n</th></tr>"
			"<tr><td>Clic izquierdo</td><td>&nbsp;&nbsp;Seleccionar / deseleccionar</td></tr>"
			"<tr><td>Shift + clic izquierdo</td><td>&nbsp;&nbsp;A\u00F1adir a la selecci\u00F3n</td></tr>"
			"<tr><td><br></td></tr>" "<tr><th colspan='2' align='left'>C\u00E1mara</th></tr>"
			"<tr><td>F</td><td>&nbsp;&nbsp;Recentrar c\u00E1mara</td></tr>"
			"<tr><td>Clic central + arrastrar</td><td>&nbsp;&nbsp;Orbitar</td></tr>"
			"<tr><td>Shift + clic central + arrastrar</td><td>&nbsp;&nbsp;Panear</td></tr>"
			"<tr><td>Rueda del rat\u00F3n</td><td>&nbsp;&nbsp;Zoom</td></tr>" "</table>");

		helpBox.adjustSize();
		helpBox.exec(); 
		});

}
