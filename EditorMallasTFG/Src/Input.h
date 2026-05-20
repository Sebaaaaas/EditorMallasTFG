#pragma once

#include <unordered_set>

class Input
{
public:

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

    static void setKey(int key, bool pressed);
    static void setMouseButton(int button, bool pressed);
    static void setMousePosition(double x, double y);
    static void addScrollDelta(double delta);

private:

    static std::unordered_set<int> pressedKeys;

    static bool mouseButtons[8];

    static double mouseX;
    static double mouseY;

    static double lastMouseX;
    static double lastMouseY;

    // Movimiento del raton desde el ultimo frame
    static float deltaX;
    static float deltaY;

    static double scrollDelta;
};