#include "Ray.h"

#include <iostream>

Ray::Ray()
{
}

Ray::~Ray()
{
}

// https://antongerdelan.net/opengl/raycasting.html
glm::vec3 Ray::mouseRay(float mouseX, float mouseY, int w, int h, const glm::mat4& view, const glm::mat4& proj) {

    // Dejamos los valores en los rangos [-1...1](normalizamos), invirtiendo la y por ir al reves en OpenGL. Z es innecesaria
    float x = (2.0f * mouseX) / w - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / h;

    // Apuntamos hacia delante, que en OpenGL es negativo en el eje Z, y convertimos a vec4 para posteriores calculos
    glm::vec4 rayClip = glm::vec4(x, y, -1.0, 1.0);

    // Coordenadas de la camara
    glm::vec4 rayEye = glm::inverse(proj) * rayClip;
    rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0);

    // Coordenadas en el mundo
    glm::vec3 rayWorld = glm::normalize(glm::vec3(glm::inverse(view) * rayEye));

    return rayWorld;
}

float Ray::pointToRayDistance(glm::vec3 point, glm::vec3 rayOrigin, glm::vec3 rayDir)
{
    glm::vec3 diff = point - rayOrigin;
    glm::vec3 proj = glm::dot(diff, rayDir) * rayDir;
    return glm::length(diff - proj);
}

glm::vec3 Ray::intersectRayPlane(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 planePoint, glm::vec3 planeNormal)
{
    float denom = glm::dot(rayDir, planeNormal);

    if (abs(denom) < 0.0001f)
        return planePoint;

    float t = glm::dot(planePoint - rayOrigin, planeNormal) / denom;

    return rayOrigin + rayDir * t;
}