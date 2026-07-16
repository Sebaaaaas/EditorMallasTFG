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
	bool loadMesh(const std::string& path);

	void setSelectionMode(SelectionMode mode);

	bool hasSelection() const;

	glm::vec3 getSelectionPosition() const;

	MeshManipulator* getMeshManipulator() const;


private:

	int win_w = 1600, win_h = 1200; // Dimensiones iniciales de la pantalla, en pixeles !! YA NO, SE ENCARGA QT

	Camera* camera;

	Mesh* defaultMesh;
	Shader* defaultShader;
	Shader* debugShader;

	// Clase que sirve para escoger vertices de la malla
	Selector* selector;

	MeshManipulator* meshManipulator;

	DebugRenderer* debugRenderer;

	bool initializeGlad(); // !! revisar si nombre corresponde con lo que hace

	// Para mover vertices, segmentos o caras  !! deberia estar aqui esto?
	/*int selectedVertex = -1;
	int selectedEdge = -1;*/
	unsigned int selectedElement = -1;

	void logic();
	void render();
};

