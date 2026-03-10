#pragma once

#include <string>
#include <glm.hpp>

struct GLFWwindow;
class Camera;
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
	int win_w = 1600, win_h = 1200; // Dimensiones iniciales de la pantalla, en pixeles
	
	GLFWwindow* window;

	Camera* camera;

	Mesh* defaultMesh;
	Shader* defaultShader;

	bool initializeGLFWAndWindow();
	bool initializeGlad();

	// Cuando hacemos click con el raton, devuelve un rayo casteado que usaremos para ver que puntos de una malla 3D interactuarian
	glm::vec3 mouseClickRay(float mouseX, float mouseY, int w, int h, glm::mat4 view, glm::mat4 proj);

	float pointToRayDistance(glm::vec3 point, glm::vec3 rayOrigin, glm::vec3 rayDir);
};

