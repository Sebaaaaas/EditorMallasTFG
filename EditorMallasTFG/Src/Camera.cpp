#include "Camera.h"

#include <GLFW/glfw3.h>

#include "Input.h"

Camera::Camera(float width, float height)
{
    position = glm::vec3(0.0f, 0.0f, 5.0f);
    target = glm::vec3(0.0f, 0.0f, 0.0f);
    up = glm::vec3(0.0f, 1.0f, 0.0f);

    fov = 45.0f;
    
    if (height == 0.0f)
        height = 1.0f;

    aspectRatio = width / height;

    nearPlane = 0.1f;
    farPlane = 100.0f;

    yaw = -90.0f;
    pitch = 0.0f;
    orbitSensitivity = 0.1f;
    panSensitivity = 0.001f;
    zoomSensitivity = 0.1f;
    distance = glm::length(position - target);
}

glm::vec3 Camera::getPosition() const {
    return position;
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::manageInput() {

    if (Input::isMouseButtonDown(GLFW_MOUSE_BUTTON_MIDDLE))
    {
        if (Input::isKeyDown(GLFW_KEY_LEFT_SHIFT))
        {
            pan(Input::getMouseDeltaX(), Input::getMouseDeltaY());
        }
        else
        {
            orbit(Input::getMouseDeltaX(), Input::getMouseDeltaY());
        }
    }

    float scroll = Input::getScrollDelta();

    if (scroll != 0.0f)
    {
        zoom(scroll);
    }

    if (Input::isKeyDown(GLFW_KEY_F))
    {
        reset();
    }
}

void Camera::setAspectRatio(float width, float height) {
    if (height == 0.0f)
        return;

    aspectRatio = width / height;
}

void Camera::setPosition(const glm::vec3& pos) {
    position = pos;
}

void Camera::setTarget(const glm::vec3& newTarget) {
    target = newTarget;
}

glm::vec3 Camera::getTarget() const {
    return target;
}

void Camera::reset() {

    setTarget(glm::vec3(0, 0, 0));
    setPosition(glm::vec3(0, 0, 0) + glm::vec3(0, 0, 5));

    yaw = -90.0f;
    pitch = 0.0f;

    distance = glm::length(position - target);
}

void Camera::orbit(float xoffset, float yoffset) {
    yaw += xoffset * orbitSensitivity;
    pitch += yoffset * orbitSensitivity;

    // Asignamos limites
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    glm::vec3 direction;

    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    direction = glm::normalize(direction);

    position = target - direction * distance;
}

void Camera::pan(float xoffset, float yoffset) {
    glm::vec3 forward = glm::normalize(target - position);
    glm::vec3 right = glm::normalize(glm::cross(forward, up));
    glm::vec3 camUp = glm::normalize(glm::cross(right, forward));

    glm::vec3 move = (-right * xoffset - camUp * yoffset) * distance * panSensitivity;

    position += move;
    target += move;
}

void Camera::zoom(float amount) {
    // Zoom que dependa de la distancia al objeto lo hace mas "intuitivo"
    distance -= amount * (distance * zoomSensitivity);

    glm::vec3 direction = glm::normalize(position - target);
    position = target + direction * distance;
}
