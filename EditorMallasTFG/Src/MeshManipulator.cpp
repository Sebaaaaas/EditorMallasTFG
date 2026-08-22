#include "MeshManipulator.h"

#include "Mesh.h"
#include "Ray.h"
#include "Camera.h"
#include "Selector.h"

MeshManipulator::MeshManipulator(Selector* selector_) {

    dragPlaneNormal = glm::vec3(0.f, 0.f, 0.f);
    dragStartPoint = glm::vec3(0.f, 0.f, 0.f);

    ray = nullptr;
    selector = selector_; // No es necesario borrar selector, ya se encarga el editor
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

    if (selector->getSelectedGroups().empty())
        return;

    dragging = true;

    transformStartMouse = glm::vec2(mouseX, mouseY);

    refreshSelectionSnapshot();

    dragPlaneNormal = camera.getPosition() - transformPivot;

    // Encontramos el punto real donde el rayo del click inicial corta el plano de arrastre, para que el delta de traslacion 
    // empiece en cero sin importar donde dentro de la seleccion se haya hecho click
    glm::vec3 rayDir = ray->mouseRay(mouseX, mouseY, w, h, camera.getViewMatrix(), camera.getProjectionMatrix());
    glm::vec3 rayOrigin = camera.getPosition();

    dragStartPoint = ray->intersectRayPlane(rayOrigin, rayDir, transformPivot, dragPlaneNormal);

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

bool MeshManipulator::isDragging() const {
    return dragging;
}

void MeshManipulator::setTransformMode(TransformMode mode) {
    transformMode = mode;
}

void MeshManipulator::setTransformAxis(TransformAxis axis) {
    transformAxis = axis;
}

void MeshManipulator::extrudeSelection(float distance) {

    const auto& selectedPolygons = selector->getSelectedPolygons();

    if (!currentMesh || selectedPolygons.empty())
        return;

    // Acumulamos las normales por grupo, para mover las caras en la direccion media de las normales
    std::unordered_map<unsigned int, glm::vec3> normalSum;

    for (unsigned int polygonIndex : selectedPolygons) {

        glm::vec3 normal = currentMesh->polygonNormal(polygonIndex);

        for (unsigned int vertexIndex : currentMesh->polygons[polygonIndex].vertices) {

            unsigned int group = currentMesh->vertexToGroup[vertexIndex];
            normalSum[group] += normal;
        }
    }

    // Creamos un vertice "ancla" por cada grupo extruido. Este define el nuevo grupo posicional y es el que usara la cara superior
    std::unordered_map<unsigned int, unsigned int> groupToTopAnchor;
    std::unordered_map<unsigned int, unsigned int> groupToTopGroup;

    for (auto& [group, sum] : normalSum) {

        unsigned int templateVertex = currentMesh->logicGroups[group][0];

        Vertex newVertex = currentMesh->vertices[templateVertex];
        newVertex.Position += glm::normalize(sum) * distance;

        // Creamos grupo posicional nuevo
        unsigned int anchor = currentMesh->addVertex(newVertex); 
        groupToTopAnchor[group] = anchor;
        groupToTopGroup[group] = currentMesh->vertexToGroup[anchor];
    }

    // Calculamos cuantas caras hacen uso de cada arista, para solamente crear poligonos nuevos en los bordes (no creamos poligonos extra al extruir multiples poligonos)
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

    // Reconstruimos los poligonos seleccionados
    for (unsigned int polygonIndex : selectedPolygons) {

        const Polygon base = currentMesh->polygons[polygonIndex];
        size_t count = base.vertices.size();

        // La cara superior usa el ancla de cada grupo directamente
        std::vector<unsigned int> topVertices;
        for (unsigned int v : base.vertices) {
            unsigned int group = currentMesh->vertexToGroup[v];
            topVertices.push_back(groupToTopAnchor[group]);
        }

        for (size_t i = 0; i < count; ++i) {

            size_t next = (i + 1) % count;

            unsigned int groupA = currentMesh->vertexToGroup[base.vertices[i]];
            unsigned int groupB = currentMesh->vertexToGroup[base.vertices[next]];

            // Saltamos arista interior compartida con otra cara seleccionada
            if (edgeUseCount[std::minmax(groupA, groupB)] > 1)
                continue;

            // Cada poligono lateral obtiene sus 4 vertices propios (misma posicion que el resto del grupo, pero vertice de renderizado distinto), para que no comparta normal
            // con otros poligonos
            unsigned int bottomA = currentMesh->addVertexToGroup(currentMesh->vertices[base.vertices[i]], groupA);
            unsigned int bottomB = currentMesh->addVertexToGroup(currentMesh->vertices[base.vertices[next]], groupB);

            unsigned int topA = currentMesh->addVertexToGroup(currentMesh->vertices[groupToTopAnchor[groupA]], groupToTopGroup[groupA]);
            unsigned int topB = currentMesh->addVertexToGroup(currentMesh->vertices[groupToTopAnchor[groupB]], groupToTopGroup[groupB]);

            Polygon side;
            side.vertices = { bottomA, bottomB, topB, topA };

            currentMesh->addPolygon(side);
        }

        currentMesh->polygons[polygonIndex].vertices = topVertices;
    }

    // Los vertices originales del borde pueden quedar sin uso si no los comparte ninguna cara sin seleccionar
    currentMesh->removeLooseVertices();

    currentMesh->rebuildTopology();
}

void MeshManipulator::deleteSelection() {

    if (!currentMesh)
        return;

    if (!selector->getSelectedPolygons().empty()) {
        currentMesh->deletePolygons(selector->getSelectedPolygons());
    }
    else if (!selector->getSelectedEdges().empty()) {

        std::unordered_set<unsigned int> polygonsToDelete;

        for (unsigned int edgeIndex : selector->getSelectedEdges()) {

            const Edge& edge = currentMesh->edges[edgeIndex];
            auto targetPair = std::minmax(currentMesh->vertexToGroup[edge.v0],
                currentMesh->vertexToGroup[edge.v1]);

            for (size_t i = 0; i < currentMesh->polygons.size(); ++i) {

                const std::vector<unsigned int>& verts = currentMesh->polygons[i].vertices;
                size_t count = verts.size();

                for (size_t j = 0; j < count; ++j) {

                    auto pair = std::minmax(currentMesh->vertexToGroup[verts[j]],
                        currentMesh->vertexToGroup[verts[(j + 1) % count]]);

                    if (pair == targetPair) {
                        polygonsToDelete.insert((unsigned int)i);
                        break;
                    }
                }
            }
        }

        currentMesh->deletePolygons(polygonsToDelete);
    }
    else if (!selector->getSelectedGroups().empty()) {
        currentMesh->deleteLogicGroups(selector->getSelectedGroups());
    }

    selector->clearSelection();
}

void MeshManipulator::setSelectedXPosition(double value) {

    if (selector->getSelectedGroups().empty() || dragging)
        return;

    refreshSelectionSnapshot();

    translateSelection(glm::vec3(value - transformPivot.x, 0.0f, 0.0f));
}

void MeshManipulator::setSelectedYPosition(double value) {

    if (selector->getSelectedGroups().empty() || dragging)
        return;

    refreshSelectionSnapshot();

    translateSelection(glm::vec3(0.0f, value - transformPivot.y, 0.0f));
}

void MeshManipulator::setSelectedZPosition(double value) {

    if (selector->getSelectedGroups().empty() || dragging)
        return;

    refreshSelectionSnapshot();

    translateSelection(glm::vec3(0.0f, 0.0f, value - transformPivot.z));
}

glm::vec3 MeshManipulator::selectionCenter() const {

    glm::vec3 center(0.0f);
    int count = 0;

    const auto& selectedGroups = selector->getSelectedGroups();

    for (unsigned int group : selectedGroups) {

        const std::vector<unsigned int>& groupVertices = currentMesh->logicGroups[group];

        if (groupVertices.empty())
            continue;

        unsigned int vertexIndex = groupVertices[0];

        center += currentMesh->vertices[vertexIndex].Position;
        ++count;
    }

    if (count == 0)
        return glm::vec3(0.0f);

    center /= count;

    return center;
}

void MeshManipulator::refreshSelectionSnapshot() {
    
    transformPivot = selectionCenter();

    originalPositions.clear();

    for (unsigned int group : selector->getSelectedGroups()) {
        for (unsigned int idx : currentMesh->logicGroups[group]) {
            originalPositions[idx] = currentMesh->vertices[idx].Position;
        }
    }
}

void MeshManipulator::updateTranslation(float mouseX, float mouseY, int w, int h, const Camera& camera) {

    glm::vec3 rayDir = ray->mouseRay(mouseX, mouseY, w, h, camera.getViewMatrix(), camera.getProjectionMatrix()); // !! revisar si hace de verdad falta hacer todo esto

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

    case TransformAxis::All: // En este caso no hay eje
        translateSelection(delta);
        return;

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
    case TransformAxis::All:
        rotateSelection(angle, dragPlaneNormal);
        break;
    }

}

void MeshManipulator::updateScale(float mouseX, float mouseY) {

    float dx = mouseX - transformStartMouse.x;

    // Multiplicamos por 0.01 para que no sea demasiado rapido (el 1.0 es para que cuando el delta sea 0, la escala se mantenga multiplicada por 1 y no por 0)
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

void MeshManipulator::transformSelection(const glm::mat4& transform) {

    for (unsigned int vertexGroup : selector->getSelectedGroups()) {
        for (unsigned int vertex : currentMesh->logicGroups[vertexGroup]) {

            glm::vec3 local = originalPositions[vertex] - transformPivot;

            glm::vec3 transformed = glm::vec3(transform * glm::vec4(local, 1.0f));

            currentMesh->vertices[vertex].Position = transformPivot + transformed;
        }
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

