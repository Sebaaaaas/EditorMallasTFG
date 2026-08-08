#pragma once

#include <unordered_set>

// !!IMPORTANTE: Hay dos fuentes de input, aqui y en MainWindow. El input de MainWindow lo "absorbe" Qt, 
// por lo que si no se recibe, revisar que no sea por eso
class Input
{
public:

    static bool isKeyDown(int key);
    static bool isKeyPressed(int key);
    static bool isMouseButtonDown(int button);

    static float getMouseDeltaX();
    static float getMouseDeltaY();

    static double getMouseX();
    static double getMouseY();

    static double getScrollDelta();

    static void endFrame();
    static void update();

    static void setKey(int key, bool pressed);
    static void setMouseButton(int button, bool pressed);
    static void setMousePosition(double x, double y);
    static void addScrollDelta(double delta);

private:

    static std::unordered_set<int> pressedKeys;
    static std::unordered_set<int> previousPressedKeys; // Para frame anterior, permite saber si una tecla se ha pulsado este frame

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