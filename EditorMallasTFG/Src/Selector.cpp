#include "Selector.h"

#include "Ray.h"
#include "Mesh.h"
#include "Camera.h"

#include "DebugRenderer.h"

#include <iostream>

Selector::Selector() {
    minSelectDistance = 1.7f;

    ray = new Ray();
    dbRenderer = new DebugRenderer();
}

Selector::~Selector() {
    delete ray;
    ray = nullptr;

    delete dbRenderer;
    dbRenderer = nullptr;
}

int Selector::pickVertex(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera) {

    glm::vec3 rayDir = ray->mouseRay(mouseX, mouseY, width, height, camera->getViewMatrix(), camera->getProjectionMatrix());
    glm::vec3 rayOrigin = camera->getPosition();

    /*int selectedVertex = -1;
    int minDistFound = minSelectDistance;

    for (int i = 0; i < mesh.vertices.size(); i++)
    {
        glm::vec3 v = mesh.vertices[i].Position;

        float dist = ray->pointToRayDistance(v, rayOrigin, rayDir);

        if (dist < minDistFound) {
            minDistFound = dist;
            selectedVertex = i;
        }
    }*/
    int selectedVertex = -1;
    float bestDist = minSelectDistance;
    float bestT = FLT_MAX;

    for (int i = 0; i < mesh.vertices.size(); i++)
    {
        glm::vec3 v = mesh.vertices[i].Position;

        glm::vec3 toPoint = v - rayOrigin;
        float t = glm::dot(toPoint, rayDir);

        if (t < 0.0f) continue; // behind camera

        glm::vec3 closestPoint = rayOrigin + rayDir * t;
        float dist = glm::length(v - closestPoint);

        if (dist < bestDist && t < bestT)
        {
            bestDist = dist;
            bestT = t;
            selectedVertex = i;
        }
    }

    glm::vec3 end = rayOrigin + rayDir * 10.0f; // ! magic number

    dbRenderer->drawLine(rayOrigin, end);
    dbRenderer->drawPoint(end);

    return selectedVertex;
}


