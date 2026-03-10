#pragma once

//#include <GLFW/glfw3.h>
struct GLFWwindow;

// https://www.glfw.org/docs/latest/input_guide.html

class Input
{
public:

    static void init(GLFWwindow* window);

    static bool isKeyDown(int key);
    static bool isMouseButtonDown(int button);

    static float getMouseDeltaX();
    static float getMouseDeltaY();

    static double getMouseX();
    static double getMouseY();

    static double getScrollDelta();

    // Para resetar variables al comienzo del frame, debe llamarse antes que glfwPollEvents();
    static void beginFrame();
    static void update();

private:

    static bool keys[1024];
    static bool mouseButtons[8];

    static double mouseX;
    static double mouseY;

    static double lastMouseX;
    static double lastMouseY;

    // Movimiento del raton desde el ultimo frame
    static float deltaX;
    static float deltaY;

    static double scrollDelta;

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};