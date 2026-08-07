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
    transformAxis = TransformAxis::X;

    transformStartMouse = glm::vec2(0.f, 0.f);
}

MeshManipulator::~MeshManipulator() {
    delete ray;
    ray = nullptr;
}

void MeshManipulator::setEditingMesh(Mesh* mesh) {
    currentMesh = mesh;
}

void MeshManipulator::beginTransform(const Camera& camera, float mouseX, float mouseY) {

    if (selectedGroups.empty())
        return;

    dragging = true;

    transformStartMouse = glm::vec2(mouseX, mouseY);

    transformPivot = selectionCenter();

    dragStartPoint = transformPivot;
    dragPlaneNormal = camera.getPosition() - transformPivot;

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

void MeshManipulator::updateTransform(float mouseX, float mouseY, int w, int h, const Camera& camera) {

    switch (transformMode)
    {
    case TransformMode::Translate:
        updateTranslation(mouseX, mouseY, w, h, camera);
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

void MeshManipulator::updateTranslation(float mouseX, float mouseY, int w, int h, const Camera& camera) {
    
    glm::vec3 rayDir = ray->mouseRay(mouseX, mouseY, w, h, camera.getViewMatrix(), camera.getProjectionMatrix());

    glm::vec3 rayOrigin = camera.getPosition();

    glm::vec3 hit = ray->intersectRayPlane(rayOrigin, rayDir, dragStartPoint, dragPlaneNormal);

    glm::vec3 delta = hit - dragStartPoint;

    glm::vec3 axis;

    switch (transformAxis)
    {
    case TransformAxis::X:
        axis = glm::vec3(1, 0, 0);
        break;

    case TransformAxis::Y:
        axis = glm::vec3(0, 1, 0);
        break;

    case TransformAxis::Z:
        axis = glm::vec3(0, 0, 1);
        break;
    }

    float amount = glm::dot(delta, axis);

    translateSelection(axis * amount);
}

void MeshManipulator::updateRotation(float mouseX, float mouseY) {

    float dx = mouseX - transformStartMouse.x;

    // Multiplicamos por 0.1 para que no sea demasiado rapido
    float angle = dx * 0.1f;

    switch (transformAxis)
    {
    case TransformAxis::X:
        rotateSelection(angle, glm::vec3(1, 0, 0));
        break;

    case TransformAxis::Y:
        rotateSelection(angle, glm::vec3(0, 1, 0));
        break;

    case TransformAxis::Z:
        rotateSelection(angle, glm::vec3(0, 0, 1));        
        break;
    }

}

void MeshManipulator::updateScale(float mouseX, float mouseY) {

    float dx = mouseX - transformStartMouse.x;

    // Multiplicamos por 0.01 para que no sea demasiado rapido
    float factor = 1.0f + dx * 0.01f;

    switch (transformAxis)
    {
    case TransformAxis::X:
        scaleSelection(glm::vec3(factor, 1, 1));
        break;

    case TransformAxis::Y:
        scaleSelection(glm::vec3(1, factor, 1));
        break;

    case TransformAxis::Z:
        scaleSelection(glm::vec3(1, 1, factor));
        break;

    case TransformAxis::All:
        scaleSelection(glm::vec3(factor));
        break;
    }
}

void MeshManipulator::selectVertex(int vertexIndex, bool additive) {

    if (!additive)
        clearSelection();

    unsigned int group = currentMesh->vertexToGroup[vertexIndex];

    selectedGroups.insert(group);
}

void MeshManipulator::selectEdge(unsigned int edgeIndex, bool additive) {

    if (!additive)
        clearSelection();

    selectedEdges.insert(edgeIndex);

    const Edge& edge = currentMesh->edges[edgeIndex];

    selectedGroups.insert(currentMesh->vertexToGroup[edge.v0]);
    selectedGroups.insert(currentMesh->vertexToGroup[edge.v1]);
}

void MeshManipulator::selectPolygon(unsigned int polygonIndex, bool additive) {

    if (!additive)
        clearSelection();

    selectedPolygons.insert(polygonIndex);

    const Polygon& polygon = currentMesh->polygons[polygonIndex];

    for (unsigned int vertex : polygon.vertices) {
        selectedGroups.insert(currentMesh->vertexToGroup[vertex]);
    }
}

void MeshManipulator::clearSelection() {    
    selectedGroups.clear();
    selectedEdges.clear();
    selectedPolygons.clear();
}

bool MeshManipulator::hasSelection() const {
    return !selectedGroups.empty() || !selectedEdges.empty() || !selectedPolygons.empty();
}

const std::unordered_set<unsigned int>& MeshManipulator::getSelectedGroups() const {
    return selectedGroups;
}

const std::unordered_set<unsigned int>& MeshManipulator::getSelectedEdges() const {
    return selectedEdges;
}

const std::unordered_set<unsigned int>& MeshManipulator::getSelectedPolygons() const {
    return selectedPolygons;
}

std::unordered_set<unsigned int> MeshManipulator::getSelectedGroups() {
    return selectedGroups;
}

void MeshManipulator::setTransformMode(TransformMode mode) {
    transformMode = mode;
}

void MeshManipulator::setTransformAxis(TransformAxis axis) {
    transformAxis = axis;
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

    for (unsigned int group : selectedGroups) {
        for (unsigned int idx : currentMesh->vertexGroups[group]) {
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
