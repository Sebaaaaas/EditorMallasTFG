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

void MeshManipulator::beginTransform(const Camera& camera, float mouseX, float mouseY, int w, int h) {

    if (selectedGroups.empty())
        return;

    dragging = true;

    transformStartMouse = glm::vec2(mouseX, mouseY);

    transformPivot = selectionCenter();

    dragPlaneNormal = camera.getPosition() - transformPivot;

    // Encontramos el punto real donde el rayo del click inicial corta el plano de arrastre, para que el delta de traslacion empiece en cero sin importar donde dentro de la
    // seleccion se haya hecho click
    glm::vec3 rayDir = ray->mouseRay(mouseX, mouseY, w, h, camera.getViewMatrix(), camera.getProjectionMatrix());
    glm::vec3 rayOrigin = camera.getPosition();

    dragStartPoint = ray->intersectRayPlane(rayOrigin, rayDir, transformPivot, dragPlaneNormal);


    // Guardamos valores antes de empezar la transformacion pertinente
    selectedVertices.clear();
    originalPositions.clear();

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

void MeshManipulator::setTransformMode(TransformMode mode) {
    transformMode = mode;
}

void MeshManipulator::setTransformAxis(TransformAxis axis) {
    transformAxis = axis;
}

void MeshManipulator::extrudeSelection(float distance) {

    //if (!currentMesh || selectedPolygons.empty())
    //    return;

    //for (unsigned int polygonIndex : selectedPolygons) {

    //    const Polygon base = currentMesh->polygons[polygonIndex];

    //    glm::vec3 normal = currentMesh->polygonNormal(polygonIndex);

    //    std::vector<unsigned int> topVertices;

    //    for (unsigned int vertexIndex : base.vertices) {

    //        unsigned int newVertex = currentMesh->addVertex(currentMesh->vertices[vertexIndex]);

    //        topVertices.push_back(newVertex);
    //    }

    //    // Movemos los vertices una pequenia distancia para diferenciarlos
    //    moveVerticesAlongNormal(topVertices, normal, distance);

    //    // Deja de existir la cara anterior, cambiamos el poligono para que use los nuevos vertices
    //    currentMesh->polygons[polygonIndex].vertices = topVertices;

    //    // Construimos los nuevos poligonos que se forman desde la base hasta el poligono ahora extruido
    //    createSidePolygons(base.vertices, topVertices);
    //}

    //currentMesh->rebuildTopology();
    if (!currentMesh || selectedPolygons.empty())
        return;

    // 1. Accumulate normal contributions per GROUP (shared position)
    std::unordered_map<unsigned int, glm::vec3> normalSum;
    for (unsigned int polygonIndex : selectedPolygons) {
        glm::vec3 normal = currentMesh->polygonNormal(polygonIndex);
        for (unsigned int v : currentMesh->polygons[polygonIndex].vertices) {
            unsigned int group = currentMesh->vertexToGroup[v];
            normalSum[group] += normal;
        }
    }

    // 2. Create exactly one new vertex per GROUP
    std::unordered_map<unsigned int, unsigned int> groupToNew;
    for (auto& [group, sum] : normalSum) {
        unsigned int templateVertex = currentMesh->vertexGroups[group][0];
        Vertex newVertex = currentMesh->vertices[templateVertex];
        newVertex.Position += glm::normalize(sum) * distance;
        groupToNew[group] = currentMesh->addVertex(newVertex);
    }

    // 3. Count how many selected faces use each edge (by group pair), so we
    //    only wall boundary edges of the selection, not interior shared edges
    std::map<std::pair<unsigned int, unsigned int>, int> edgeUseCount;

    for (unsigned int polygonIndex : selectedPolygons) {

        const std::vector<unsigned int>& baseVerts = currentMesh->polygons[polygonIndex].vertices;
        size_t count = baseVerts.size();

        for (size_t i = 0; i < count; ++i) {

            unsigned int groupA = currentMesh->vertexToGroup[baseVerts[i]];
            unsigned int groupB = currentMesh->vertexToGroup[baseVerts[(i + 1) % count]];

            edgeUseCount[std::minmax(groupA, groupB)]++;
        }
    }

    // 4. Rebuild each selected face; only wall edges used by exactly one selected face
    for (unsigned int polygonIndex : selectedPolygons) {

        const Polygon base = currentMesh->polygons[polygonIndex];
        size_t count = base.vertices.size();

        std::vector<unsigned int> topVertices;
        for (unsigned int v : base.vertices) {
            unsigned int group = currentMesh->vertexToGroup[v];
            topVertices.push_back(groupToNew[group]);
        }

        for (size_t i = 0; i < count; ++i) {

            size_t next = (i + 1) % count;

            unsigned int groupA = currentMesh->vertexToGroup[base.vertices[i]];
            unsigned int groupB = currentMesh->vertexToGroup[base.vertices[next]];

            // Interior edge shared with another selected face - skip, no wall needed
            if (edgeUseCount[std::minmax(groupA, groupB)] > 1)
                continue;

            Polygon side;
            side.vertices = {
                base.vertices[i],
                base.vertices[next],
                topVertices[next],
                topVertices[i]
            };

            currentMesh->addPolygon(side);
        }

        currentMesh->polygons[polygonIndex].vertices = topVertices;
    }

    currentMesh->rebuildTopology();
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

void MeshManipulator::moveVerticesAlongNormal(std::vector<unsigned int> vertexIndices, glm::vec3 normal, float distance) {

    for (unsigned int vertexIndex : vertexIndices) {
        currentMesh->vertices[vertexIndex].Position += normal * distance;
    }
}

void MeshManipulator::createSidePolygons(const std::vector<unsigned int>& baseVertices, const std::vector<unsigned int>& topVertices) {

    size_t vertexCount = baseVertices.size();

    for (size_t i = 0; i < vertexCount; ++i) {

        size_t next = (i + 1) % vertexCount;

        Polygon side;

        side.vertices = {
            baseVertices[i],
            baseVertices[next],
            topVertices[next],
            topVertices[i]
        };

        currentMesh->addPolygon(side);
    }
}
