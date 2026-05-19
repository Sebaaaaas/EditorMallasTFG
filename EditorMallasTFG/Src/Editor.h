#pragma once

#include <string>
#include <glm.hpp>

struct GLFWwindow;

class MeshManipulator;
class Selector;
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
	void renderFrame();

private:

	const std::string windowTitle = "Mesh editor";
	int win_w = 1600, win_h = 1200; // Dimensiones iniciales de la pantalla, en pixeles
	
	GLFWwindow* window;

	Camera* camera;

	Mesh* defaultMesh;
	Shader* defaultShader;
	Shader* debugShader;

	// Clase que sirve para escoger vertices de la malla
	Selector* selector;

	MeshManipulator* meshManipulator;

	DebugRenderer* debugRenderer;

	bool initializeGlad();

	void manageInput();


	// Mover vertice !! deberia estar aqui esto?
	int selectedVertex = -1;


};

