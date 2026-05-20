#include "Input.h"

std::unordered_set<int> Input::pressedKeys;

bool Input::mouseButtons[8] = { false };

double Input::mouseX = 0;
double Input::mouseY = 0;

double Input::lastMouseX = 0;
double Input::lastMouseY = 0;

float Input::deltaX = 0;
float Input::deltaY = 0;

double Input::scrollDelta = 0;

void Input::update() {

    deltaX = mouseX - lastMouseX;
    deltaY = lastMouseY - mouseY;

    lastMouseX = mouseX;
    lastMouseY = mouseY;
}

bool Input::isKeyDown(int key) {
    return pressedKeys.count(key) > 0;
}

bool Input::isMouseButtonDown(int button) {
    return mouseButtons[button];
}

float Input::getMouseDeltaX() {
    return deltaX;
}

float Input::getMouseDeltaY() {
    return deltaY;
}

double Input::getMouseX() {
    return mouseX;
}

double Input::getMouseY() {
    return mouseY;
}

double Input::getScrollDelta() {
    return scrollDelta;
}

void Input::beginFrame() {
    scrollDelta = 0;
}

void Input::setKey(int key, bool pressed) {
    
    if (pressed)
        pressedKeys.insert(key);
    else
        pressedKeys.erase(key);
}

void Input::setMouseButton(int button, bool pressed) {

    if (button < 0 || button >= 8)
        return;

    mouseButtons[button] = pressed;
}

void Input::setMousePosition(double x, double y) {
    mouseX = x;
    mouseY = y;
}

void Input::addScrollDelta(double delta) {
    scrollDelta += delta;
}
