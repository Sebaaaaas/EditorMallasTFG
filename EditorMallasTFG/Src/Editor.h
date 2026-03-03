#pragma once

#include <string>

struct GLFWwindow;
class Shader;
class Mesh;

class Editor
{
public:
	Editor();
	~Editor();

	bool init();
	void run();

private:

	const std::string windowTitle = "Mesh editor";
	int win_w = 800, win_h = 600; // Dimensiones iniciales de la pantalla, en pixeles
	GLFWwindow* window;

	Mesh* defaultMesh;
	Shader* defaultShader;

	bool initializeGLFWAndWindow();
	bool initializeGlad();
};

