#include "MeshManipulator.h"

#include "Mesh.h"
#include "Camera.h"
#include "Ray.h"

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

    if (vertexIndex == -1)
        return;

    dragging = true;

    const auto& v = mesh->vertices[vertexIndex];

    dragStartPoint = v.Position;

    dragPlaneNormal = camera.getPosition() - v.Position;
}

void MeshManipulator::updateDrag(Mesh* mesh, float mouseX, float mouseY, int w, int h, const Camera& camera) {

    glm::vec3 rayDir = ray->mouseRay(mouseX, mouseY, w, h,
        camera.getViewMatrix(), camera.getProjectionMatrix());

    glm::vec3 rayOrigin = camera.getPosition();

    glm::vec3 hit = ray->intersectRayPlane(
        rayOrigin, rayDir,
        dragStartPoint, dragPlaneNormal
    );

    glm::vec3 delta = hit - dragStartPoint;

    // Movemos los grupos seleccionados
    for (unsigned int group : selectedGroups)
    {
        for (unsigned int idx : mesh->vertexGroups[group])
        {
            mesh->vertices[idx].Position += delta;
        }
    }

    // Recalculamos normales para pintado con shading correcto
    mesh->recalculateNormals();

    mesh->updateAllVertices();

    dragStartPoint = hit;
}

void MeshManipulator::endDrag() {
    dragging = false;
}

void MeshManipulator::selectVertex(const Mesh* mesh, int vertexIndex, bool additive) {

    if (vertexIndex == -1)
        return;

    unsigned int group = mesh->vertexToGroup[vertexIndex];

    if (!additive)
        selectedGroups.clear();

    selectedGroups.insert(group);
}

void MeshManipulator::clearSelection() {
    
    selectedGroups.clear();
}

bool MeshManipulator::hasSelection() const {

    return !selectedGroups.empty();
}
