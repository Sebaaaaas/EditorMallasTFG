#pragma once

#include <glm.hpp>

class Ray
{
public:
    Ray();
    ~Ray();

    glm::vec3 mouseRay(float mouseX, float mouseY, int width, int height, const glm::mat4& view, const glm::mat4& projection);

    float pointToRayDistance(glm::vec3 point, glm::vec3 rayOrigin, glm::vec3 rayDir);

    glm::vec3 intersectRayPlane(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 planePoint, glm::vec3 planeNormal);

    // Algoritmo Moller Trumbore
    bool intersectTriangle(const glm::vec3& rayOrigin, const glm::vec3& rayDir, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t) const;

};