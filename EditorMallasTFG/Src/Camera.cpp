#include "Camera.h"

//#include <glm.hpp>

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
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, target, up);
}

glm::mat4 Camera::getProjectionMatrix() const {
    return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::setAspectRatio(float width, float height)
{
    if (height == 0.0f)
        return;

    aspectRatio = width / height;
}

void Camera::setPosition(const glm::vec3& pos)
{
    position = pos;
}

void Camera::setTarget(const glm::vec3& newTarget)
{
    target = newTarget;
}