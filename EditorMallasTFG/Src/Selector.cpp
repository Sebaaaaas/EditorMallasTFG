#include "Selector.h"

#include "Ray.h"
#include "Mesh.h"
#include "Camera.h"

#include "DebugRenderer.h"

Selector::Selector() {
    minSelectDistance = 0.7f; // !! deberia cambiar con la distancia?

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
    
    int selectedVertex = -1;
    float bestDist = minSelectDistance;

    for (int i = 0; i < mesh.vertices.size(); i++) {
        glm::vec3 v = mesh.vertices[i].Position;

        glm::vec3 toPoint = v - rayOrigin;
        float t = glm::dot(toPoint, rayDir); // Proyectamos el vertice sobre el rayo lanzado

        if (t < 0.0f) continue; // Detras de la camara

        glm::vec3 closestPoint = rayOrigin + rayDir * t;
        float dist = glm::length(v - closestPoint);

        if (dist < bestDist) {
            bestDist = dist;
            selectedVertex = i;
        }
    }

    glm::vec3 end = rayOrigin + rayDir * 10.0f; // !! numero magico

    dbRenderer->drawLine(rayOrigin, end); // !! problema con shader
    dbRenderer->drawPoint(end);

    return selectedVertex;
}


