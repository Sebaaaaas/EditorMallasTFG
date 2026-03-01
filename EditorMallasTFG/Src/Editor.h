#pragma once

#define GLFW_INCLUDE_NONE

#include <string>

#include <glad/gl.h>
// GLFW include siempre despues de glad
#include <GLFW/glfw3.h>
#include <stdio.h>

class Editor
{
public:
	Editor();
	~Editor();

	bool init();
	void release();
	void run();

private:
	std::string windowTitle;
	GLFWwindow* window;
};

