#pragma once

#include <glm.hpp>
#include <string>

#include "Selector.h"

class MeshManipulator;
class Camera;
class Shader;
class Mesh;

class DebugRenderer;

class Editor
{
public:

	Editor();
	~Editor();

	bool init();
	void run();

	void setWindowSize(int w, int h);

	// Intenta cargar la malla en formato .obj en el directorio path(dentro de Bin/Assets). Si falla emite mensaje de error
	bool loadMesh(const std::string& path);

	void setSelectionMode(SelectionMode mode);

	MeshManipulator* getMeshManipulator() const;


private:

	int win_w = 1600, win_h = 1200; // Dimensiones iniciales de la pantalla, en pixeles !! YA NO, SE ENCARGA QT

	Camera* camera;

	Mesh* defaultMesh;
	Shader* debugShader;
	Shader* defaultShader;

	// Clase que sirve para escoger vertices de la malla
	Selector* selector;

	MeshManipulator* meshManipulator;

	DebugRenderer* debugRenderer;

	bool initializeGlad(); // !! revisar si nombre corresponde con lo que hace

	// Para mover vertices, segmentos o caras
	unsigned int hoveredElement = -1;

	void logic();
	void render();

	// Dibuja el elemento actualmente seleccionado para remarcarlo
	void drawDebug(const glm::mat4& MVP);
};

