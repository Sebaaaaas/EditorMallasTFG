#pragma once

#define GLFW_INCLUDE_NONE

#include <string>

#include <glad/gl.h>
// GLFW include siempre despues de glad
#include <GLFW/glfw3.h>
#include <stdio.h>

class Shader;

class Editor
{
public:
	Editor();
	~Editor();

	bool init();
	void release();
	void run();

private:

	const std::string windowTitle = "Mesh editor";
	int win_w = 800, win_h = 600; // Dimensiones iniciales de la pantalla, en pixeles
	GLFWwindow* window;

	Shader* shader;

	bool initializeWindow();
};

