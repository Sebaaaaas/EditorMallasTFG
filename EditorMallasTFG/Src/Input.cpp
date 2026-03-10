#include "Input.h"

#include <GLFW/glfw3.h>

bool Input::keys[1024] = { false };
bool Input::mouseButtons[8] = { false };

double Input::mouseX = 0;
double Input::mouseY = 0;

double Input::lastMouseX = 0;
double Input::lastMouseY = 0;

float Input::deltaX = 0;
float Input::deltaY = 0;

double Input::scrollDelta = 0;

void Input::init(GLFWwindow* window)
{
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetScrollCallback(window, scrollCallback);
}

void Input::update()
{
    deltaX = mouseX - lastMouseX;
    deltaY = lastMouseY - mouseY;

    lastMouseX = mouseX;
    lastMouseY = mouseY;
}

bool Input::isKeyDown(int key)
{
    return keys[key];
}

bool Input::isMouseButtonDown(int button)
{
    return mouseButtons[button];
}

float Input::getMouseDeltaX()
{
    return deltaX;
}

float Input::getMouseDeltaY()
{
    return deltaY;
}

double Input::getMouseX()
{
    return mouseX;
}

double Input::getMouseY()
{
    return mouseY;
}

double Input::getScrollDelta()
{
    return scrollDelta;
}

void Input::beginFrame()
{
    scrollDelta = 0;
}

void Input::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key < 0 || key >= 1024) return;

    if (action == GLFW_PRESS)
        keys[key] = true;
    else if (action == GLFW_RELEASE)
        keys[key] = false;
}

void Input::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button < 0 || button >= 8) return;

    if (action == GLFW_PRESS)
        mouseButtons[button] = true;
    else if (action == GLFW_RELEASE)
        mouseButtons[button] = false;
}

void Input::cursorCallback(GLFWwindow* window, double xpos, double ypos)
{
    mouseX = xpos;
    mouseY = ypos;
}

void Input::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) // En un raton normal solo recibimos offset en el eje y
{
    scrollDelta += (float)yoffset;
}
