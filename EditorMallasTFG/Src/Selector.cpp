#include "Selector.h"

#include "Ray.h"
#include "Mesh.h"
#include "Camera.h"

#include "DebugRenderer.h"

#include <iostream>

Selector::Selector() {
    minSelectDistance = 1.2f;

    ray = new Ray();
    dbRenderer = new DebugRenderer();
}

Selector::~Selector() {
    delete ray;
    ray = nullptr;

    delete dbRenderer;
    dbRenderer = nullptr;
}

int Selector::pickVertex(const Mesh& mesh, float mouseX, float mouseY, int width, int height, const Camera& camera) {
    glm::vec3 rayDir = ray->mouseRay(mouseX, mouseY, width, height, camera.getViewMatrix(), camera.getProjectionMatrix());
    glm::vec3 rayOrigin = camera.getPosition();
    //std::cout << rayOrigin[0] << ", " << rayOrigin[1] << ", " << rayOrigin[2] << std::endl;

    int selectedVertex = -1;
    int minDistFound = minSelectDistance;

    for (int i = 0; i < mesh.vertices.size(); i++)
    {
        glm::vec3 v(mesh.vertices[i].Position[0],
                    mesh.vertices[i].Position[1],
                    mesh.vertices[i].Position[2]);

        float dist = ray->pointToRayDistance(v, rayOrigin, rayDir);

        if (dist < minDistFound) {
            minDistFound = dist;
            selectedVertex = i;
        }
    }

    glm::vec3 end = rayOrigin + rayDir * 10.0f;
    dbRenderer->drawLine(rayOrigin, end);

    return selectedVertex;
}


