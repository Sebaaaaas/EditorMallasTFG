#include "Selector.h"

#include "Camera.h"
#include "Mesh.h"
#include "Ray.h"

Selector::Selector() {

    currentSelectionMode = SelectionMode::Vertex;

    minSelectDistance = 10.0f;
    minEdgeDistance = 10.0f;
}

Selector::~Selector() {
}

void Selector::projectVerticesToScreen(const Mesh& mesh, int width, int height, const glm::mat4& view, const glm::mat4& projection) {

    projectedVertices.resize(mesh.logicGroups.size());

    glm::mat4 viewProjection = projection * view;

    for (size_t i = 0; i < mesh.logicGroups.size(); ++i) {
        unsigned int representative = mesh.logicGroups[i][0];
        projectedVertices[i] = worldToScreen(mesh.vertices[representative].Position, width, height, viewProjection);
    }
}

int Selector::pick(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera) {
    switch (currentSelectionMode)
    {
    case SelectionMode::Vertex:
        return pickVertex(mesh, mouseX, mouseY, width, height, camera);

    case SelectionMode::Edge:
        return pickEdge(mesh, mouseX, mouseY, width, height, camera);

    case SelectionMode::Face:
        return pickFace(mesh, mouseX, mouseY, width, height, camera);
    }

    return -1;
}

void Selector::setSelectionMode(SelectionMode newMode) {
    currentSelectionMode = newMode;
    selectedGroups.clear();
}

SelectionMode Selector::getSelectionMode() const {
    return currentSelectionMode;
}

void Selector::selectVertex(int vertexIndex, Mesh* mesh, bool additive) {

    if (!additive)
        clearSelection();

    unsigned int group = mesh->vertexToGroup[vertexIndex];

    selectedGroups.insert(group);
}

void Selector::selectEdge(unsigned int edgeIndex, Mesh* mesh, bool additive) {

    if (!additive)
        clearSelection();

    selectedEdges.insert(edgeIndex);

    const Edge& edge = mesh->edges[edgeIndex];

    selectedGroups.insert(mesh->vertexToGroup[edge.v0]);
    selectedGroups.insert(mesh->vertexToGroup[edge.v1]);
}

void Selector::selectPolygon(unsigned int polygonIndex, Mesh* mesh, bool additive) {

    if (!additive)
        clearSelection();

    selectedPolygons.insert(polygonIndex);

    const Polygon& polygon = mesh->polygons[polygonIndex];

    for (unsigned int vertex : polygon.vertices) {
        selectedGroups.insert(mesh->vertexToGroup[vertex]);
    }
}

const std::unordered_set<unsigned int>& Selector::getSelectedGroups() const {
    return selectedGroups;
}

const std::unordered_set<unsigned int>& Selector::getSelectedEdges() const {
    return selectedEdges;
}

const std::unordered_set<unsigned int>& Selector::getSelectedPolygons() const {
    return selectedPolygons;
}

void Selector::clearSelection() {

    selectedGroups.clear();
    selectedEdges.clear();
    selectedPolygons.clear();
}

bool Selector::hasSelection() const {
    return !selectedGroups.empty() || !selectedEdges.empty() || !selectedPolygons.empty();
}

ProjectedVertex Selector::worldToScreen(const glm::vec3& p, int width, int height, const glm::mat4& viewProjection) { // !! se puede seguir haciendo mas eficiente sacando cosas

    ProjectedVertex result;

    // !! No multiplicamos por model porque actualmente nuestra matriz model es la matriz identidad(mat4(1.0f)), si queremos moverlo en el mundo habra
    // que cambiar esto y aniadir la matriz model a la operacion
    glm::vec4 clipSpace = viewProjection * glm::vec4(p, 1.0f); // Pasamos p a vector4 para poder operar con las matrices

    // Si w <= 0, se encuentra detras de la camara y no sera seleccionable
    if (clipSpace.w <= 0.0f) {
        result.visible = false;
        result.depth = std::numeric_limits<float>::max();
        result.screenPosition = glm::vec2(0.0f);
        return result;
    }

    // Proceso de viewport transform (ndc: normalized device coordinates), nos deja las coordenadas en el espacio [-1,1]
    glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w;

    // Convertimos el espacio normalizado a espacio en la pantalla
    result.screenPosition.x = (ndc.x + 1.0f) * 0.5f * width;
    result.screenPosition.y = (1.0f - ndc.y) * 0.5f * height; // Aqui restamos para darle la vuelta, los sistemas de coordenadas estan al reves si no

    result.depth = ndc.z;
    result.visible = true;

    return result;
}

int Selector::pickVertex(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera) {

    int selectedVertex = -1;

    // Mejores valores encontrados hasta el momento
    float minScreenDistance = minSelectDistance;
    float minDepth = std::numeric_limits<float>::max();

    // Recorremos todos los vertices de la malla
    for (size_t group = 0; group < mesh.logicGroups.size(); ++group) {

        const ProjectedVertex& projected = projectedVertices[group];

        if (!projected.visible)
            continue;

        float dist = glm::distance(projected.screenPosition, glm::vec2(mouseX, mouseY));

        if (isBetterCandidate(dist, projected.depth, minScreenDistance, minDepth)) {
            selectedVertex = mesh.logicGroups[group][0];
        }
    }


    return selectedVertex;
}

int Selector::pickEdge(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera) {

    int selectedEdge = -1;

    float minScreenDistance = minEdgeDistance;
    float minDepth = std::numeric_limits<float>::max();

    glm::vec2 mouse(mouseX, mouseY);

    for (int i = 0; i < mesh.edges.size(); ++i) {

        const Edge& edge = mesh.edges[i];

        // Obtenemos las dos proyecciones de los vertices del segmento
        const ProjectedVertex& pa = projectedVertices[mesh.vertexToGroup[edge.v0]];
        const ProjectedVertex& pb = projectedVertices[mesh.vertexToGroup[edge.v1]];

        if (!pa.visible && !pb.visible)
            continue;

        glm::vec2 ab = pb.screenPosition - pa.screenPosition;

        // Calculo de longitud del vector ab(hacerlo asi ahorra raices cuadradas)
        float len = glm::dot(ab, ab);

        // Ingoramos si es muy pequenio !! igual ni hace falta
        if (len < 0.00001f)
            continue;

        // Determinamos la posicion del raton t sobre el segmento, que resulta en valores entre 0(sobre a) y 1(sobre b)
        float t = glm::dot(mouse - pa.screenPosition, ab) / len;
        t = glm::clamp(t, 0.0f, 1.0f); // Por si da un valor fuera del rango, lo mantenemos dentro de el

        // Punto del segmento mas cercano al raton
        glm::vec2 closest = pa.screenPosition + t * ab;

        float dist = glm::distance(mouse, closest);
        float depth = pa.depth * (1.0f - t) + pb.depth * t;

        if (isBetterCandidate(dist, depth, minScreenDistance, minDepth)) {
            selectedEdge = i;
        }
    }

    return selectedEdge;
}

int Selector::pickFace(const Mesh& mesh, float mouseX, float mouseY, int width, int height, Camera* camera) {

    int selectedPolygon = -1;
    float bestDepth = std::numeric_limits<float>::max();

    glm::vec2 mouse(mouseX, mouseY);

    // Recorremos poligonos
    for (int i = 0; i < mesh.polygons.size(); ++i) {

        const Polygon& polygon = mesh.polygons[i];

        // Por si acaso hay poligonos erroneos con menos de 3 vertices
        if (polygon.vertices.size() < 3)
            continue;

        // Si el poligono no tiene vertices visibles, lo ignoramos, con que uno sea visible, cuenta como candidato
        bool visible = false;

        for (unsigned int idx : polygon.vertices) {
            if (projectedVertices[mesh.vertexToGroup[idx]].visible) {
                visible = true;
                break;
            }
        }

        if (!visible)
            continue;
        
        // Recorremos los vertices del poligono, creando triangulos a partir del vertice inicial seleccionado
        const ProjectedVertex& pa = projectedVertices[mesh.vertexToGroup[polygon.vertices[0]]];

        for (int j = 1; j + 1 < polygon.vertices.size(); ++j) {

            const ProjectedVertex& pb = projectedVertices[mesh.vertexToGroup[polygon.vertices[j]]];
            const ProjectedVertex& pc = projectedVertices[mesh.vertexToGroup[polygon.vertices[j + 1]]];

            float u, v;

            if (pointInTriangle(mouse, pa.screenPosition, pb.screenPosition, pc.screenPosition, u, v)) {

                float w = 1.0f - u - v;

                float depth = (w * pa.depth +
                        u * pb.depth +
                        v * pc.depth);

                if (depth < bestDepth) {

                    bestDepth = depth;
                    selectedPolygon = i;
                }
            }
        }
    }

    return selectedPolygon;
}

bool Selector::isBetterCandidate(float distance, float depth, float& bestDistance, float& bestDepth) const {
    
    if (distance < bestDistance - distanceEpsilon) {
        bestDistance = distance;
        bestDepth = depth;
        return true;
    }

    if (std::abs(distance - bestDistance) <= distanceEpsilon && depth < bestDepth) {
        bestDistance = distance;
        bestDepth = depth;
        return true;
    }

    return false;
}

bool Selector::pointInTriangle(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, float& u, float& v) {

    glm::vec2 v0 = c - a;
    glm::vec2 v1 = b - a;
    glm::vec2 v2 = p - a;

    float dot00 = glm::dot(v0, v0);
    float dot01 = glm::dot(v0, v1);
    float dot02 = glm::dot(v0, v2);
    float dot11 = glm::dot(v1, v1);
    float dot12 = glm::dot(v1, v2);

    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);

    u = (dot11 * dot02 - dot01 * dot12) * invDenom;

    v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    return (u >= 0.0f) && (v >= 0.0f) && (u + v <= 1.0f);

}