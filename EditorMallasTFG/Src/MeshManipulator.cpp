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

    transformPivot = glm::vec3(0.f, 0.f, 0.f);
    transformMode = TransformMode::Translate;

    transformStartMouse = glm::vec2(0.f, 0.f);
}

MeshManipulator::~MeshManipulator() {
    delete ray;
    ray = nullptr;
}

void MeshManipulator::setEditingMesh(Mesh* mesh) {
    currentMesh = mesh;
}

void MeshManipulator::beginTransform(const Mesh* mesh, const std::vector<unsigned int>& vertexIndex, const Camera& camera, float mouseX, float mouseY) {

    if (vertexIndex.empty())
        return;

    dragging = true;

    transformStartMouse = glm::vec2(mouseX, mouseY);

    glm::vec3 averageVertexPosition = mesh->vertices[vertexIndex[0]].Position;

    for(int i = 1; i < vertexIndex.size(); ++i)
        averageVertexPosition += mesh->vertices[vertexIndex[i]].Position;

    averageVertexPosition /= vertexIndex.size();

    dragStartPoint = averageVertexPosition;

    dragPlaneNormal = camera.getPosition() - dragStartPoint;

    // Guardamos valores antes de empezar la transformacion pertinente
    selectedVertices.clear();
    originalPositions.clear();

    transformPivot = selectionCenter();

    for (unsigned int group : selectedGroups) {
        for (unsigned int idx : currentMesh->vertexGroups[group]) {
            selectedVertices.push_back(idx);
            originalPositions.push_back(currentMesh->vertices[idx].Position);
        }
    }
}

void MeshManipulator::updateTransform(Mesh* mesh, float mouseX, float mouseY, int w, int h, const Camera& camera) {

    switch (transformMode)
    {
    case TransformMode::Translate:
        updateTranslation(mesh, mouseX, mouseY, w, h, camera);
        break;

    case TransformMode::Rotate:
        updateRotation(mouseX, mouseY);
        break;

    case TransformMode::Scale:
        updateScale(mouseX, mouseY);
        break;
    }
}

void MeshManipulator::endTransform() {
    dragging = false;
}

void MeshManipulator::updateTranslation(Mesh* mesh, float mouseX, float mouseY, int w, int h, const Camera& camera) {
    
    glm::vec3 rayDir = ray->mouseRay(mouseX, mouseY, w, h, camera.getViewMatrix(), camera.getProjectionMatrix());

    glm::vec3 rayOrigin = camera.getPosition();

    glm::vec3 hit = ray->intersectRayPlane(rayOrigin, rayDir, dragStartPoint, dragPlaneNormal);

    glm::vec3 delta = hit - dragStartPoint;

    translateSelection(delta);
}

void MeshManipulator::updateRotation(float mouseX, float mouseY) {

    float dx = mouseX - transformStartMouse.x;

    // Multiplicamos por 0.1 para que no sea demasiado rapido
    float angle = dx * 0.1f;

    rotateSelection(angle, glm::vec3(0, 1, 0));
}

void MeshManipulator::updateScale(float mouseX, float mouseY) {

    float dx = mouseX - transformStartMouse.x;

    // Multiplicamos por 0.01 para que no sea demasiado rapido
    float factor = 1.0f + dx * 0.01f;

    scaleSelection(glm::vec3(factor));
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

std::unordered_set<unsigned int> MeshManipulator::getSelectedGroups() {
    return selectedGroups;
}

void MeshManipulator::setTransformMode(TransformMode mode) {
    transformMode = mode;
}

TransformMode MeshManipulator::getTransformMode() const {
    return transformMode;
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

void MeshManipulator::transformSelection(const glm::mat4& transform) {

    for (size_t i = 0; i < selectedVertices.size(); ++i) {

        unsigned int idx = selectedVertices[i];

        glm::vec3 local = originalPositions[i] - transformPivot;

        glm::vec3 transformed =
            glm::vec3(transform * glm::vec4(local, 1.0f));

        currentMesh->vertices[idx].Position = transformPivot + transformed;
    }

    currentMesh->recalculateNormals();
    currentMesh->updateAllVertices();

    glm::vec3 center = selectionCenter();
    emit selectedPositionChanged(center.x, center.y, center.z);
}

void MeshManipulator::translateSelection(const glm::vec3& delta) {
    
    glm::mat4 transform(1.0f);

    transform = glm::translate(transform, delta);

    transformSelection(transform);
}

void MeshManipulator::rotateSelection(float angle, glm::vec3 axis) {

    glm::mat4 transform(1.0f);

    transform = glm::rotate(transform, glm::radians(angle), glm::normalize(axis));

    transformSelection(transform);
}

void MeshManipulator::scaleSelection(glm::vec3 scale) {
    
    glm::mat4 transform(1.0f);

    transform = glm::scale(transform, scale);

    transformSelection(transform);
}
