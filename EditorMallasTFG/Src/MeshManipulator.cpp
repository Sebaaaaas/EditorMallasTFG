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

    /*if (vertexIndex != -1) {
        selectedVertex = vertexIndex;
        dragging = true;
    }*/

    if (vertexIndex == -1) return;

    selectedVertex = vertexIndex;
    dragging = true;

    const auto& v = mesh->vertices[selectedVertex];

    // Plane goes through the vertex
    dragStartPoint = v.Position;

    // Plane faces the camera
    dragPlaneNormal = camera.getPosition() - v.Position;
    // OR: (camera.getPosition() - v.Position)
}

void MeshManipulator::updateDrag(Mesh* mesh, float mouseX, float mouseY, int w, int h, const Camera& camera) {

    glm::vec3 rayDir = ray->mouseRay(mouseX, mouseY, w, h, camera.getViewMatrix(), camera.getProjectionMatrix());

    glm::vec3 rayOrigin = camera.getPosition();

    //std::cout << rayOrigin[0] << ", " << rayOrigin[1] << ", " << rayOrigin[2] << std::endl;

    glm::vec3 hit = ray->intersectRayPlane(rayOrigin, rayDir, dragStartPoint, dragPlaneNormal);

    glm::vec3 delta = hit - dragStartPoint;

    auto& v = mesh->vertices[selectedVertex];
    std::cout << selectedVertex << std::endl;
    //std::cout << "Pre: " << v.Position.x << std::endl;

    v.Position += delta;

    //std::cout << "Pos: " << v.Position.x << std::endl;


    dragStartPoint = hit;

    mesh->updateVertex(selectedVertex);
}

void MeshManipulator::endDrag() {
    selectedVertex = -1;
    dragging = false;
}