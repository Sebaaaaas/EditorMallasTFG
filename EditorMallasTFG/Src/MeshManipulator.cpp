#include "MeshManipulator.h"

#include "Mesh.h"
#include "Camera.h"
#include "Ray.h"

#include <iostream>

MeshManipulator::MeshManipulator() {
    dragPlaneNormal = glm::vec3(0.f, 0.f, 0.f);
    dragStartPoint = glm::vec3(0.f, 0.f, 0.f);

    ray = nullptr;
}

MeshManipulator::~MeshManipulator() {
    delete ray;
    ray = nullptr;
}

void MeshManipulator::beginDrag(const Mesh* mesh, int vertexIndex, const Camera& camera) {
    if (vertexIndex != -1)
    {
        selectedVertex = vertexIndex;
        dragging = true;
    }
}

void MeshManipulator::updateDrag(Mesh* mesh, float mouseX, float mouseY, int w, int h, const Camera& camera) {
    glm::vec3 rayDir = ray->mouseRay(mouseX, mouseY, w, h, camera.getViewMatrix(), camera.getProjectionMatrix());

    glm::vec3 rayOrigin = camera.getPosition();

    //std::cout << rayOrigin[0] << ", " << rayOrigin[1] << ", " << rayOrigin[2] << std::endl;

    glm::vec3 hit = ray->intersectRayPlane(rayOrigin, rayDir, dragStartPoint, dragPlaneNormal);

    glm::vec3 delta = hit - dragStartPoint;

    auto& v = mesh->vertices[selectedVertex];
    std::cout << selectedVertex << std::endl;

    v.Position[0] += delta.x;
    v.Position[1] += delta.y;
    v.Position[2] += delta.z;

    dragStartPoint = hit;

    // Update GPU
    mesh->updateVertex(selectedVertex);
}

void MeshManipulator::endDrag() {
    selectedVertex = -1;
    dragging = false;
}