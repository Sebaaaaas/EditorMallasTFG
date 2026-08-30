#pragma once

#include <QObject>
#include <glm/glm.hpp>
#include <string>

#include "EditorTypes.h"
#include "Selector.h"

class SelectionRenderer;
class MeshManipulator;
class Camera;
class Shader;
class Mesh;

class Editor : public QObject {

	Q_OBJECT // usar "public:" despues de esto para declarar las variables publicas, esto es necesario para detectar que es un QObject

public:

	Editor();
	~Editor();

	bool init();
	void run();

	void setWindowSize(int w, int h);

	// Intenta cargar la malla en formato .obj en el directorio path(dentro de Bin/Assets). Si falla emite mensaje de error
	bool loadMesh(const std::string& path);

	bool saveMesh(const std::string& path);

	void setSelectionMode(SelectionMode mode);
	void setProjectionMode(ProjectionMode mode);
	void setTransformMode(TransformMode mode);
	void setTransformAxis(TransformAxis mode);

	// Paso intermedio para comunicar MainWindow con MeshManipulator
	void setSelectedXPosition(double value);
	void setSelectedYPosition(double value);
	void setSelectedZPosition(double value);

	void setRenderMode(RenderMode mode);

signals:
	// Cuando cambia la posicion, actualizamos el los spinbox
	void selectedPositionChanged(double x, double y, double z);

public slots: // Permite recibir seniales de QWidgets cuando cambian sus valores https://doc.qt.io/qt-6/signalsandslots.html

	// Reemite el aviso desde MeshManipulator hasta la ventana Qt de que un elemento ha cambiado de posicion
	void onManipulatorPositionChanged(double x, double y, double z);

private:

	int win_w = 1600, win_h = 1200; // Dimensiones iniciales de la pantalla, en pixeles !! YA NO, SE ENCARGA QT

	Camera* camera;

	Mesh* defaultMesh;
	Shader* selectionShader;
	Shader* defaultShader;

	// Clase que sirve para escoger vertices de la malla
	Selector* selector;

	MeshManipulator* meshManipulator;

	SelectionRenderer* selectionRenderer;

	RenderMode renderMode;

	bool initializeGlad(); // !! revisar si nombre corresponde con lo que hace

	// Para mover vertices, segmentos o caras
	int hoveredElement;

	void logic();
	void render();

	// Dibuja el elemento actualmente seleccionado para remarcarlo
	void drawSelection(const glm::mat4& MVP);

};

