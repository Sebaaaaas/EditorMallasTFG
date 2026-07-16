#include "MeshManipulator.h"

#include "Mesh.h"
#include "Camera.h"
#include "Ray.h"
#include "Editor.h"

MeshManipulator::MeshManipulator() {
    dragPlaneNormal = glm::vec3(0.f, 0.f, 0.f);
    dragStartPoint = glm::vec3(0.f, 0.f, 0.f);

    ray = nullptr;
    currentMesh = nullptr;
}

MeshManipulator::~MeshManipulator() {
    delete ray;
    ray = nullptr;
}

void MeshManipulator::setEditingMesh(Mesh* mesh) {
    currentMesh = mesh;
}

void MeshManipulator::beginDrag(const Mesh* mesh, const std::vector<unsigned int>& vertexIndex, const Camera& camera) {

    if (vertexIndex.empty())
        return;

    dragging = true;

    glm::vec3 averageVertexPosition = mesh->vertices[vertexIndex[0]].Position;

    for(int i = 1; i < vertexIndex.size(); ++i)
        averageVertexPosition += mesh->vertices[vertexIndex[i]].Position;

    averageVertexPosition /= vertexIndex.size();

    dragStartPoint = averageVertexPosition;

    dragPlaneNormal = camera.getPosition() - dragStartPoint;
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

    glm::vec3 averagePos = glm::vec3(0);
    int quantity = 0;
    // Movemos los grupos seleccionados
    for (unsigned int group : selectedGroups)
    {
        for (unsigned int idx : mesh->vertexGroups[group])
        {
            mesh->vertices[idx].Position += delta;
            averagePos += mesh->vertices[idx].Position;
            quantity++;
        }
    }

    averagePos /= quantity;

    // Recalculamos normales para pintado con shading correcto
    mesh->recalculateNormals();

    mesh->updateAllVertices();

    dragStartPoint = hit;

    emit selectedPositionChanged(averagePos.x, averagePos.y, averagePos.z); // !! erroneo, debe ser la media de posiciones
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

std::unordered_set<unsigned int> MeshManipulator::getSelectedGroups()
{
    return selectedGroups;
}

void MeshManipulator::setSelectedXPosition(double value) {
    
    glm::vec3 center = selectionCenter();
    translateSelection(glm::vec3(value - center.x, 0.0f, 0.0f));
}

void MeshManipulator::setSelectedYPosition(double value) {

    glm::vec3 center = selectionCenter();
    translateSelection(glm::vec3(0.0f, value - center.y, 0.0f));
}

void MeshManipulator::setSelectedZPosition(double value) {

    glm::vec3 center = selectionCenter();
    translateSelection(glm::vec3(0.0f, 0.0f, value - center.z));
}

glm::vec3 MeshManipulator::selectionCenter() const {

    glm::vec3 average(0.0f);
    int count = 0;

    for (unsigned int group : selectedGroups)
    {
        for (unsigned int idx : currentMesh->vertexGroups[group])
        {
            average += currentMesh->vertices[idx].Position;
            ++count;
        }
    }

    if (count == 0)
        return glm::vec3(0.0f);

    average /= count;

    return average;
}

void MeshManipulator::translateSelection(const glm::vec3& delta) {
    for (unsigned int group : selectedGroups)
    {
        for (unsigned int idx : currentMesh->vertexGroups[group])
        {
            currentMesh->vertices[idx].Position += delta;
        }
    }

    // Recalculamos normales para pintado con shading correcto !!deberia hacer que esto fuera solo para los vertices relevantes, 
    //                                                         no hace falta recalcular todo
    currentMesh->recalculateNormals(); 
    currentMesh->updateAllVertices();

    glm::vec3 center = selectionCenter();
    emit selectedPositionChanged(center.x, center.y, center.z);
}